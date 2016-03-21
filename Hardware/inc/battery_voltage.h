#ifndef __BATTERY_VOLTAGE_H
#define __BATTERY_VOLTAGE_H

#include "stm32f10x.h"
#define BATTERY_VOLTAGE_FILTER_LENGTH 5

void Battery_Voltage_ADC_Init(void);
u16  Battery_Voltage_ADC_ReadValue(u8 ch);
float Battery_Voltage_ReadValue(void);

#endif
