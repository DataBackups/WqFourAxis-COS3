#include "motor.h"

void Motor_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    //uint16_t PrescalerValue = 0;    								//���Ƶ��PWMƵ��
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 			//������A��ʱ�Ӻ͸���ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 ,ENABLE);   			//�򿪶�ʱ��2ʱ��  
    
																	// ����GPIO���ܡ�
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    																
    TIM_DeInit(TIM2);												// ��λ��ʱ����  
																	// ���ü�ʱ�� 
    //PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;   //���PWMƵ��24000Hz
    //Tout= ((ARR+1)*(PSC+1))/Tclk,  Tclk�� TIMx ������ʱ��Ƶ�ʣ���λΪ Mhz��,Tout�� TIMx ���ʱ�䣨��λΪ us��
    TIM_TimeBaseStructure.TIM_Period = arr;		            		//(ARR)��������	
    TIM_TimeBaseStructure.TIM_Prescaler = psc;			//PWM(PSC)ʱ�ӷ�Ƶ
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;	
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	//���ϼ���
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
      
																	// ����TIM2ΪPWM���ģʽ
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;    
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    TIM_OC1Init(TIM2,&TIM_OCInitStructure);
    TIM_OC2Init(TIM2,&TIM_OCInitStructure);
    TIM_OC3Init(TIM2,&TIM_OCInitStructure);
    TIM_OC4Init(TIM2,&TIM_OCInitStructure);
    
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
																	
    TIM_Cmd(TIM2,ENABLE);											// ������ʱ����
}

void Motor_PWM_Flash(s16 MOTO1_PWM,s16 MOTO2_PWM,s16 MOTO3_PWM,s16 MOTO4_PWM)
{
    if(MOTO1_PWM>=Motor_PWM_Max)	MOTO1_PWM = Motor_PWM_Max;
    if(MOTO2_PWM>=Motor_PWM_Max)	MOTO2_PWM = Motor_PWM_Max;
    if(MOTO3_PWM>=Motor_PWM_Max)	MOTO3_PWM = Motor_PWM_Max;
    if(MOTO4_PWM>=Motor_PWM_Max)	MOTO4_PWM = Motor_PWM_Max;
    if(MOTO1_PWM<=0)	MOTO1_PWM = 0;
    if(MOTO2_PWM<=0)	MOTO2_PWM = 0;
    if(MOTO3_PWM<=0)	MOTO3_PWM = 0;
    if(MOTO4_PWM<=0)	MOTO4_PWM = 0;//�޶����벻��С��0������999

    TIM2->CCR1 = MOTO1_PWM;
    TIM2->CCR2 = MOTO2_PWM;
    TIM2->CCR3 = MOTO3_PWM;
    TIM2->CCR4 = MOTO4_PWM;        //�Զ�ʱ���Ĵ�����ֵ
}
