#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "battery_voltage.h"
#include "led.h"
#include "motor.h"
#include "mpu6050.h"
#include "imu.h"
#include "timer.h"

u8 calibrate_status;
volatile S_FLOAT_XYZ Exp_Angle;
extern float mpu6500_tempreature;

int main(void)
{	 
	float temp;
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
		IMU_Prepare_Data();
		IMU_Update();
		temp = Battery_Voltage_ReadValue();
		printf("��ѹֵ= %0.2f\n",temp);
		printf("\n");
		printf("roll = %5.2f\n",Angle.X);
		printf("\n");
		printf("pitch = %5.2f\n",Angle.Y);
		printf("\n");
		printf("yaw = %5.2f\n",Angle.Z);
		printf("\n");
		printf("temp = %f\n",mpu6500_tempreature);
		printf("\n");
		LED_Flash();
		delay_ms(500);
	}
}
