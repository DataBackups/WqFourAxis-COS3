#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

extern volatile s16 system_time;//µ¥Î»£ºS
extern volatile u16 cnt;
extern volatile u16 ms;
extern volatile u16 ms5611_ms;
extern volatile u16 ms5611_status;

void TIM3_Int_Init(u16 arr, u16 psc);

#endif
