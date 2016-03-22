#include "mpu6050.h"
#include "delay.h"
#include "iic.h"

u8 offset_flag = 0;						//У׼ģʽ��־λ��Ϊ0δ����У׼��Ϊ1����У׼
extern u8 calibrate_status;			//�Ƿ�ִ��У׼ת̬��־λ
u8 mpu6050_buffer[14];					//iic��ȡmpu6050���ŵ�����
float	mpu6500_tempreature = 0;
s16 mpu6500_tempreature_temp = 0;
s16 mpu6500_tempreature_Offset = 0;

S_INT16_XYZ	MPU6500_Acc_Offset	=	{0,0,0};
S_INT16_XYZ	MPU6500_Gyro_Offset	=	{0,0,0};	
S_INT16_XYZ MPU6500_Acc = {0,0,0};
S_INT16_XYZ MPU6500_Gyro = {0,0,0};

/*
 * ��������MPU6050_Init
 * ����  ����ʼ��MPU6050
 * ����  ����
 * ���  ������ֵ:0,�ɹ�������,�������
 */
u8 MPU6050_Init(void)
{
    u8 res;

    IIC_Init();//��ʼ��IIC����
    MPU6050_Write_Byte(MPU6050_PWR_MGMT1_REG,0X80);	           	//��λMPU6050
    delay_ms(100);
    MPU6050_Write_Byte(MPU6050_PWR_MGMT1_REG,0X00);	          	//����MPU6050
    MPU6050_Set_Gyro_Fsr(3);									//�����Ǵ�����,��2000dps
    MPU6050_Set_Accel_Fsr(2);									//���ٶȴ�����,��8g
    MPU6050_Set_Rate(100);										//���ò�����100Hz
    MPU6050_Write_Byte(MPU6050_INT_EN_REG,0X00);				//�ر������ж�
    //MPU6050_Write_Byte(MPU6050_USER_CTRL_REG,0X00);			//I2C��ģʽ�ر�
    //MPU6050_Write_Byte(MPU6050_FIFO_EN_REG,0X00);				//�ر�FIFO
    MPU6050_Write_Byte(MPU6050_INTBP_CFG_REG,0X02);				//��·��Ч
    res=MPU6050_Read_Byte(MPU6050_DEVICE_ID_REG);
    if(res==MPU6050_ADDRESS_AD0_LOW)//����ID��ȷ
    {
        MPU6050_Write_Byte(MPU6050_PWR_MGMT1_REG,0X01);			//����CLKSEL,PLL X��Ϊ�ο�
        MPU6050_Write_Byte(MPU6050_PWR_MGMT2_REG,0X00);			//���ٶ��������Ƕ�����
        MPU6050_Set_Rate(100);									//���ò�����Ϊ100Hz
    }
	else 
		return 1;
    return 0;
}

/*
 * ��������MPU6050_Set_Gyro_Fsr
 * ����  ������MPU6050�����Ǵ����������̷�Χ
 * ����  ��fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
 * ���  ������ֵ:0,���óɹ�������,����ʧ��
 */
u8 MPU6050_Set_Gyro_Fsr(u8 fsr)
{
    return MPU6050_Write_Byte(MPU6050_GYRO_CFG_REG,fsr<<3);		//���������������̷�Χ
}

/*
 * ��������MPU6050_Set_Accel_Fsr
 * ����  ������MPU6050���ٶȴ����������̷�Χ
 * ����  ��fsr:0,��2g;1,��4g;2,��8g;3,��16g
 * ���  ������ֵ:0,���óɹ�������,����ʧ��
 */
u8 MPU6050_Set_Accel_Fsr(u8 fsr)
{
    return MPU6050_Write_Byte(MPU6050_ACCEL_CFG_REG,fsr<<3);	//���ü��ٶȴ����������̷�Χ
}

/*
 * ��������MPU6050_Set_LPF
 * ����  ������MPU6050�����ֵ�ͨ�˲���
 * ����  ��lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
 * ���  ������ֵ:0,���óɹ�������,����ʧ��
 */
