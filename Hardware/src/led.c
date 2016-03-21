#include "led.h"

LED_Buffer LED_Buffer_B;
LED_Event_E LED_Control;

void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);	 
															  //使能PA,PB端口时钟以及使用复用时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_3;	  //LED0-->PB.1 LED1-->PB.3端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					  //根据设定参数初始化GPIOB.1  GPIOB.3
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);   //关闭JTAG，保留SWD   PB.3用作普通的IO口
	GPIO_SetBits(GPIOB,GPIO_Pin_1|GPIO_Pin_3);				  //PB.1 PB.3 输出高

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_11;	  //LED2-->PA.8 LED3-->PA.11 端口配置, 推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				  //推挽输出 ，IO口速度为50MHz
	GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_11); 			  //PA.8 PA.11 输出高
	
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
}

void LED_Bottom_Flash(void)
{

    if(LED_Buffer_B.bits.LED_1)
        LED1_ON;
    else
        LED1_OFF;

    if(LED_Buffer_B.bits.LED_2)
        LED2_ON;
    else
        LED2_OFF;

    if(LED_Buffer_B.bits.LED_3)
        LED3_ON;
    else
        LED3_OFF;

    if(LED_Buffer_B.bits.LED_4)
        LED4_ON;
    else
        LED4_OFF;

}

void LED_Flash(void)
{
	switch(LED_Control.event)
	{
		case Event_Ready:
			if(++LED_Control.cnt >= 3)
				LED_Control.cnt=0;
			if(LED_Control.cnt==0)
				LED_Buffer_B.byte =LED1|LED2;
			else
				LED_Buffer_B.byte =0;
			break;
		case Event_Calibration:
			LED_Buffer_B.byte=LED1|LED2;
			break;
		case Event_Low_Voltage:
			if(++LED_Control.cnt >= 3)
				LED_Control.cnt=0;
			if(LED_Control.cnt==0)
				LED_Buffer_B.byte =0x0f;
			else
				LED_Buffer_B.byte =0;
			break;
		case Event_Calibration_Fail:
			if(++LED_Control.cnt >= 4)
				LED_Control.cnt=0;
			if(LED_Control.cnt<2)
				LED_Buffer_B.byte =LED3|LED4;
			else
				LED_Buffer_B.byte =LED1|LED2;
			break;
		case Event_Lost_Remote:
			if(++LED_Control.cnt >= 4)
				LED_Control.cnt=0;
			LED_Buffer_B.byte= 1<<LED_Control.cnt;
			break;
		case Event_Auto_Land:
			LED_Buffer_B.byte=0x0f;
			break;
		case Event_Batter_Charge:
			LED_Buffer_B.byte=0x00;
			break;
	}
	LED_Bottom_Flash();
}
