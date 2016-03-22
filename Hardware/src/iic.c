#include "iic.h"
#include "delay.h"

/*
 * 函数名：IIC_Delay
 * 描述  ：IIC延时函数
 * 输入  ：无
 * 输出  ：无
 */
void IIC_Delay(void)
{
    delay_us(2);
}

/*
 * 函数名：IIC_Init
 * 描述  ：初始化IIC
 * 输入  ：无
 * 输出  ：无
 */
void IIC_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);		//先使能外设IO PORTB时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	 	// 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 	//根据设定参数初始化GPIO
}


/*
 * 函数名：IIC_Start
 * 描述  ：产生IIC起始信号
 * 输入  ：无
 * 输出  ：无
 */
void IIC_Start(void)
{
    SDA_OUT();     												//sda线输出
    IIC_SDA=1;
    IIC_SCL=1;
    IIC_Delay();
    IIC_SDA=0;													//START:when CLK is high,DATA change form high to low
    IIC_Delay();
    IIC_SCL=0;													//钳住I2C总线，准备发送或接收数据
}

/*
 * 函数名：IIC_Stop
 * 描述  ：产生IIC停止信号
 * 输入  ：无
 * 输出  ：无
 */
void IIC_Stop(void)
{
    SDA_OUT();													//sda线输出
    IIC_SCL=0;
    IIC_SDA=0;													//STOP:when CLK is high DATA change form low to high
    IIC_Delay();
    IIC_SCL=1;
    IIC_SDA=1;													//发送I2C总线结束信号
    IIC_Delay();
}

/*
 * 函数名：IIC_Wait_Ack
 * 描述  ：等待应答信号到来
 * 输入  ：无
 * 输出  ：返回1，接收应答失败；返回0，接收应答成功
 */
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SDA_IN();      												//SDA设置为输入
    IIC_SDA=1;
    IIC_Delay();
    IIC_SCL=1;
    IIC_Delay();
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL=0;													//时钟输出0
    return 0;
}

/*
 * 函数名：IIC_Ack
 * 描述  ：产生ACK应答
 * 输入  ：无
 * 输出  ：无
 */
void IIC_Ack(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=0;
    IIC_Delay();
    IIC_SCL=1;
    IIC_Delay();
    IIC_SCL=0;
}

/*
 * 函数名：IIC_NAck
 * 描述  ：不产生ACK应答
 * 输入  ：无
 * 输出  ：无
 */
void IIC_NAck(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=1;
    IIC_Delay();
    IIC_SCL=1;
    IIC_Delay();
    IIC_SCL=0;
}

/*
 * 函数名：IIC_Send_Byte
 * 描述  ：返回从机有无应答
 * 输入  ：无
 * 输出  ：返回1，有应答；返回0，无应答
 */
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL=0;													//拉低时钟开始数据传输
    for(t=0; t<8; t++)
    {
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1;
        IIC_SCL=1;
        IIC_Delay();
        IIC_SCL=0;
        IIC_Delay();
    }
}

/*
 * 函数名：IIC_Read_Byte
 * 描述  ：读1个字节
 * 输入  ：ack=1时，发送ACK，ack=0，发送nACK
 * 输出  ：返回接收到的数据receive
 */
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA设置为输入
    for(i=0; i<8; i++ )
    {
        IIC_SCL=0;
        IIC_Delay();
        IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;
        IIC_Delay();
    }
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK
    return receive;
}
