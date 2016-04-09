#include "ms5611.h"
#include "iic.h"
#include "delay.h"
#include "math.h"

s32 ref_pressure;//参考气压
s32 ref_altitude;//参考海拔


MS5611_DATA MS5611_Date = {0};

/**************************实现函数********************************************
*函数原型:		void MS5611_Reset(void)
*功　　能:	    发送复位命令到 MS561101B 
*******************************************************************************/
void MS5611_Reset(void) 
{
	IIC_Start();
    IIC_Send_Byte(MS5611_ADDR); //写地址
	IIC_Wait_Ack();
	IIC_Send_Byte(MS5611_RESET);//发送复位命令
	IIC_Wait_Ack();	
    IIC_Stop();
}

void MS5611_Init(void)
{
	MS5611_Reset(); // 复位 MS561101B 
	delay_ms(100); // 延时 
	MS5611_PROM_READ();// 读取EEPROM 中的标定值 待用	
	MS5611_Altitude_Offset(50);
	
	MS5611_Altitude_Calculate();
}

u16 MS5611_Read_Write_2Bytes(unsigned char addr)
{
	unsigned char byteH,byteL;
	unsigned int return_value;
	
	IIC_Start();
	IIC_Send_Byte(MS5611_ADDR);
	IIC_Wait_Ack();
	IIC_Send_Byte(addr);
	IIC_Wait_Ack();	
	IIC_Stop();
	delay_us(5);
	IIC_Start();
	IIC_Send_Byte(MS5611_ADDR+1);  		//进入接收模式	
	delay_us(1);
	IIC_Wait_Ack();
	byteH = IIC_Read_Byte(1);  			//带ACK的读数据
	delay_us(1);
	byteL = IIC_Read_Byte(0);	 		//最后一个字节NACK
	IIC_Stop();
	return_value = ((unsigned int)byteH<<8) | (byteL);
	
	return(return_value);
}

/**************************实现函数********************************************
*函数原型:		void MS561101BA_readPROM(void)
*功　　能:	    读取 MS561101B 的工厂标定值
读取 气压计的标定值  用于修正温度和气压的读数
*******************************************************************************/
void MS5611_PROM_READ(void)
{
  MS5611_Date.C1 = MS5611_Read_Write_2Bytes(MS5611_PROM_COEFF_1);
  MS5611_Date.C2 = MS5611_Read_Write_2Bytes(MS5611_PROM_COEFF_2);
  MS5611_Date.C3 = MS5611_Read_Write_2Bytes(MS5611_PROM_COEFF_3);
  MS5611_Date.C4 = MS5611_Read_Write_2Bytes(MS5611_PROM_COEFF_4);
  MS5611_Date.C5 = MS5611_Read_Write_2Bytes(MS5611_PROM_COEFF_5);
  MS5611_Date.C6 = MS5611_Read_Write_2Bytes(MS5611_PROM_COEFF_6); 
  //SPI2_Read_Write_2Bytes(MS5611_PROM_CRC); 
}



//MS5611二阶温度补偿
void MS5611_SecondOrder_Temp_Compensation(void)    
{
	double temp;

	MS5611_Date.OFF = (u32)MS5611_Date.C2 * 65536 + ((u32)MS5611_Date.C4 * MS5611_Date.dT) / 128;
	MS5611_Date.SENS = (u32)MS5611_Date.C1 * 32768 + ((u32)MS5611_Date.C3 * MS5611_Date.dT) / 256;

	if(MS5611_Date.TEMP < 2000)//如果MS5611温度小于20摄氏度
	{
		MS5611_Date.TEMP2 = (MS5611_Date.dT * MS5611_Date.dT) / 2147483648U;
		temp = (MS5611_Date.TEMP - 2000) * (MS5611_Date.TEMP - 2000);
		MS5611_Date.OFF2 = 2.5 * temp;//OFF2 = 5 * (TEMP – 2000)^2 / 2
		MS5611_Date.SENS2 = 1.25 * temp;//SENS2 = 5 * (TEMP – 2000)^2/ 2
		
		if(MS5611_Date.TEMP < -1500)//如果MS5611温度小于-15摄氏度
		{
			temp = (MS5611_Date.TEMP + 1500)^2;
			MS5611_Date.OFF2 += 7 * temp;
			MS5611_Date.SENS2 += 5.5 * temp;
		}
	}
	else//如果MS5611温度大于20摄氏度
	{
		MS5611_Date.TEMP2 = 0;
		MS5611_Date.OFF2 = 0;
		MS5611_Date.SENS2 = 0;
	}

	MS5611_Date.TEMP = MS5611_Date.TEMP - MS5611_Date.TEMP2;
	MS5611_Date.OFF -= MS5611_Date.OFF2;
	MS5611_Date.SENS -= MS5611_Date.SENS2;
}