u8 MPU6050_Set_LPF(u16 lpf)
{
    u8 data=0;
    if(lpf>=188)data=1;
    else if(lpf>=98)data=2;
    else if(lpf>=42)data=3;
    else if(lpf>=20)data=4;
    else if(lpf>=10)data=5;
    else data=6;
    return MPU6050_Write_Byte(MPU6050_CFG_REG,data);			//�������ֵ�ͨ�˲���
}

/*
 * ��������MPU6050_Set_Rate
 * ����  ������MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
 * ����  ��rate:4~1000(Hz)
 * ���  ������ֵ:0,���óɹ�������,����ʧ��
 */
u8 MPU6050_Set_Rate(u16 rate)
{
    u8 data;
    if(rate>1000)rate=1000;
    if(rate<4)rate=4;
    data=1000/rate-1;
    data=MPU6050_Write_Byte(MPU6050_SAMPLE_RATE_REG,data);		//�������ֵ�ͨ�˲���
    return MPU6050_Set_LPF(rate/2);								//�Զ�����LPFΪ�����ʵ�һ��
}

/*
 * ��������MPU6050_Write_Len
 * ����  ��IIC����д
 * ����  ��addr:������ַ��reg:�Ĵ�����ַ��len:д�볤�ȣ�buf:������
 * ���  ������ֵ:0,����������,�������
 */
u8 MPU6050_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    u8 i;
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0);									//����������ַ+д����
    if(IIC_Wait_Ack())											//�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);											//д�Ĵ�����ַ
    IIC_Wait_Ack();												//�ȴ�Ӧ��
    for(i=0; i<len; i++)
    {
        IIC_Send_Byte(buf[i]);									//��������
        if(IIC_Wait_Ack())										//�ȴ�ACK
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_Stop();
    return 0;
}

/*
 * ��������MPU6050_Read_Len
 * ����  ��IIC������
 * ����  ��addr:������ַ��reg:�Ĵ�����ַ��len:Ҫ��ȡ�ĳ��ȣ�buf:��ȡ�������ݴ洢��
 * ���  ������ֵ:0,����������,�������
 */
u8 MPU6050_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0);									//����������ַ+д����
    if(IIC_Wait_Ack())											//�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);											//д�Ĵ�����ַ
    IIC_Wait_Ack();												//�ȴ�Ӧ��
    IIC_Start();
    IIC_Send_Byte((addr<<1)|1);									//����������ַ+������
    IIC_Wait_Ack();												//�ȴ�Ӧ��
    while(len)
    {
        if(len==1)*buf=IIC_Read_Byte(0);						//������,����nACK
        else *buf=IIC_Read_Byte(1);								//������,����ACK
        len--;
        buf++;
    }
    IIC_Stop();	//����һ��ֹͣ����
    return 0;
}

/*
 * ��������MPU6050_Write_Byte
 * ����  ��IICдһ���ֽ�
 * ����  ��reg:�Ĵ�����ַ��data:����
 * ���  ������ֵ:0,����������,�������
 */
u8 MPU6050_Write_Byte(u8 reg,u8 data)
{
    IIC_Start();
    IIC_Send_Byte((MPU6050_ADDRESS_AD0_LOW<<1)|0);				//����������ַ+д����
    if(IIC_Wait_Ack())											//�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);											//д�Ĵ�����ַ
    IIC_Wait_Ack();												//�ȴ�Ӧ��
    IIC_Send_Byte(data);										//��������
    if(IIC_Wait_Ack())											//�ȴ�ACK
    {
        IIC_Stop();
        return 1;
    }
    IIC_Stop();
    return 0;
}

/*
 * ��������MPU6050_Read_Byte
 * ����  ��IIC��һ���ֽ�
 * ����  ��reg:�Ĵ�����ַ
 * ���  ������ֵ:����������
 */
