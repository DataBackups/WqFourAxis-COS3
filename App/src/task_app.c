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
//__align(8) CPU_STK	PRESSURE_TASK_STK[PRESSURE_STK_SIZE];	//�����ջ
CPU_STK	PRESSURE_TASK_STK[PRESSURE_STK_SIZE];				//�����ջ

OS_TCB Safety_Task_TCB;										//������ƿ�
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

//IMU������
void imu_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		IMU_Prepare_Data();
		IMU_Update();
		OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1ms
		//Data_Send_To_PC();
		
//		LED1_ON;
//		LED2_ON;
//		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
//		LED1_OFF;
//		LED2_OFF;
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}

//CONTROL������
void control_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		if(imu_calibrate_flag == 1)
		{
			imu_calibrate_flag = 0;
			Remote_Control_Is_Calibrate();
			not_calibrate = 1;
		}
		
		Remote_Control_PWM_Convert();			
		Control();
		OSTimeDlyHMSM(0,0,0,2,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ2ms
//		LED3_ON;
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
//		LED3_OFF;
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}

//PRESSURE����
void pressure_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
//	CPU_SR_ALLOC();
//	static float float_num=0.01;
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
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
//		float_num+=0.01f;
//		if(float_num > 0.100000)
//		{
//			float_num = 0.01;
//		}
//		OS_CRITICAL_ENTER();	//�����ٽ���
//		printf("float_num��ֵΪ: %.4f\r\n",float_num);
//		OS_CRITICAL_EXIT();		//�˳��ٽ���
//		delay_ms(500);			//��ʱ500ms
	}
}

//������
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
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
//		LED4_ON;
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
//		LED4_OFF;
//		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}
