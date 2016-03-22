#ifndef __IMU_H
#define __IMU_H

#include "stm32f10x.h"

#define SAMPLE_HALF_T 0.001f     		//�������ڵ�һ�룬��λ��s
#define FILTER_LENGTH 20				//�����˲����ڳ���

#define GYRO_G 	0.0610351f				//���ٶȱ�ɶ�/��     �˲�����Ӧ����2000��ÿ��  Gyro_G=1/16.375=0.0610687
#define GYRO_GR	0.0010653f				//���ٶȱ�ɻ���/��	 �˲�����Ӧ����2000��ÿ��

//����ת��Ƕȵĵ�λ 
//#define RtA 		57.324841f	

//�Ƕ�ת�仡�ȵĵ�λ 
//#define AtR    	0.0174533f	

//����MPU6050��ʼ��ʱ�����ٶȼƳ�ʼ���������ǡ�4g��      ����1 / (65536 / (8 * g)) = 0.0011962890625f 
//#define Acc_G 	0.0011963f 

//����MPU6050��ʼ��ʱ�������ǳ�ʼ���������ǡ�2000��ÿ�룬����1 / (65536 / 4000) = 0.06103515625f 
//#define Gyro_G 	0.0610351f 

//���������Ƕ�/�룬ת��Ϊ����/������ 0.0610351f * 0.0174533f = 0.00106526391083f 
//#define Gyro_Gr	0.0010653f

#define IMU_KP 10.0f     				//����
#define IMU_KI 0.008f 					//����

typedef struct
{
	float X;
	float Y;
	float Z;
}S_FLOAT_XYZ;

extern volatile S_FLOAT_XYZ Angle;

void IMU_Prepare_Data(void);
void IMU_Update(void);
void IMU_Date_Init(void);

#endif
