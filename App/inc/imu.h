#ifndef __IMU_H
#define __IMU_H

#include "stm32f10x.h"

#define SAMPLE_HALF_T 0.001f     		//采样周期的一半，单位：s
#define FILTER_LENGTH 20				//滑动滤波窗口长度

#define GYRO_G 	0.0610351f				//角速度变成度/秒     此参数对应陀螺2000度每秒  Gyro_G=1/16.375=0.0610687
#define GYRO_GR	0.0010653f				//角速度变成弧度/秒	 此参数对应陀螺2000度每秒

//弧度转变角度的单位 
//#define RtA 		57.324841f	

//角度转变弧度的单位 
//#define AtR    	0.0174533f	

//这里MPU6050初始化时，加速度计初始化的量程是±4g，      于是1 / (65536 / (8 * g)) = 0.0011962890625f 
//#define Acc_G 	0.0011963f 

//这里MPU6050初始化时，陀螺仪初始化的量程是±2000度每秒，于是1 / (65536 / 4000) = 0.06103515625f 
//#define Gyro_G 	0.0610351f 

//上面计算的是度/秒，转换为弧度/秒则有 0.0610351f * 0.0174533f = 0.00106526391083f 
//#define Gyro_Gr	0.0010653f

#define IMU_KP 10.0f     				//比例
#define IMU_KI 0.008f 					//积分

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
