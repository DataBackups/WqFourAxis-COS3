#include "motor.h"

void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    uint16_t PrescalerValue = 0;    								//控制电机PWM频率
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 			//打开外设A的时钟和复用时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 ,ENABLE);   			//打开定时器2时钟  
    
																	// 设置GPIO功能。
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    																
    TIM_DeInit(TIM2);												// 复位定时器。  
																	// 配置计时器 
    PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;   //电机PWM频率24000Hz
    //Tout= ((ARR+1)*(PSC+1))/Tclk,  Tclk： TIMx 的输入时钟频率（单位为 Mhz）,Tout： TIMx 溢出时间（单位为 us）
    TIM_TimeBaseStructure.TIM_Period = 999;		            		//(ARR)计数上线	
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;			//PWM(PSC)时钟分频
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;	
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	//向上计数
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
      
																	// 配置TIM2为PWM输出模式
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
																	
    TIM_Cmd(TIM2,ENABLE);											// 启动计时器。
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
    if(MOTO4_PWM<=0)	MOTO4_PWM = 0;//限定输入不能小于0，大于999

    TIM2->CCR1 = MOTO1_PWM;
    TIM2->CCR2 = MOTO2_PWM;
    TIM2->CCR3 = MOTO3_PWM;
    TIM2->CCR4 = MOTO4_PWM;        //对定时器寄存器赋值
}
