#include "led.h"
#include "remote_control.h"

void Aid_Control_Led_Alarm(void)
{

	LED_Control.event = Event_Ready;

	if(1)
		LED_Control.event = Event_Calibration;
	if(1)
		LED_Control.event = Event_Lost_Remote;
	if(1)
		LED_Control.event = Event_Calibration_Fail;
	if(1)
		LED_Control.event = Event_Low_Voltage;
	if(imu_calibrate_flag)
		LED_Control.event = Event_Calibration;
	if(1)
		LED_Control.event = Event_Auto_Land;
	if(1)
		LED_Control.event = Event_Batter_Charge;
}




