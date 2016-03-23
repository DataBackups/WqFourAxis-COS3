#include "sys.h"
#include "usart.h"	  
#include "led.h"
#include "remote_control.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
													//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
													//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
													//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);					//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   									//如果使能了接收

													//环形 数组结构体实例化两个变量
Usart_Buffer Usart_TX_Buffer;						//环形发送结构体
Usart_Buffer Usart_RX_Buffer;						//环形接收结构体

uint8_t RX_buffer[RX_BUFFER_SIZE];
uint8_t TX_buffer[TX_BUFFER_SIZE];

/*
 * 函数名：Usart_Buffer_Read_Data
 * 描述  ：读取环形数据中的一个字节
 * 输入  ：Ring_Buffer：输入环形发送结构体
 * 输出  ：要从串口发送的数据
 */
uint8_t Usart_Buffer_Read_Data(Usart_Buffer *Ring_Buffer)
{
	uint8_t temp;
	temp = Ring_Buffer->P_buffer[Ring_Buffer->Read_Index & Ring_Buffer->Mask];	//数据长度掩码很重要，这是决定数据环形的关键
	Ring_Buffer->Read_Index++;													//读取完成一次，读指针加1，为下一次 读取做 准备
	return temp;
}

/*
 * 函数名：Usart_Buffer_Write_Data
 * 描述  ：将一个字节写入一个环形结构体中
 * 输入  ：Ring_Buffer：输入环形接收结构体 ，Data_In：从串口接收的数据
 * 输出  ：无
 */
void Usart_Buffer_Write_Data(Usart_Buffer *Ring_Buffer,uint8_t Data_In)
{
	Ring_Buffer->P_buffer[Ring_Buffer->Write_Index & Ring_Buffer->Mask] = Data_In;	//数据长度掩码很重要，这是决定数据环形的关键
	Ring_Buffer->Write_Index++;														//写完一次，写指针加1，为下一次写入做准备
}

/*
 * 函数名：Usart_Buffer_Cnt
 * 描述  ：当写指针写完一圈，追上了读指针，那么证明数据写满了，此时应该增加缓冲区长度，或者缩短外围数据处理时间
 * 输入  ：Ring_Buffer：输入环形发送结构体
 * 输出  ：返回环形数据区的可用字节长度
 */
uint16_t Usart_Buffer_Cnt(Usart_Buffer *Ring_Buffer)
{
	return (Ring_Buffer->Write_Index - Ring_Buffer->Read_Index) & Ring_Buffer->Mask;//数据长度掩码很重要，这是决定数据环形的关键
}

/*
 * 函数名：uart_init
 * 描述  ：串口3初始化函数
 * 输入  ：bound：波特率
 * 输出  ：无
 */ 
void uart_init(u32 bound){
																				//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

																				//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 									//PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;								//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);										//初始化GPIOA.9

																				//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;									//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;						//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);										//初始化GPIOA.10  

																				//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;					//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;							//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);												//根据指定的参数初始化VIC寄存器
	
																				//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;									//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;					//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;						//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;							//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				//收发模式

	USART_Init(USART1, &USART_InitStructure); 									//初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);								//开启串口接受中断
	USART_Cmd(USART1, ENABLE);                    								//使能串口1 
	
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

void USART1_IRQHandler(void)                											//串口1中断服务程序
{
#if SYSTEM_SUPPORT_OS 																	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{   
		USART_SendData(USART1, Usart_Buffer_Read_Data(&Usart_TX_Buffer)); 				//环形数据缓存发送
		if(Usart_Buffer_Cnt(&Usart_TX_Buffer)==0)  
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);								//假如缓冲空了，就关闭串口发送中断
	}
	else if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);									//清除接收中断标志
																						//此种环形缓冲数组串口接收方式，适用于解包各种数据，很方便。对数据的要求是:
																						//发送方必须要求有数据包头，以便解决串口数据无地址的问题
		Usart_Temp_Data = (uint8_t) USART_ReceiveData(USART1);          				//临时数据赋值
		Usart_Buffer_Write_Data(&Usart_RX_Buffer,Usart_Temp_Data);               		//写串口接收缓冲数组
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
#if SYSTEM_SUPPORT_OS 																	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif	

