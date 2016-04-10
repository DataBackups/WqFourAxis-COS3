#ifndef __BATTERY_VOLTAGE_H
#define __BATTERY_VOLTAGE_H

#include "stm32f10x.h"

#define BATTERY_VOLTAGE_FILTER_LENGTH 	5			//��ص�ѹADC����ֵ�����˲�����
#define BATTERY_VOLTAGE_ALARM_VAL  		3.7	  		//�ӵ�
#define BATTERY_VOLTAGE_CHARGE_VAL    	1.0	  		//�ó��ĵ�ѹ

//��ѹ��Ϣ�ṹ��
typedef struct
{  
	int    Battery_ADC;             			//��ѹADֵ
	float  Battery_Test_Value;            		//��ѹʵ��ֵ
	float  Battery_Real_Voltage;               	//��ص�ʵ�ʵ�ѹ�������ñ��
	float  MCU_Real_Voltage;                 	//AD�ο�Դ��ѹ�������ǵ�Ƭ�������ѹ��һ����3.3V���ң�Ҫʵ��
	float  ADC_Input_Voltage;               	//AD���������ѹ--->R15��R17�����ĺ��̵�ѹ
	float  Battery_K;                 			//�����ѹֵϵ�������ڵ�ѹУ׼
	int    Battery_ADC_Min;          			//��ѹ����
	char   Battery_Alarm;				  		//����λ
	char   Battery_Charge_State;			  	//���״̬
}Battery_Voltage;

extern Battery_Voltage Battery;

void Battery_Voltage_ADC_Init(void);            //��ص�ѹADC����ʼ������
u16  Battery_Voltage_ADC_ReadValue(u8 ch);      //��ص�ѹADCԭʼ���ݶ�ȡ
float Battery_Voltage_ReadValue(void);          //�˲���ĵ�ص�ѹ����������ʱ����Ϊ16us
void Battery_Voltege_Check(void);

#endif
