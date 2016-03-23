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
	FLASH_Unlock();									//����flash,�Ƿ�ȫ���࿼��
	EE_Init();										//��ʼ��ģ��eeprom����
	Flash_Memory_Init();							//������ʼ��
	delay_init();	    	 						//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 							//���ڳ�ʼ��Ϊ115200
	Battery_Voltage_ADC_Init();  
	LED_Init();
	Motor_Init(999,2);      						//PWM�����ʼ�������PWMƵ��24000Hz
	while(MPU6050_Init());
	//Motor_PWM_Flash(100,100,100,100);
	LED_Control.event = Event_Batter_Charge;
	TIM3_Int_Init(1000,72);
	IMU_Date_Init(); 								//ÿ�ν������ȳ�ʼ����������
	MPU6050_Date_Offset(5000);		
	while(1)
	{
//		IMU_Prepare_Data();
//		IMU_Update();
//		temp = Battery_Voltage_ReadValue();
//		printf("��ѹֵ= %0.2f\n",temp);
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
