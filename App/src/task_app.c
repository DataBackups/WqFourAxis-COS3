#include "task_app.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "battery_voltage.h"
#include "led.h"
#include "motor.h"
#include "mpu6050.h"
#include "imu.h"
#include "timer.h"
#include "eeprom.h"
#include "flash_memory.h"
#include "ms5611.h"
#include "remote_control.h"
#include "control.h"
#include "data_pc.h"
#include "aid_control.h"

uint16_t battery_count = 0;

extern OS_TCB StartTaskTCB;									//任务控制块
extern CPU_STK START_TASK_STK[START_STK_SIZE];				//任务堆栈	

OS_TCB Imu_Task_TCB;										//任务控制块
CPU_STK IMU_TASK_STK[IMU_STK_SIZE];							//任务堆栈	

OS_TCB Control_Task_TCB;									//任务控制块
CPU_STK CONTROL_TASK_STK[CONTROL_STK_SIZE];					//任务堆栈	

OS_TCB	Pressure_Task_TCB;									//任务控制块
__align(8) CPU_STK	PRESSURE_TASK_STK[PRESSURE_STK_SIZE];	//任务堆栈

OS_TCB Safety_Task_TCB;									//任务控制块
CPU_STK SAFETY_TASK_STK[SAFETY_STK_SIZE];					//任务堆栈

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//进入临界区
	//创建IMU任务
	OSTaskCreate((OS_TCB 	* )&Imu_Task_TCB,		
				 (CPU_CHAR	* )"imu task", 		
                 (OS_TASK_PTR )imu_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )IMU_TASK_PRIO,     
                 (CPU_STK   * )&IMU_TASK_STK[0],	
                 (CPU_STK_SIZE)IMU_STK_SIZE/10,	
                 (CPU_STK_SIZE)IMU_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	//创建CONTROL任务
	OSTaskCreate((OS_TCB 	* )&Control_Task_TCB,		
				 (CPU_CHAR	* )"control task", 		
                 (OS_TASK_PTR )control_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )CONTROL_TASK_PRIO,     	
                 (CPU_STK   * )&CONTROL_TASK_STK[0],	
                 (CPU_STK_SIZE)CONTROL_STK_SIZE/10,	
                 (CPU_STK_SIZE)CONTROL_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	
			 
	//创建PRESSURE测试任务
	OSTaskCreate((OS_TCB 	* )&Pressure_Task_TCB,		
				 (CPU_CHAR	* )"pressure test task", 		
                 (OS_TASK_PTR )pressure_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )PRESSURE_TASK_PRIO,     	
                 (CPU_STK   * )&PRESSURE_TASK_STK[0],	
                 (CPU_STK_SIZE)PRESSURE_STK_SIZE/10,	
                 (CPU_STK_SIZE)PRESSURE_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);				

	//创建SAFETY任务
	OSTaskCreate((OS_TCB 	* )&Safety_Task_TCB,		
				 (CPU_CHAR	* )"safety task", 		
                 (OS_TASK_PTR )safety_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )SAFETY_TASK_PRIO,     	
                 (CPU_STK   * )&SAFETY_TASK_STK[0],	
                 (CPU_STK_SIZE)SAFETY_STK_SIZE/10,	
                 (CPU_STK_SIZE)SAFETY_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//进入临界区
}

//led0任务函数
void imu_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED1_ON;
		LED2_ON;
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
		LED1_OFF;
		LED2_OFF;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}

//led1任务函数
void control_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED3_ON;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
		LED3_OFF;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}

//浮点测试任务
void pressure_task(void *p_arg)
{
	CPU_SR_ALLOC();
	static float float_num=0.01;
	while(1)
	{
		float_num+=0.01f;
		if(float_num > 0.100000)
		{
			float_num = 0.01;
		}
		OS_CRITICAL_ENTER();	//进入临界区
		printf("float_num的值为: %.4f\r\n",float_num);
		OS_CRITICAL_EXIT();		//退出临界区
		delay_ms(500);			//延时500ms
	}
}

//任务函数
void safety_task(void *p_arg)	
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED4_ON;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
		LED4_OFF;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}
