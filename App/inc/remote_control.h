#ifndef __REMOTE_CONTROL_H
#define __REMOTE_CONTROL_H

#include "stm32f10x.h"

#define MAX_REC_LEN 32

#define REMOTE_CONTROL_CMD_FLY_LOCK				5
#define REMOTE_CONTROL_CMD_FLY_UNLOCK			6
#define REMOTE_CONTROL_CMD_FLY_FOUR_DATA		7
#define REMOTE_CONTROL_CMD_FLY_LAND 			9
#define REMOTE_CONTROL_CMD_FLY_HEAD_FREE 		12
#define REMOTE_CONTROL_CMD_FLY_UNHEAD_FREE		13
#define REMOTE_CONTROL_CMD_FLY_STATE			16
#define REMOTE_CONTROL_CMD_FLY_CALI				205

void Remote_control_APP(uint8_t Data_In);
void Remote_control_Cmd_Process(void);
void Remote_Control_PWM_Convert(void);
u8 Remote_Control_Is_Connected(void);
u8 Remote_Control_Is_Calibrate(void);

u8 Is_WFLY_Lock(void);
u8 Is_WFLY_Unlock(void);


#endif
