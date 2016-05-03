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
//__align(8) CPU_STK	PRESSURE_TASK_STK[PRESSURE_STK_SIZE];	//任务堆栈
CPU_STK	PRESSURE_TASK_STK[PRESSURE_STK_SIZE];				//任务堆栈

OS_TCB Safety_Task_TCB;										//任务控制块
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
				 		 
	OS_CRITICAL_EXIT();	//进入临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}

//IMU任务函数
void imu_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		IMU_Prepare_Data();
		IMU_Update();
		OSTimeDlyHMSM(0,0,0,3,OS_OPT_TIME_HMSM_STRICT,&err); //延时3ms
		//Data_Send_To_PC();
	}
}

//CONTROL任务函数
void control_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		if(imu_calibrate_flag == 1)
		{
			if(Remote_Control_Is_Calibrate())
			{
				imu_calibrate_flag = 0;
				not_calibrate = 1;
			}					
		}
		
		Remote_Control_PWM_Convert();			
		Control();
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
	}
}

//PRESSURE任务
void pressure_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		ms5611_ms++;

		if(ms5611_ms == 2)
		{
			if(ms5611_status == 1) 
			{
				MS5611_Pressure_Calculate();
				MS5611_Altitude_Calculate();
			}
			MS5611_TemperatureADC_Conversion();
		}
		if(ms5611_ms == 4)
		{
			MS5611_Temperature_Calculate();
			MS5611_PressureADC_Conversion();
			ms5611_status = 1;
			ms5611_ms = 0;
		}
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err); //延时50ms
	}
}

//任务函数
void safety_task(void *p_arg)	
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		if((++battery_count) * 100 >= BATTERY_VOLTAGE_CHECK_COUNT) 
		{
			battery_count = 0; 
			lost_remote_flag = 0;
			Battery_Voltege_Check();
		}
		
		Aid_Control_Crash();
		
		Aid_Control_Led_Alarm();
		
		LED_Flash();
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //延时100ms
	}
}
