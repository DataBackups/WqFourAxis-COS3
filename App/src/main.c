#include "task_app.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "battery_voltage.h"
#include "led.h"
#include "motor.h"
#include "mpu6050.h"
#include "imu.h"
#include "timer.h"
#include "eeprom.h"
#include "flash_memory.h"
#include "ms5611.h"
#include "remote_control.h"
#include "control.h"
#include "data_pc.h"
#include "aid_control.h"

//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];

void APP_System_Init(void);

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	APP_System_Init();
		
	OSInit(&err);									//初始化UCOSIII
	OS_CRITICAL_ENTER();							//进入临界区
													//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();								//退出临界区	 
	OSStart(&err);  								//开启UCOSIII
	while(1);
}

void APP_System_Init(void)
{
	FLASH_Unlock();									//解锁flash,是否安全更多考虑
	EE_Init();										//初始化模拟eeprom设置
	Flash_Memory_Init();							//参数初始化
	delay_init();	    	 						//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组配置,设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
#if 1
	uart_init(115200);	 							//串口初始化为115200  蓝牙控制
#else
	uart_init(460800);								//串口初始化为460800  PC调试
#endif
	Battery_Voltage_ADC_Init();  
	LED_Init();
	Motor_Init(3999,35);      						//PWM输出初始化，电机PWM频率500Hz
	while(MPU6050_Init());							
	TIM3_Int_Init(1000,72);							//1ms中断一次	
	MS5611_Init();
}