u8 MPU6050_Read_Byte(u8 reg)
{
    u8 res;
    IIC_Start();
    IIC_Send_Byte((MPU6050_ADDRESS_AD0_LOW<<1)|0);				//����������ַ+д����
    IIC_Wait_Ack();												//�ȴ�Ӧ��
    IIC_Send_Byte(reg);											//д�Ĵ�����ַ
    IIC_Wait_Ack();												//�ȴ�Ӧ��
    IIC_Start();
    IIC_Send_Byte((MPU6050_ADDRESS_AD0_LOW<<1)|1);				//����������ַ+������
    IIC_Wait_Ack();												//�ȴ�Ӧ��
    res=IIC_Read_Byte(0);										//��ȡ����,����nACK
    IIC_Stop();													//����һ��ֹͣ����
    return res;
}

/*
 * ��������MPU6050_Get_Temperature
 * ����  ���õ��¶�ֵ
 * ����  ����
 * ���  ������ֵ:�¶�ֵ(������100��)
 */
short MPU6050_Get_Temperature(void)
{
    u8 buf[2];
    short raw;
    float temp;
    MPU6050_Read_Len(MPU6050_ADDRESS_AD0_LOW,MPU6050_TEMP_OUTH_REG,2,buf);
    raw=((u16)buf[0]<<8)|buf[1];
    temp=36.53+((double)raw)/340;
    return temp*100;
}

/*
 * ��������MPU6050_Get_Gyroscope
 * ����  ���õ�������ֵ(ԭʼֵ)
 * ����  ��gx,gy,gz:������x,y,z���ԭʼ����(������)
 * ���  ������ֵ:0,�ɹ�������,�������
 */
u8 MPU6050_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res;
    res=MPU6050_Read_Len(MPU6050_ADDRESS_AD0_LOW,MPU6050_GYRO_XOUTH_REG,6,buf);
    if(res==0)
    {
        *gx=((u16)buf[0]<<8)|buf[1];
        *gy=((u16)buf[2]<<8)|buf[3];
        *gz=((u16)buf[4]<<8)|buf[5];
    }
    return res;
}

/*
 * ��������MPU6050_Get_Accelerometer
 * ����  ���õ����ٶ�ֵ(ԭʼֵ)
 * ����  ��gx,gy,gz:������x,y,z���ԭʼ����(������)
 * ���  ������ֵ:0,�ɹ�������,�������
 */
u8 MPU6050_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;
    res=MPU6050_Read_Len(MPU6050_ADDRESS_AD0_LOW,MPU6050_ACCEL_XOUTH_REG,6,buf);
    if(res==0)
    {
        *ax=((u16)buf[0]<<8)|buf[1];
        *ay=((u16)buf[2]<<8)|buf[3];
        *az=((u16)buf[4]<<8)|buf[5];
    }
    return res;
}

/*
 * ��������MPU6050_Date_Offset
 * ����  ��MPU6050����У׼
 * ����  ��У׼����
 * ���  ����
 */ 
void MPU6050_Date_Offset(u16 cnt)
{
	static S_INT32_XYZ Temp_Gyro , Temp_Acc;
	int i = 0;
	
	Temp_Gyro.X = 0;
	Temp_Gyro.Y = 0;
	Temp_Gyro.Z = 0;
	
	Temp_Acc.X  = 0;
	Temp_Acc.Y  = 0;
	Temp_Acc.Z  = 0;
	
	offset_flag = 1;											//����MPU6050У׼ģʽ
	for(i = cnt; i > 0; i--)
	{
		MPU6050_Read_Value();
		
		Temp_Acc.X	+=	MPU6500_Acc.X;
		Temp_Acc.Y	+=	MPU6500_Acc.Y;		
//		Temp_Acc.Z	+=	MPU6500_Acc.Z;
		Temp_Gyro.X	+=	MPU6500_Gyro.X;
		Temp_Gyro.Y	+=	MPU6500_Gyro.Y;
		Temp_Gyro.Z	+=	MPU6500_Gyro.Z;
	}
	
	if(calibrate_status)										//���ٶ����ݱ�����EEPROM�У���������ˢ�£����ִ���ֶ�У׼��������ˢ��
	{
		MPU6500_Acc_Offset.X 	=	Temp_Acc.X	/	cnt;
		MPU6500_Acc_Offset.Y 	=	Temp_Acc.Y	/	cnt;
		MPU6500_Acc_Offset.Z  =	Temp_Acc.Z	/	cnt;	
	}
	
	MPU6500_Gyro_Offset.X	= Temp_Gyro.X	/	cnt;
	MPU6500_Gyro_Offset.Y	= Temp_Gyro.Y	/	cnt;
	MPU6500_Gyro_Offset.Z =	Temp_Gyro.Z	/	cnt;

	offset_flag = 0;											//�˳�MPU6050У׼ģʽ
}

