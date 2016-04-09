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
	FLASH_Unlock();									//解锁flash,是否安全更多考虑
	EE_Init();										//初始化模拟eeprom设置
	Flash_Memory_Init();							//参数初始化
	delay_init();	    	 						//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 							//串口初始化为115200
	Battery_Voltage_ADC_Init();  
	LED_Init();
	Motor_Init(3999,35);      						//PWM输出初始化，电机PWM频率500Hz
	while(MPU6050_Init());
	TIM3_Int_Init(1000,72);
	IMU_Date_Init(); 								//每次解锁后都先初始化导航数据
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
		if(lock_unlock_flag)//判断是否解锁与是否在执行解锁动作	
		{		
											//判断是否在执行上锁动作	
			IMU_Update();					//姿态更新频率为1KHz
			ms++;
			
			if((ms % 2) == 0)	 			//控制频率500Hz
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
			
			if(Battery_Voltage_ReadValue() < 3.1)//如果电池电压小于3.1V，则快闪提示报警(电机转动会拉低电压)
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
	
		else																	 //未解锁
		{
			Motor_PWM_Flash(0,0,0,0);					 //确保安全
			if(Battery_Voltage_ReadValue() < 3.65)//如果电池电压小于3.65V，则快闪提示报警
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
