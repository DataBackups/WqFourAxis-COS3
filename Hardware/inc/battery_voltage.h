#ifndef __BATTERY_VOLTAGE_H
#define __BATTERY_VOLTAGE_H

#include "stm32f10x.h"
#define BATTERY_VOLTAGE_FILTER_LENGTH 5			//��ص�ѹADC����ֵ�����˲�����

void Battery_Voltage_ADC_Init(void);            //��ص�ѹADC����ʼ������
u16  Battery_Voltage_ADC_ReadValue(u8 ch);      //��ص�ѹADCԭʼ���ݶ�ȡ
float Battery_Voltage_ReadValue(void);          //�˲���ĵ�ص�ѹ����������ʱ����Ϊ16us

#endif
