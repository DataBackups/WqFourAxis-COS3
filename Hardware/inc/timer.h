#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

extern volatile s16 system_time;//��λ��S
extern volatile u16 cnt;
extern volatile u16 ms;
extern volatile u16 ms5611_ms;
extern volatile u16 ms5611_status;
extern u8 loop_500Hz_flag,loop_100Hz_flag,loop_50Hz_flag,loop_10Hz_flag;

void TIM3_Int_Init(u16 arr, u16 psc);

#endif