/*
 * ��������MPU6050_Read_Value
 * ����  ����ȡMPU6050ԭʼ����
 * ����  ����
 * ���  ����
 */ 
void MPU6050_Read_Value(void)
{
	u8 res;
	res=MPU6050_Read_Len(MPU6050_ADDRESS_AD0_LOW,MPU6050_ACCEL_XOUTH_REG,14,mpu6050_buffer);
	if(res == 0)
	{
		if(offset_flag == 0)
		{
			MPU6500_Acc.X  = Byte16(s16, mpu6050_buffer[0],  mpu6050_buffer[1]) - MPU6500_Acc_Offset.X;
			MPU6500_Acc.Y  = Byte16(s16, mpu6050_buffer[2],  mpu6050_buffer[3]) - MPU6500_Acc_Offset.Y;
			MPU6500_Acc.Z  = Byte16(s16, mpu6050_buffer[4],  mpu6050_buffer[5]);
			MPU6500_Gyro.X = Byte16(s16, mpu6050_buffer[8],  mpu6050_buffer[9]) - MPU6500_Gyro_Offset.X;
			MPU6500_Gyro.Y = Byte16(s16, mpu6050_buffer[10],  mpu6050_buffer[11]) - MPU6500_Gyro_Offset.Y;
			MPU6500_Gyro.Z = Byte16(s16, mpu6050_buffer[12],  mpu6050_buffer[13]) - MPU6500_Gyro_Offset.Z;

//			mpu6500_tempreature_temp	=	Byte16(s16, mpu6050_buffer[6],  mpu6050_buffer[7]);
//			mpu6500_tempreature	=	(float)(35000+((521+mpu6500_tempreature_temp)*100)/34); // ԭ����ĸΪ340�����ڷ���*100����������1000����
//			mpu6500_tempreature = mpu6500_tempreature/1000;                             
//			if(( -4	<	MPU6500_Gyro.X ) && (MPU6500_Gyro.X < 4) ) MPU6500_Gyro.X = 0;
//			if(( -4	<	MPU6500_Gyro.Y ) && (MPU6500_Gyro.Y < 4) ) MPU6500_Gyro.Y = 0;
			if(( -4	< MPU6500_Gyro.Z) && (MPU6500_Gyro.Z < 4)) 
				MPU6500_Gyro.Z = 0;
		}
		else if(offset_flag)  									//MPU6500����У׼ģʽ
		{
			MPU6500_Acc.X  = Byte16(s16, mpu6050_buffer[0],  mpu6050_buffer[1]);
			MPU6500_Acc.Y  = Byte16(s16, mpu6050_buffer[2],  mpu6050_buffer[3]);
			MPU6500_Acc.Z  = Byte16(s16, mpu6050_buffer[4],  mpu6050_buffer[5]);
			MPU6500_Gyro.X = Byte16(s16, mpu6050_buffer[8],  mpu6050_buffer[9]);
			MPU6500_Gyro.Y = Byte16(s16, mpu6050_buffer[10],  mpu6050_buffer[11]);
			MPU6500_Gyro.Z = Byte16(s16, mpu6050_buffer[12],  mpu6050_buffer[13]);
		}
	}
}
