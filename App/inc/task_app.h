#ifndef __TASK_APP_H
#define __TASK_APP_H

#include "stm32f10x.h"
#include "includes.h"
								
#define START_TASK_PRIO			10			//�������ȼ�
#define START_STK_SIZE 			256			//�����ջ��С	
void start_task(void *p_arg);				//������

#define IMU_TASK_PRIO			9			//�������ȼ�
#define IMU_STK_SIZE 			128			//�����ջ��С	
void imu_task(void *p_arg);					//������

#define CONTROL_TASK_PRIO		8			//�������ȼ�
#define CONTROL_STK_SIZE 		256			//�����ջ��С	
void control_task(void *p_arg);				//������

#define PRESSURE_TASK_PRIO		7			//�������ȼ�
#define PRESSURE_STK_SIZE		128			//�����ջ��С
void pressure_task(void *p_arg);			//������

#define SAFETY_TASK_PRIO		6			//�������ȼ�
#define SAFETY_STK_SIZE			128			//�����ջ��С
void safety_task(void *p_arg);				//������

#endif
