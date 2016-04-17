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

//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];

void APP_System_Init(void);

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	APP_System_Init();
		
	OSInit(&err);									//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();							//�����ٽ���
													//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();								//�˳��ٽ���	 
	OSStart(&err);  								//����UCOSIII
	while(1);
}

void APP_System_Init(void)
{
	FLASH_Unlock();									//����flash,�Ƿ�ȫ���࿼��
	EE_Init();										//��ʼ��ģ��eeprom����
	Flash_Memory_Init();							//������ʼ��
	delay_init();	    	 						//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������,�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
#if 1
	uart_init(115200);	 							//���ڳ�ʼ��Ϊ115200  ��������
#else
	uart_init(460800);								//���ڳ�ʼ��Ϊ460800  PC����
#endif
	Battery_Voltage_ADC_Init();  
	LED_Init();
	Motor_Init(3999,35);      						//PWM�����ʼ�������PWMƵ��500Hz
	while(MPU6050_Init());							
	TIM3_Int_Init(1000,72);							//1ms�ж�һ��	
	MS5611_Init();
}
