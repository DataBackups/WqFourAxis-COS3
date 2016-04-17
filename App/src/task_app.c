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

extern OS_TCB StartTaskTCB;									//������ƿ�
extern CPU_STK START_TASK_STK[START_STK_SIZE];				//�����ջ	

OS_TCB Imu_Task_TCB;										//������ƿ�
CPU_STK IMU_TASK_STK[IMU_STK_SIZE];							//�����ջ	

OS_TCB Control_Task_TCB;									//������ƿ�
CPU_STK CONTROL_TASK_STK[CONTROL_STK_SIZE];					//�����ջ	

OS_TCB	Pressure_Task_TCB;									//������ƿ�
__align(8) CPU_STK	PRESSURE_TASK_STK[PRESSURE_STK_SIZE];	//�����ջ

OS_TCB Safety_Task_TCB;									//������ƿ�
CPU_STK SAFETY_TASK_STK[SAFETY_STK_SIZE];					//�����ջ

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	//����IMU����
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
				 
	//����CONTROL����
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
			 
	//����PRESSURE��������
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

	//����SAFETY����
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
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}

//led0������
void imu_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED1_ON;
		LED2_ON;
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
		LED1_OFF;
		LED2_OFF;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}

//led1������
void control_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED3_ON;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
		LED3_OFF;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}

//�����������
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
		OS_CRITICAL_ENTER();	//�����ٽ���
		printf("float_num��ֵΪ: %.4f\r\n",float_num);
		OS_CRITICAL_EXIT();		//�˳��ٽ���
		delay_ms(500);			//��ʱ500ms
	}
}

//������
void safety_task(void *p_arg)	
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED4_ON;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
		LED4_OFF;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}
