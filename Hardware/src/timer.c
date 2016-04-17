#include "timer.h"
#include "usart.h"
#include "ms5611.h"
#include "imu.h"
#include "remote_control.h"
#include "control.h"
#include "battery_voltage.h"
#include "motor.h"
#include "includes.h"

volatile s16 system_time = 0;//��λ��S
volatile u16 cnt = 0;
volatile u16 ms = 0;
volatile u16 ms5611_ms = 0;
volatile u16 ms5611_status = 0;
volatile u16 loop_500Hz_conut,loop_100Hz_conut = 0,loop_50Hz_conut = 0,loop_10Hz_conut = 0;

u8 loop_500Hz_flag,loop_100Hz_flag,loop_50Hz_flag,loop_10Hz_flag;

/*
 * ��������TIM3_Int_Init
 * ����  ��ͨ�ö�ʱ��3�жϳ�ʼ��,����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
 * ����  ��arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
 * ���  ����
 */  
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 			//ʱ��ʹ��
	
																	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr - 1; 					//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1; 					//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 				//����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 						//ʹ��ָ����TIM3�ж�,��������ж�

																	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  				//TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  		//��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  			//�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  								//��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  										//ʹ��TIMx	
	printf("Timer 3 init success...\r\n");	
}

/*
 * ��������TIM3_IRQHandler
 * ����  ����ʱ��3�жϷ������
 * ����  ����
 * ���  ����
 */  
void TIM3_IRQHandler(void)   //TIM3�ж�
{
#if SYSTEM_SUPPORT_OS 												//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  			//���TIM3�����жϷ������
	{			
		loop_500Hz_conut++;
		loop_100Hz_conut++;
		loop_50Hz_conut++;
		loop_10Hz_conut++;
		if(loop_500Hz_conut == 2)
		{
			loop_500Hz_conut = 0;
			loop_500Hz_flag = 1;
		}		
		if(loop_100Hz_conut == 10)
		{
			loop_100Hz_conut = 0;
			loop_100Hz_flag = 1;
		}
		if(loop_50Hz_conut == 20)
		{
			loop_50Hz_conut = 0;
			loop_50Hz_flag = 1;
		}
		if(loop_10Hz_conut == 100)
		{
			loop_10Hz_conut = 0;
			loop_10Hz_flag = 1;
		}
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  				//���TIMx�����жϱ�־ 	
	}
#if SYSTEM_SUPPORT_OS 												//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}
