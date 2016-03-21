#ifndef __BATTERY_VOLTAGE_H
#define __BATTERY_VOLTAGE_H

#include "stm32f10x.h"
#define BATTERY_VOLTAGE_FILTER_LENGTH 5			//电池电压ADC采样值滑动滤波次数

void Battery_Voltage_ADC_Init(void);            //电池电压ADC检测初始化函数
u16  Battery_Voltage_ADC_ReadValue(u8 ch);      //电池电压ADC原始数据读取
float Battery_Voltage_ReadValue(void);          //滤波后的电池电压，函数运行时间大概为16us

#endif
