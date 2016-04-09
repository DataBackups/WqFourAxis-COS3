#ifndef __CONTROL_H
#define __CONTROL_H

#include "stm32f10x.h"
#include "mpu6050.h"
#include "motor.h"
#include "imu.h"

#define LAUNCH_THROTTLE		600 			//∆∑…”Õ√≈
#define ADD_THROTTLE		1000 			//≤π≥‰”Õ√≈

typedef struct PID
{
	float P;
	float Pout;
	float I;
	float Iout;
	float D;
	float Dout;
	float I_Max;
	float Out;
}PID;

void Control(void);
void PID_Init(void);


#endif
