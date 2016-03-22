#include "mpu6050.h"
#include "delay.h"
#include "iic.h"

u8 offset_flag = 0;						//校准模式标志位，为0未进行校准，为1进行校准
extern u8 calibrate_status;			//是否执行校准转态标志位
u8 mpu6050_buffer[14];					//iic读取mpu6050后存放的数据
float	mpu6500_tempreature = 0;
s16 mpu6500_tempreature_temp = 0;
s16 mpu6500_tempreature_Offset = 0;

S_INT16_XYZ	MPU6500_Acc_Offset	=	{0,0,0};
S_INT16_XYZ	MPU6500_Gyro_Offset	=	{0,0,0};	
S_INT16_XYZ MPU6500_Acc = {0,0,0};
S_INT16_XYZ MPU6500_Gyro = {0,0,0};

/*
 * 函数名：MPU6050_Init
 * 描述  ：初始化MPU6050
 * 输入  ：无
 * 输出  ：返回值:0,成功；其他,错误代码
 */
u8 MPU6050_Init(void)
{
    u8 res;

    IIC_Init();//初始化IIC总线
    MPU6050_Write_Byte(MPU6050_PWR_MGMT1_REG,0X80);	           	//复位MPU6050
    delay_ms(100);
    MPU6050_Write_Byte(MPU6050_PWR_MGMT1_REG,0X00);	          	//唤醒MPU6050
    MPU6050_Set_Gyro_Fsr(3);									//陀螺仪传感器,±2000dps
    MPU6050_Set_Accel_Fsr(2);									//加速度传感器,±8g
    MPU6050_Set_Rate(100);										//设置采样率100Hz
    MPU6050_Write_Byte(MPU6050_INT_EN_REG,0X00);				//关闭所有中断
    //MPU6050_Write_Byte(MPU6050_USER_CTRL_REG,0X00);			//I2C主模式关闭
    //MPU6050_Write_Byte(MPU6050_FIFO_EN_REG,0X00);				//关闭FIFO
    MPU6050_Write_Byte(MPU6050_INTBP_CFG_REG,0X02);				//旁路有效
    res=MPU6050_Read_Byte(MPU6050_DEVICE_ID_REG);
    if(res==MPU6050_ADDRESS_AD0_LOW)//器件ID正确
    {
        MPU6050_Write_Byte(MPU6050_PWR_MGMT1_REG,0X01);			//设置CLKSEL,PLL X轴为参考
        MPU6050_Write_Byte(MPU6050_PWR_MGMT2_REG,0X00);			//加速度与陀螺仪都工作
        MPU6050_Set_Rate(100);									//设置采样率为100Hz
    }
	else 
		return 1;
    return 0;
}

/*
 * 函数名：MPU6050_Set_Gyro_Fsr
 * 描述  ：设置MPU6050陀螺仪传感器满量程范围
 * 输入  ：fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
 * 输出  ：返回值:0,设置成功；其他,设置失败
 */
u8 MPU6050_Set_Gyro_Fsr(u8 fsr)
{
    return MPU6050_Write_Byte(MPU6050_GYRO_CFG_REG,fsr<<3);		//设置陀螺仪满量程范围
}

/*
 * 函数名：MPU6050_Set_Accel_Fsr
 * 描述  ：设置MPU6050加速度传感器满量程范围
 * 输入  ：fsr:0,±2g;1,±4g;2,±8g;3,±16g
 * 输出  ：返回值:0,设置成功；其他,设置失败
 */
u8 MPU6050_Set_Accel_Fsr(u8 fsr)
{
    return MPU6050_Write_Byte(MPU6050_ACCEL_CFG_REG,fsr<<3);	//设置加速度传感器满量程范围
}

/*
 * 函数名：MPU6050_Set_LPF
 * 描述  ：设置MPU6050的数字低通滤波器
 * 输入  ：lpf:数字低通滤波频率(Hz)
 * 输出  ：返回值:0,设置成功；其他,设置失败
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
    return MPU6050_Write_Byte(MPU6050_CFG_REG,data);			//设置数字低通滤波器
}

/*
 * 函数名：MPU6050_Set_Rate
 * 描述  ：设置MPU6050的采样率(假定Fs=1KHz)
 * 输入  ：rate:4~1000(Hz)
 * 输出  ：返回值:0,设置成功；其他,设置失败
 */
u8 MPU6050_Set_Rate(u16 rate)
{
    u8 data;
    if(rate>1000)rate=1000;
    if(rate<4)rate=4;
    data=1000/rate-1;
    data=MPU6050_Write_Byte(MPU6050_SAMPLE_RATE_REG,data);		//设置数字低通滤波器
    return MPU6050_Set_LPF(rate/2);								//自动设置LPF为采样率的一半
}

/*
 * 函数名：MPU6050_Write_Len
 * 描述  ：IIC连续写
 * 输入  ：addr:器件地址；reg:寄存器地址；len:写入长度；buf:数据区
 * 输出  ：返回值:0,正常；其他,错误代码
 */
u8 MPU6050_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    u8 i;
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0);									//发送器件地址+写命令
    if(IIC_Wait_Ack())											//等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);											//写寄存器地址
    IIC_Wait_Ack();												//等待应答
    for(i=0; i<len; i++)
    {
        IIC_Send_Byte(buf[i]);									//发送数据
        if(IIC_Wait_Ack())										//等待ACK
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_Stop();
    return 0;
}

