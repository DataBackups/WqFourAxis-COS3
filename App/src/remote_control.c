#include "remote_control.h"

volatile uint8_t Usart_Data_Temp[MAX_REC_LEN];
uint8_t Data_Temp_Cnt = 0;
static uint8_t Valid_Data_Len=0;
static uint8_t Check_Sum=0;
uint16_t Remote_Control_Data[4]={1500,1500,1500,1500};

//处理发送给飞控的MSP命令
void Remote_control_Cmd_Process(void)
{
	switch(Usart_Data_Temp[4])
	{
		case REMOTE_CONTROL_CMD_FLY_FOUR_DATA:
			Remote_Control_Data[0] = Usart_Data_Temp[6]<<8  | Usart_Data_Temp[5];  //THROTTLE
			Remote_Control_Data[1] = Usart_Data_Temp[8]<<8  | Usart_Data_Temp[7];  //YAW
			Remote_Control_Data[2] = Usart_Data_Temp[10]<<8 | Usart_Data_Temp[9];  //PITCH
			Remote_Control_Data[3] = Usart_Data_Temp[12]<<8 | Usart_Data_Temp[11]; //ROLL
			break;
		case REMOTE_CONTROL_CMD_FLY_LOCK://arm，上锁
			//armState=REQ_ARM;
			break;
		case REMOTE_CONTROL_CMD_FLY_UNLOCK://disarm，解锁
			//armState=REQ_DISARM;
			break;
		case REMOTE_CONTROL_CMD_FLY_STATE:	
			//flyLogApp=1;
			break;
		case REMOTE_CONTROL_CMD_FLY_CALI:
			//imuCaliFlag=1;
			break;
		case REMOTE_CONTROL_CMD_FLY_HEAD_FREE:
			//SetHeadFree(1);
			break;
		case REMOTE_CONTROL_CMD_FLY_UNHEAD_FREE:
			//SetHeadFree(0);
			break;
		case REMOTE_CONTROL_CMD_FLY_LAND:		//自动降落
			//altCtrlMode=LANDING;
			break;
	}
}

void Remote_control_APP(uint8_t Data_In)
{
	Usart_Data_Temp[Data_Temp_Cnt]= Data_In;
	if(Data_Temp_Cnt<3)
	{
		switch(Data_Temp_Cnt)
		{
			case 0:
				if(Usart_Data_Temp[Data_Temp_Cnt]=='$')
					Data_Temp_Cnt++;
				break;
			case 1:
				if(Usart_Data_Temp[Data_Temp_Cnt]=='M')
					Data_Temp_Cnt++;
				else
					Data_Temp_Cnt=0;
				break;
			case 2:
				if(Usart_Data_Temp[Data_Temp_Cnt]=='<')
					Data_Temp_Cnt++;
				else
					Data_Temp_Cnt=0;
			break;
		}
	}
	else															//valid data
	{
		if(Data_Temp_Cnt==3)										//len
		{
			Check_Sum=0;
			Valid_Data_Len = Usart_Data_Temp[Data_Temp_Cnt];
		}
		Data_Temp_Cnt++;
		if(Data_Temp_Cnt >= Valid_Data_Len + 6)						// rec over. process. tobe placed in 50Hz loop
		{															//checksum
			if(Usart_Data_Temp[Data_Temp_Cnt-1]==Check_Sum)
			{
				Remote_control_Cmd_Process();						//could be place to main
			}
			Data_Temp_Cnt=0; 
		}
		else
			Check_Sum ^= Usart_Data_Temp[Data_Temp_Cnt-1];
	}  
}
