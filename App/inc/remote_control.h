#ifndef __REMOTE_CONTROL_H
#define __REMOTE_CONTROL_H

#include "stm32f10x.h"

enum 
{
	DISARMED = 0,
	REQ_ARM,
	ARMED,
	REQ_DISARM
};

#define MAX_REC_LEN 32

#define REMOTE_CONTROL_CMD_FLY_FOUR_DATA		7
#define REMOTE_CONTROL_CMD_FLY_LOCK				5
#define REMOTE_CONTROL_CMD_FLY_UNLOCK			6
#define REMOTE_CONTROL_CMD_FLY_LAUNCH			8
#define REMOTE_CONTROL_CMD_FLY_LAND 			9
#define REMOTE_CONTROL_CMD_FLY_HOLD_HIGHT		10
#define REMOTE_CONTROL_CMD_FLY_STOP_HIGHT		11
#define REMOTE_CONTROL_CMD_FLY_HEAD_FREE 		12
#define REMOTE_CONTROL_CMD_FLY_UNHEAD_FREE		13
#define REMOTE_CONTROL_CMD_FLY_CALI				205
#define REMOTE_CONTROL_CMD_FLY_STATE			16

void Remote_control_APP(uint8_t Data_In);
void Remote_control_Cmd_Process(void);
void Remote_Control_PWM_Convert(void);
u8 Remote_Control_Is_Connected(void);
u8 Remote_Control_Is_Calibrate(void);

extern u8 imu_calibrate_flag;
extern u8 hold_height;
extern u8 lock_unlock_flag;
extern u8 lost_remote_flag;
extern u8 not_calibrate;
extern u8 fly_enable;
extern u8 fly_enable_flag;

#endif
