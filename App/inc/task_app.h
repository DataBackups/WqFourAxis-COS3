#ifndef __TASK_APP_H
#define __TASK_APP_H

#include "stm32f10x.h"
#include "includes.h"
								
#define START_TASK_PRIO			3			//�������ȼ�
#define START_STK_SIZE 			512			//�����ջ��С	
void start_task(void *p_arg);				//������

#define LED0_TASK_PRIO			4			//�������ȼ�
#define LED0_STK_SIZE 			128			//�����ջ��С	
void led0_task(void *p_arg);				//������

#define LED1_TASK_PRIO			5			//�������ȼ�
#define LED1_STK_SIZE 			128			//�����ջ��С	
void led1_task(void *p_arg);				//������

#define FLOAT_TASK_PRIO			6			//�������ȼ�
#define FLOAT_STK_SIZE			128			//�����ջ��С
void float_task(void *p_arg);				//������


#endif
