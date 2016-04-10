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
	TIM3_Int_Init(1000,72);
	IMU_Date_Init(); 								//每次解锁后都先初始化导航数据
	MPU6050_Date_Offset(5000);		
	MS5611_Init();
	while(1)
	{
		if(loop_100Hz_flag)
		{
			
		}
		if(loop_50Hz_flag)
		{
			
		}
		if(loop_10Hz_flag)
		{
			
		}
	}
}
