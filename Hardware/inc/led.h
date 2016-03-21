#ifndef __LED_H
#define __LED_H

#include "sys.h"
#include "stm32f10x.h"

#define LED1_ON    PAout(11) = 1
#define LED1_OFF   PAout(11) = 0
#define LED2_ON    PAout(8) = 1
#define LED2_OFF   PAout(8) = 0
#define LED3_ON    PBout(1) = 1
#define LED3_OFF   PBout(1) = 0
#define LED4_ON    PBout(3) = 1
#define LED4_OFF   PBout(3) = 0

#define LED1	0x01
#define LED2    0x02
#define LED3    0x04
#define LED4    0x08

#define Event_Ready 	           0        //׼������� M1��M2��˸��M3��M4Ϩ��
#define Event_Calibration		   1	    //У׼������ M1��M2����3�룬M3��M4Ϩ��
#define Event_Low_Voltage	       2		//��ص͵�ѹ M1��M2��M3��M4��˸
#define Event_Calibration_Fail	   3		//У׼ʧ��   M1��M2��M3��M4������˸
#define Event_Lost_Remote 	       4		//��ʧң���� M1��M2��M3��M4�������˸
#define Event_Auto_Land            5        //�Զ�����   M1��M2��M3��M4����
#define Event_Batter_Charge        6		//��س��   M1��M2��M3��M4Ϩ��

typedef union {
    u8 byte;
    struct
    {
        u8 LED_1	:1;		//λ��LED_1ռbyte�����λ
        u8 LED_2	:1;		
        u8 LED_3	:1;
        u8 LED_4	:1;
        u8 reserved	:4;
    } bits;
} LED_Buffer;

typedef struct LED_Event
{
    u8 event;
    u16 cnt;
} LED_Event_E;

extern LED_Event_E LED_Control;

void LED_Init(void);
void LED_Flash(void);
	
#endif
