#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

#define Motor_PWM_Max 999

void Motor_Init(u16 arr, u16 psc);
void Motor_PWM_Flash(s16 MOTO1_PWM,s16 MOTO2_PWM,s16 MOTO3_PWM,s16 MOTO4_PWM);

#endif
