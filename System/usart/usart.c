#include "sys.h"
#include "usart.h"	  
#include "led.h"
#include "remote_control.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
													//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
													//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
													//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);					//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   									//���ʹ���˽���

													//���� ����ṹ��ʵ������������
Usart_Buffer Usart_TX_Buffer;						//���η��ͽṹ��
Usart_Buffer Usart_RX_Buffer;						//���ν��սṹ��

uint8_t RX_buffer[RX_BUFFER_SIZE];
uint8_t TX_buffer[TX_BUFFER_SIZE];

/*
 * ��������Usart_Buffer_Read_Data
 * ����  ����ȡ���������е�һ���ֽ�
 * ����  ��Ring_Buffer�����뻷�η��ͽṹ��
 * ���  ��Ҫ�Ӵ��ڷ��͵�����
 */
uint8_t Usart_Buffer_Read_Data(Usart_Buffer *Ring_Buffer)
{
	uint8_t temp;
	temp = Ring_Buffer->P_buffer[Ring_Buffer->Read_Index & Ring_Buffer->Mask];	//���ݳ����������Ҫ�����Ǿ������ݻ��εĹؼ�
	Ring_Buffer->Read_Index++;													//��ȡ���һ�Σ���ָ���1��Ϊ��һ�� ��ȡ�� ׼��
	return temp;
}

/*
 * ��������Usart_Buffer_Write_Data
 * ����  ����һ���ֽ�д��һ�����νṹ����
 * ����  ��Ring_Buffer�����뻷�ν��սṹ�� ��Data_In���Ӵ��ڽ��յ�����
 * ���  ����
 */
void Usart_Buffer_Write_Data(Usart_Buffer *Ring_Buffer,uint8_t Data_In)
{
	Ring_Buffer->P_buffer[Ring_Buffer->Write_Index & Ring_Buffer->Mask] = Data_In;	//���ݳ����������Ҫ�����Ǿ������ݻ��εĹؼ�
	Ring_Buffer->Write_Index++;														//д��һ�Σ�дָ���1��Ϊ��һ��д����׼��
}

/*
 * ��������Usart_Buffer_Cnt
 * ����  ����дָ��д��һȦ��׷���˶�ָ�룬��ô֤������д���ˣ���ʱӦ�����ӻ��������ȣ�����������Χ���ݴ���ʱ��
 * ����  ��Ring_Buffer�����뻷�η��ͽṹ��
 * ���  �����ػ����������Ŀ����ֽڳ���
 */
uint16_t Usart_Buffer_Cnt(Usart_Buffer *Ring_Buffer)
{
	return (Ring_Buffer->Write_Index - Ring_Buffer->Read_Index) & Ring_Buffer->Mask;//���ݳ����������Ҫ�����Ǿ������ݻ��εĹؼ�
}

/*
 * ��������uart_init
 * ����  ������3��ʼ������
 * ����  ��bound��������
 * ���  ����
 */ 
void uart_init(u32 bound){
																				//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��

																				//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 									//PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;								//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);										//��ʼ��GPIOA.9

																				//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;									//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;						//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);										//��ʼ��GPIOA.10  

																				//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;					//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;							//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);												//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
																				//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;									//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;					//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;						//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;							//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); 									//��ʼ������1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);								//�������ڽ����ж�
	USART_Cmd(USART1, ENABLE);                    								//ʹ�ܴ���1 
	
	Usart_TX_Buffer.Write_Index = 0;
	Usart_TX_Buffer.Read_Index = 0;
	Usart_TX_Buffer.Mask = TX_BUFFER_SIZE - 1;
	Usart_TX_Buffer.P_buffer = &TX_buffer[0];

	Usart_RX_Buffer.Write_Index = 0;
	Usart_RX_Buffer.Read_Index = 0;
	Usart_RX_Buffer.Mask = RX_BUFFER_SIZE - 1;
	Usart_RX_Buffer.P_buffer = &RX_buffer[0];
	
	printf("Usart 1 init success...\r\n");

}

volatile uint8_t Usart_Temp_Data;

void USART1_IRQHandler(void)                											//����1�жϷ������
{
#if SYSTEM_SUPPORT_OS 																	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{   
		USART_SendData(USART1, Usart_Buffer_Read_Data(&Usart_TX_Buffer)); 				//�������ݻ��淢��
		if(Usart_Buffer_Cnt(&Usart_TX_Buffer)==0)  
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);								//���绺����ˣ��͹رմ��ڷ����ж�
	}
	else if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);									//��������жϱ�־
																						//���ֻ��λ������鴮�ڽ��շ�ʽ�������ڽ���������ݣ��ܷ��㡣�����ݵ�Ҫ����:
																						//���ͷ�����Ҫ�������ݰ�ͷ���Ա������������޵�ַ������
		Usart_Temp_Data = (uint8_t) USART_ReceiveData(USART1);          				//��ʱ���ݸ�ֵ
		Usart_Buffer_Write_Data(&Usart_RX_Buffer,Usart_Temp_Data);               		//д���ڽ��ջ�������
		if(Usart_Temp_Data == 0x05)
		{
			LED1_ON;
			LED2_ON;
			LED3_ON;
			LED4_ON;
		}
#if BLE_APP
		Remote_control_APP(Usart_Temp_Data);
#endif
		
#if USB_PC
		Monitor_control_PC(Usart_Temp_Data);
#endif
	}
#if SYSTEM_SUPPORT_OS 																	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif	

