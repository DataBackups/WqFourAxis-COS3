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


int main(void)
{	 
	FLASH_Unlock();									//����flash,�Ƿ�ȫ���࿼��
	EE_Init();										//��ʼ��ģ��eeprom����
	Flash_Memory_Init();							//������ʼ��
	delay_init();	    	 						//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 							//���ڳ�ʼ��Ϊ115200
	Battery_Voltage_ADC_Init();  
	LED_Init();
	Motor_Init(3999,35);      						//PWM�����ʼ�������PWMƵ��500Hz
	while(MPU6050_Init());
	TIM3_Int_Init(1000,72);
	IMU_Date_Init(); 								//ÿ�ν������ȳ�ʼ����������
	MPU6050_Date_Offset(5000);		
	MS5611_Init();
	while(1)
	{
//		if(ms5611_ms == 20)
//		{
//			if(ms5611_status == 1) 
//			{
//				MS5611_Pressure_Calculate();
//				MS5611_Altitude_Calculate();
//			}
//			MS5611_TemperatureADC_Conversion();
//		}
//		if(ms5611_ms == 40)
//		{
//			MS5611_Temperature_Calculate();
//			MS5611_PressureADC_Conversion();
//			ms5611_status = 1;
//			ms5611_ms = 0;
//		}
		IMU_Prepare_Data();  
		if(lock_unlock_flag)//�ж��Ƿ�������Ƿ���ִ�н�������	
		{		
											//�ж��Ƿ���ִ����������	
			IMU_Update();					//��̬����Ƶ��Ϊ1KHz
			ms++;
			
			if((ms % 2) == 0)	 			//����Ƶ��500Hz
			{ 
				Remote_Control_PWM_Convert();
				//Data_Send();
				Control();	
			}			
					
			if(ms == 1000)
			{
				system_time++;
				ms = 0;	
			}		
			
			if(Battery_Voltage_ReadValue() < 3.1)//�����ص�ѹС��3.1V���������ʾ����(���ת�������͵�ѹ)
			{
				if(cnt % 50 == 0)	
				{	
					cnt = 0;
				}	
			}			
			else if(cnt % 500 == 0)	
			{	
				cnt = 0;
			}	
		}
	
		else																	 //δ����
		{
			Motor_PWM_Flash(0,0,0,0);					 //ȷ����ȫ
			if(Battery_Voltage_ReadValue() < 3.65)//�����ص�ѹС��3.65V���������ʾ����
			{
				if(cnt % 50 == 0)	
				{	
					cnt = 0;
				}	
			}	
			else if(cnt % 1000 == 0)	
			{	
				cnt = 0;
			}	
		}			
	}
}
