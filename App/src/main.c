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

int main(void)
{	 
	FLASH_Unlock();									//解锁flash,是否安全更多考虑
	EE_Init();										//初始化模拟eeprom设置
	Flash_Memory_Init();							//参数初始化
	delay_init();	    	 						//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
#if 1
	uart_init(115200);	 							//串口初始化为115200  蓝牙控制
#else
	uart_init(460800);								//串口初始化为460800  PC调试
#endif
	Battery_Voltage_ADC_Init();  
	LED_Init();
	Motor_Init(3999,35);      						//PWM输出初始化，电机PWM频率500Hz
	while(MPU6050_Init());							
	TIM3_Int_Init(1000,72);							//1ms中断一次	
	MS5611_Init();
	while(1)
	{
		IMU_Prepare_Data();
		IMU_Update();			//姿态更新频率为1KHz
		if(loop_500Hz_flag)
		{
			loop_500Hz_flag = 0;
			//Data_Send_To_PC();
		}
		if(loop_100Hz_flag)
		{
			loop_100Hz_flag = 0;
			
			if(imu_calibrate_flag == 1)
			{
				imu_calibrate_flag = 0;
				Remote_Control_Is_Calibrate();
				not_calibrate = 1;
			}
			
			Remote_Control_PWM_Convert();			
			Control();
		}
		
		if(loop_50Hz_flag)
		{
			loop_50Hz_flag = 0;
			
			ms5611_ms++;
	
			if(ms5611_ms == 20)
			{
				if(ms5611_status == 1) 
				{
					MS5611_Pressure_Calculate();
					MS5611_Altitude_Calculate();
				}
				MS5611_TemperatureADC_Conversion();
			}
			if(ms5611_ms == 40)
			{
				MS5611_Temperature_Calculate();
				MS5611_PressureADC_Conversion();
				ms5611_status = 1;
				ms5611_ms = 0;
			}			
		}
		
		if(loop_10Hz_flag)
		{
			loop_10Hz_flag = 0;
			if((++battery_count) * 100 >= BATTERY_VOLTAGE_CHECK_COUNT) 
			{
				battery_count = 0; 
				lost_remote_flag = 0;
				Battery_Voltege_Check();
			}
			
			
			
			Aid_Control_Crash();
			
			Aid_Control_Led_Alarm();
			
			LED_Flash();
		}
	}
}
