#include "led.h"
#include "remote_control.h"
#include "battery_voltage.h"
#include "motor.h"
#include "imu.h"
#include "math.h"

void Aid_Control_Led_Alarm(void)
{

	LED_Control.event = Event_Ready;

	if(1)
		LED_Control.event = Event_Calibration;
	if(1)
		LED_Control.event = Event_Lost_Remote;
	if(1)
		LED_Control.event = Event_Calibration_Fail;
	if(Battery.Battery_Alarm)
		LED_Control.event = Event_Low_Voltage;
	if(imu_calibrate_flag)
		LED_Control.event = Event_Calibration;
	if(1)
		LED_Control.event = Event_Auto_Land;
	if(Battery.Battery_Charge_State)
		LED_Control.event = Event_Batter_Charge;
}

void Aid_Control_Crash(void)
{

	if(fabs(Angle.X)>80 || fabs(Angle.Y)>80 )
	{
		Motor_PWM_Flash(0,0,0,0);
		//FLY_ENABLE=0;
	}
}