/*
 * 函数名：MPU6050_Read_Len
 * 描述  ：IIC连续读
 * 输入  ：addr:器件地址；reg:寄存器地址；len:要读取的长度；buf:读取到的数据存储区
 * 输出  ：返回值:0,正常；其他,错误代码
 */
u8 MPU6050_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    IIC_Start();
    IIC_Send_Byte((addr<<1)|0);									//发送器件地址+写命令
    if(IIC_Wait_Ack())											//等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);											//写寄存器地址
    IIC_Wait_Ack();												//等待应答
    IIC_Start();
    IIC_Send_Byte((addr<<1)|1);									//发送器件地址+读命令
    IIC_Wait_Ack();												//等待应答
    while(len)
    {
        if(len==1)*buf=IIC_Read_Byte(0);						//读数据,发送nACK
        else *buf=IIC_Read_Byte(1);								//读数据,发送ACK
        len--;
        buf++;
    }
    IIC_Stop();	//产生一个停止条件
    return 0;
}

/*
 * 函数名：MPU6050_Write_Byte
 * 描述  ：IIC写一个字节
 * 输入  ：reg:寄存器地址；data:数据
 * 输出  ：返回值:0,正常；其他,错误代码
 */
u8 MPU6050_Write_Byte(u8 reg,u8 data)
{
    IIC_Start();
    IIC_Send_Byte((MPU6050_ADDRESS_AD0_LOW<<1)|0);				//发送器件地址+写命令
    if(IIC_Wait_Ack())											//等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);											//写寄存器地址
    IIC_Wait_Ack();												//等待应答
    IIC_Send_Byte(data);										//发送数据
    if(IIC_Wait_Ack())											//等待ACK
    {
        IIC_Stop();
        return 1;
    }
    IIC_Stop();
    return 0;
}

/*
 * 函数名：MPU6050_Read_Byte
 * 描述  ：IIC读一个字节
 * 输入  ：reg:寄存器地址
 * 输出  ：返回值:读到的数据
 */
u8 MPU6050_Read_Byte(u8 reg)
{
    u8 res;
    IIC_Start();
    IIC_Send_Byte((MPU6050_ADDRESS_AD0_LOW<<1)|0);				//发送器件地址+写命令
    IIC_Wait_Ack();												//等待应答
    IIC_Send_Byte(reg);											//写寄存器地址
    IIC_Wait_Ack();												//等待应答
    IIC_Start();
    IIC_Send_Byte((MPU6050_ADDRESS_AD0_LOW<<1)|1);				//发送器件地址+读命令
    IIC_Wait_Ack();												//等待应答
    res=IIC_Read_Byte(0);										//读取数据,发送nACK
    IIC_Stop();													//产生一个停止条件
    return res;
}

/*
 * 函数名：MPU6050_Get_Temperature
 * 描述  ：得到温度值
 * 输入  ：无
 * 输出  ：返回值:温度值(扩大了100倍)
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
 * 函数名：MPU6050_Get_Gyroscope
 * 描述  ：得到陀螺仪值(原始值)
 * 输入  ：gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
 * 输出  ：返回值:0,成功；其他,错误代码
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
 * 函数名：MPU6050_Get_Accelerometer
 * 描述  ：得到加速度值(原始值)
 * 输入  ：gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
 * 输出  ：返回值:0,成功；其他,错误代码
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
 * 函数名：MPU6050_Date_Offset
 * 描述  ：MPU6050数据校准
 * 输入  ：校准次数
 * 输出  ：无
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
	
	offset_flag = 1;											//进入MPU6050校准模式
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
	
	if(calibrate_status)										//加速度数据保存在EEPROM中，无需重新刷新，如果执行手动校准，则重新刷新
	{
		MPU6500_Acc_Offset.X 	=	Temp_Acc.X	/	cnt;
		MPU6500_Acc_Offset.Y 	=	Temp_Acc.Y	/	cnt;
		MPU6500_Acc_Offset.Z  =	Temp_Acc.Z	/	cnt;	
	}
	
	MPU6500_Gyro_Offset.X	= Temp_Gyro.X	/	cnt;
	MPU6500_Gyro_Offset.Y	= Temp_Gyro.Y	/	cnt;
	MPU6500_Gyro_Offset.Z =	Temp_Gyro.Z	/	cnt;

	offset_flag = 0;											//退出MPU6050校准模式
}

/*
 * 函数名：MPU6050_Read_Value
 * 描述  ：读取MPU6050原始数据
 * 输入  ：无
 * 输出  ：无
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
//			mpu6500_tempreature	=	(float)(35000+((521+mpu6500_tempreature_temp)*100)/34); // 原来分母为340，现在分子*100，即：扩大1000倍；
//			mpu6500_tempreature = mpu6500_tempreature/1000;                             
//			if(( -4	<	MPU6500_Gyro.X ) && (MPU6500_Gyro.X < 4) ) MPU6500_Gyro.X = 0;
//			if(( -4	<	MPU6500_Gyro.Y ) && (MPU6500_Gyro.Y < 4) ) MPU6500_Gyro.Y = 0;
			if(( -4	< MPU6500_Gyro.Z) && (MPU6500_Gyro.Z < 4)) 
				MPU6500_Gyro.Z = 0;
		}
		else if(offset_flag)  									//MPU6500处于校准模式
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
