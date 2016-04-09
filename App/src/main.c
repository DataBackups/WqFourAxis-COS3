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
	Motor_Init(999,2);      						//PWM�����ʼ�������PWMƵ��24000Hz
	while(MPU6050_Init());
	TIM3_Int_Init(1000,72);
	IMU_Date_Init(); 								//ÿ�ν������ȳ�ʼ����������
	MPU6050_Date_Offset(5000);		
	MS5611_Init();
	while(1)
	{
		delay_ms(500);
	}
}
