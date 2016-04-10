#ifndef __BATTERY_VOLTAGE_H
#define __BATTERY_VOLTAGE_H

#include "stm32f10x.h"

#define BATTERY_VOLTAGE_FILTER_LENGTH 	5			//电池电压ADC采样值滑动滤波次数
#define BATTERY_VOLTAGE_ALARM_VAL  		3.7	  		//接地
#define BATTERY_VOLTAGE_CHARGE_VAL    	1.0	  		//该充电的电压

//电压信息结构体
typedef struct
{  
	int    Battery_ADC;             			//电压AD值
	float  Battery_Test_Value;            		//电压实际值
	float  Battery_Real_Voltage;               	//电池的实际电压，用万用表测
	float  MCU_Real_Voltage;                 	//AD参考源电压，这里是单片机供电电压，一般在3.3V左右，要实测
	float  ADC_Input_Voltage;               	//AD采样输入电压--->R15和R17相连的焊盘电压
	float  Battery_K;                 			//计算电压值系数，用于电压校准
	int    Battery_ADC_Min;          			//电压门限
	char   Battery_Alarm;				  		//报警位
	char   Battery_Charge_State;			  	//充电状态
}Battery_Voltage;

extern Battery_Voltage Battery;

void Battery_Voltage_ADC_Init(void);            //电池电压ADC检测初始化函数
u16  Battery_Voltage_ADC_ReadValue(u8 ch);      //电池电压ADC原始数据读取
float Battery_Voltage_ReadValue(void);          //滤波后的电池电压，函数运行时间大概为16us
void Battery_Voltege_Check(void);

#endif
