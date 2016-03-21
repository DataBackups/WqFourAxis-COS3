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

#define Event_Ready 	           0        //准备好起飞 M1和M2闪烁，M3和M4熄灭
#define Event_Calibration		   1	    //校准陀螺仪 M1和M2长亮3秒，M3和M4熄灭
#define Event_Low_Voltage	       2		//电池低电压 M1、M2、M3、M4闪烁
#define Event_Calibration_Fail	   3		//校准失败   M1、M2和M3、M4交替闪烁
#define Event_Lost_Remote 	       4		//丢失遥控器 M1、M2、M3、M4跑马灯闪烁
#define Event_Auto_Land            5        //自动降落   M1、M2、M3、M4常亮
#define Event_Batter_Charge        6		//电池充电   M1、M2、M3、M4熄灭

typedef union {
    u8 byte;
    struct
    {
        u8 LED_1	:1;		//位域，LED_1占byte的最低位
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
