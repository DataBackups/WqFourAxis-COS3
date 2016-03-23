#include "iic.h"
#include "delay.h"

/*
 * ��������IIC_Delay
 * ����  ��IIC��ʱ����
 * ����  ����
 * ���  ����
 */
void IIC_Delay(void)
{
    delay_us(2);
}

/*
 * ��������IIC_Init
 * ����  ����ʼ��IIC
 * ����  ����
 * ���  ����
 */
void IIC_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);		//��ʹ������IO PORTBʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	 	// �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 	//�����趨������ʼ��GPIO
}


/*
 * ��������IIC_Start
 * ����  ������IIC��ʼ�ź�
 * ����  ����
 * ���  ����
 */
void IIC_Start(void)
{
    SDA_OUT();     												//sda�����
    IIC_SDA=1;
    IIC_SCL=1;
    IIC_Delay();
    IIC_SDA=0;													//START:when CLK is high,DATA change form high to low
    IIC_Delay();
    IIC_SCL=0;													//ǯסI2C���ߣ�׼�����ͻ��������
}

/*
 * ��������IIC_Stop
 * ����  ������IICֹͣ�ź�
 * ����  ����
 * ���  ����
 */
void IIC_Stop(void)
{
    SDA_OUT();													//sda�����
    IIC_SCL=0;
    IIC_SDA=0;													//STOP:when CLK is high DATA change form low to high
    IIC_Delay();
    IIC_SCL=1;
    IIC_SDA=1;													//����I2C���߽����ź�
    IIC_Delay();
}

/*
 * ��������IIC_Wait_Ack
 * ����  ���ȴ�Ӧ���źŵ���
 * ����  ����
 * ���  ������1������Ӧ��ʧ�ܣ�����0������Ӧ��ɹ�
 */
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SDA_IN();      												//SDA����Ϊ����
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
    IIC_SCL=0;													//ʱ�����0
    return 0;
}

/*
 * ��������IIC_Ack
 * ����  ������ACKӦ��
 * ����  ����
 * ���  ����
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
 * ��������IIC_NAck
 * ����  ��������ACKӦ��
 * ����  ����
 * ���  ����
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
 * ��������IIC_Send_Byte
 * ����  �����شӻ�����Ӧ��
 * ����  ����
 * ���  ������1����Ӧ�𣻷���0����Ӧ��
 */
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL=0;													//����ʱ�ӿ�ʼ���ݴ���
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
 * ��������IIC_Read_Byte
 * ����  ����1���ֽ�
 * ����  ��ack=1ʱ������ACK��ack=0������nACK
 * ���  �����ؽ��յ�������receive
 */
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK
    return receive;
}