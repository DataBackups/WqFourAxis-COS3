#include "remote_control.h"
#include "imu.h"
#include "control.h"
#include "flash_memory.h"

u8 imu_calibrate_flag = 0;
u8 fly_state_up = 0;
u8 lock_unlock_flag = 0 ;				//解锁标志位，为0未解锁，为1解锁;上锁需要将标志置 0	
u8 calibrate_status = 0;			 	//是否执行校准转态标志位
extern vs16 throttle;
extern S_FLOAT_XYZ Exp_Angle;


volatile uint8_t Usart_Data_Temp[MAX_REC_LEN];
uint8_t Data_Temp_Cnt = 0;
static uint8_t Valid_Data_Len=0;
static uint8_t Check_Sum=0;

uint16_t Remote_Control_Data[4]={1500,1500,1500,1500};

//处理发送给飞控的MSP命令
void Remote_Control_Cmd_Process(void)
{
	switch(Usart_Data_Temp[4])
	{
		case REMOTE_CONTROL_CMD_FLY_FOUR_DATA:
			Remote_Control_Data[0] = Usart_Data_Temp[6]<<8  | Usart_Data_Temp[5];  //THROTTLE  油门
			Remote_Control_Data[1] = Usart_Data_Temp[8]<<8  | Usart_Data_Temp[7];  //YAW	   航向
			Remote_Control_Data[2] = Usart_Data_Temp[10]<<8 | Usart_Data_Temp[9];  //PITCH     俯仰
			Remote_Control_Data[3] = Usart_Data_Temp[12]<<8 | Usart_Data_Temp[11]; //ROLL      横滚
			break;
		case REMOTE_CONTROL_CMD_FLY_LOCK:			//解锁
			lock_unlock_flag = 1;
			break;
		case REMOTE_CONTROL_CMD_FLY_UNLOCK:			//上锁
			lock_unlock_flag = 0;
			break;
		case REMOTE_CONTROL_CMD_FLY_LAUNCH:			//起飞
			
			break;
		case REMOTE_CONTROL_CMD_FLY_LAND:			//降落
			
			break;
		case REMOTE_CONTROL_CMD_FLY_HEAD_FREE:  	//有头模式
			
			break;
		case REMOTE_CONTROL_CMD_FLY_UNHEAD_FREE:	//无头模式
			
			break;
		case REMOTE_CONTROL_CMD_FLY_HOLD_HIGHT:		//定高
			
			break;
		case REMOTE_CONTROL_CMD_FLY_STOP_HIGHT: 	//取消定高
			
			break;	
		case REMOTE_CONTROL_CMD_FLY_CALI:			//校准陀螺仪
			imu_calibrate_flag =1;
			break;
		case REMOTE_CONTROL_CMD_FLY_STATE:			//上传信息到手机
			fly_state_up = 1;
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
				Remote_Control_Cmd_Process();						//could be place to main
			}
			Data_Temp_Cnt=0; 
		}
		else
			Check_Sum ^= Usart_Data_Temp[Data_Temp_Cnt-1];
	}  
}

float angle_z;

/*
 * 函数名：Remote_Control_Is_Connected
 * 描述  ：判断遥控器是否正常连接
 * 输入  ：无
 * 输出  ：0：连接失败；1：正常连接
 */  
u8 Remote_Control_Is_Connected(void)
{
	if((Remote_Control_Data[0] > 1900) &&(Remote_Control_Data[1] < 1100) && (Remote_Control_Data[2] > 1900) && (Remote_Control_Data[3] > 1900))
		return 0;
	else 
		return 1;
}

/*
 * 函数名：Remote_Control_Is_Calibrate
 * 描述  ：校准陀螺仪
 * 输入  ：无
 * 输出  ：0：未解锁；1：解锁成功
 */  
u8 Remote_Control_Is_Calibrate(void)
{	
	if( !lock_unlock_flag )													//如果未解锁才执行校准检测
	{	
		SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;							//关闭中断
		calibrate_status = 1;

		IMU_Date_Init(); 										 	 	 	//每次解锁后都先初始化导航数据
		MPU6050_Date_Offset(5000);								 			//校准MPU6500
		Flash_Memory_MPU6500_GYRO_Offset_Write();						 	//写入陀螺仪补偿
		Flash_Memory_MPU6500_ACC_Offset_Write();						 	//写入加速度计补偿
		Flash_Memory_Init();												//参数初始化
		calibrate_status = 0;
		SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk; 							//开启中断
		return 1;
	}
	else 
		return 0;
}

/*
 * 函数名：Remote_Control_PWM_Convert
 * 描述  ：手机蓝牙遥控器信号转换
 * 输入  ：无
 * 输出  ：无
 */ 
void Remote_Control_PWM_Convert(void)
{
	static u16 cnt;
	
	cnt++;
	
	Exp_Angle.X = (Remote_Control_Data[3]- 1500) * 0.05f;
	if((Exp_Angle.X > -2) && (Exp_Angle.X < 2))	Exp_Angle.X = 0;
	Exp_Angle.Y = (Remote_Control_Data[2]- 1500) * 0.05f;
	if((Exp_Angle.Y > -2) && (Exp_Angle.Y < 2))	Exp_Angle.Y = 0;
	
	throttle	=	(vs16)(Remote_Control_Data[0] - 1000) * 3;
	if(cnt == 20)
	{
		cnt = 0;	
		if((Remote_Control_Data[1] > 1600) || (Remote_Control_Data[1] < 1400))//防止死区累计误差
		{
			if(throttle > LAUNCH_THROTTLE)	
				Exp_Angle.Z += -(Remote_Control_Data[1]- 1500) * 0.006f;
		}		
		
		if((Exp_Angle.Z > 180) && (Angle.Z > 0))
		{
			angle_z = Angle.Z - 360;
		}
		else if((Exp_Angle.Z < -180) && (Angle.Z < 0))
		{
			angle_z = Angle.Z + 360;
		}
		else 
			angle_z = Angle.Z;
				
		if(Exp_Angle.Z > 360)  Exp_Angle.Z = (float)((s32)Exp_Angle.Z % 360);
		if(Exp_Angle.Z < -360) Exp_Angle.Z = (float)((s32)Exp_Angle.Z % -360);		
	}
}
