#ifndef __TASK_APP_H
#define __TASK_APP_H

#include "stm32f10x.h"
#include "includes.h"
								
#define START_TASK_PRIO			10			//任务优先级
#define START_STK_SIZE 			256			//任务堆栈大小	
void start_task(void *p_arg);				//任务函数

#define IMU_TASK_PRIO			9			//任务优先级
#define IMU_STK_SIZE 			128			//任务堆栈大小	
void imu_task(void *p_arg);					//任务函数

#define CONTROL_TASK_PRIO		8			//任务优先级
#define CONTROL_STK_SIZE 		256			//任务堆栈大小	
void control_task(void *p_arg);				//任务函数

#define PRESSURE_TASK_PRIO		7			//任务优先级
#define PRESSURE_STK_SIZE		128			//任务堆栈大小
void pressure_task(void *p_arg);			//任务函数

#define SAFETY_TASK_PRIO		6			//任务优先级
#define SAFETY_STK_SIZE			128			//任务堆栈大小
void safety_task(void *p_arg);				//任务函数

#endif