void MS5611_Temp_Compensation(void)    
{
	MS5611_Date.OFF = (u32)MS5611_Date.C2 * 65536 + ((u32)MS5611_Date.C4 * MS5611_Date.dT) / 128;
	MS5611_Date.SENS = (u32)MS5611_Date.C1 * 32768 + ((u32)MS5611_Date.C3 * MS5611_Date.dT) / 256;
}

/*
 * 函数名：MS5611_Read_ADC
 * 描述  ：读取ADC结果
 * 输入  ：无
 * 输出  ：返回24位的气压/温度
 */
long MS5611_Read_ADC(void)
{
	unsigned char byteH,byteM,byteL;
	long return_value;
	IIC_Start();
	IIC_Send_Byte(MS5611_ADDR); //写地址
	IIC_Wait_Ack();
	IIC_Send_Byte(0);// start read sequence
	IIC_Wait_Ack();	
	IIC_Stop();
	
	IIC_Start();
	IIC_Send_Byte(MS5611_ADDR+1);  //进入接收模式	
	IIC_Wait_Ack();
	byteH = IIC_Read_Byte(1);  //带ACK的读数据  bit 23-16
	byteM = IIC_Read_Byte(1);  //带ACK的读数据  bit 8-15
	byteL = IIC_Read_Byte(0);  //带NACK的读数据 bit 0-7
	IIC_Stop();
	return_value = (((long)byteH) << 16) | (((long)byteM) << 8) | (byteL);
	
	return(return_value);
}

void MS5611_TemperatureADC_Conversion(void)
{
	IIC_Start();
	IIC_Send_Byte(MS5611_ADDR); 			//写地址
	IIC_Wait_Ack();
	IIC_Send_Byte(MS5611_D2_OSR_4096); 		//写转换命令
	IIC_Wait_Ack();	
	IIC_Stop();
}

void MS5611_Temperature_Calculate(void)
{

	MS5611_Date.D2 = MS5611_Read_ADC();
	MS5611_Date.dT = MS5611_Date.D2 - (((u32)MS5611_Date.C5) << 8);
	MS5611_Date.TEMP = 2000 + MS5611_Date.dT * MS5611_Date.C6 / 8388608;
	//MS5611_Temp_Compensation();
	MS5611_SecondOrder_Temp_Compensation();
}

void MS5611_PressureADC_Conversion(void)
{
	IIC_Start();
	IIC_Send_Byte(MS5611_ADDR); 			//写地址
	IIC_Wait_Ack();
	IIC_Send_Byte(MS5611_D1_OSR_4096); 		//写转换命令
	IIC_Wait_Ack();	
	IIC_Stop();
}

void MS5611_Pressure_Calculate(void)
{
	MS5611_Date.D1 = MS5611_Read_ADC();//获得气压值
	MS5611_Date.P = (MS5611_Date.D1 * MS5611_Date.SENS / 2097152 - MS5611_Date.OFF) / 32768;
}

void MS5611_Altitude_Offset(u16 cnt)
{
	static double temp;
	u16 i = 0;

	for(i = cnt; i > 0; i--)
	{
		MS5611_TemperatureADC_Conversion();
		delay_ms(12); // 延时
		MS5611_Temperature_Calculate();
		MS5611_PressureADC_Conversion();
		delay_ms(12); // 延时
		MS5611_Pressure_Calculate();
		
		temp += MS5611_Date.P;
	}	
		
	ref_pressure = temp / cnt;
	ref_altitude = 44330 * (1 - pow((ref_pressure) / 101325.0, 0.190295));
}


float altitude = 0; 
vs32 height;//单位:cm
vs32 height_avg;
vs32 Height_Buf[MS5611_FILTER_LENGTH] = {0};

float MS5611_Altitude_Calculate(void)                             
{      
	static u8 filter_cnt	=	0;
	u8 cnt;
	s32	temp;

	altitude = 44330 * (1 - pow((MS5611_Date.P) / 101325.0, 0.190295));
	height = (altitude - ref_altitude) * 100;     
	//height = 10 * (ref_pressure - MS5611_Date.P);//单位厘米	

	temp = 0;	
	Height_Buf[filter_cnt] = height;
	filter_cnt++;

	for(cnt	=	0;cnt <	MS5611_FILTER_LENGTH;cnt++)
	{
		temp += Height_Buf[cnt];
	}
	height_avg = temp / MS5611_FILTER_LENGTH;

	if(filter_cnt	==	MS5611_FILTER_LENGTH)	filter_cnt = 0;
	height = height_avg;

	return (altitude);
}
