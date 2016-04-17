#ifndef __TASK_APP_H
#define __TASK_APP_H

#include "stm32f10x.h"
#include "includes.h"
//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		512

//任务函数
void start_task(void *p_arg);

//任务优先级
#define LED0_TASK_PRIO		4
//任务堆栈大小	
#define LED0_STK_SIZE 		128

void led0_task(void *p_arg);

//任务优先级
#define LED1_TASK_PRIO		5
//任务堆栈大小	
#define LED1_STK_SIZE 		128

//任务函数
void led1_task(void *p_arg);

//任务优先级
#define FLOAT_TASK_PRIO		6
//任务堆栈大小
#define FLOAT_STK_SIZE		128

//任务函数
void float_task(void *p_arg);

#endif
