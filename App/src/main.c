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


u8 unlock_flag;
u8 calibrate_status;
float angle_z;
s32 height;
u8 rc_channel_5__status;
u8 rc_channel_6__status;

extern float mpu6500_tempreature;

int main(void)
{	 
	float temp;
	s16 temp1[1];
	s16 i = 0;
	FLASH_Unlock();									//解锁flash,是否安全更多考虑
	EE_Init();										//初始化模拟eeprom设置
	Flash_Memory_Init();							//参数初始化
	delay_init();	    	 						//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 							//串口初始化为115200
	Battery_Voltage_ADC_Init();  
	LED_Init();
	Motor_Init(999,2);      						//PWM输出初始化，电机PWM频率24000Hz
	while(MPU6050_Init());
	//Motor_PWM_Flash(100,100,100,100);
	LED_Control.event = Event_Batter_Charge;
	TIM3_Int_Init(1000,72);
	IMU_Date_Init(); 								//每次解锁后都先初始化导航数据
	MPU6050_Date_Offset(5000);		
	while(1)
	{
//		IMU_Prepare_Data();
//		IMU_Update();
//		temp = Battery_Voltage_ReadValue();
//		printf("电压值= %0.2f\n",temp);
//		printf("\n");
//		printf("roll = %5.2f\n",Angle.X);
//		printf("\n");
//		printf("pitch = %5.2f\n",Angle.Y);
//		printf("\n");
//		printf("yaw = %5.2f\n",Angle.Z);
//		printf("\n");
//		printf("temp = %f\n",mpu6500_tempreature);
//		printf("\n");
		LED_Flash();
//		while(i < 10)
//		{
//			EE_WriteVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_ACC_X_OFFSET],i++);
//			EE_ReadVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_ACC_X_OFFSET], &temp1[0]);
//			printf("temp = %d\n",(int)(temp1[0]));
//			printf("\n");		
//			delay_ms(500);
//		}
		delay_ms(500);
	}
}
