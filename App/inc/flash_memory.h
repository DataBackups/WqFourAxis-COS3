#ifndef __FLASH_MEMORY_H
#define __FLASH_MEMORY_H

#include "stm32f10x.h"

//定义好的Flash虚拟地址，为兼容以后版本，不建议改动
#define FLASH_MEMORY_INIT_STATUS 0
#define FLASH_MEMORY_MPU6500_GYRO_X_OFFSET 1
#define FLASH_MEMORY_MPU6500_GYRO_Y_OFFSET 2
#define FLASH_MEMORY_MPU6500_GYRO_Z_OFFSET 3
#define FLASH_MEMORY_MPU6500_ACC_X_OFFSET 4
#define FLASH_MEMORY_MPU6500_ACC_Y_OFFSET 5
#define FLASH_MEMORY_MPU6500_ACC_Z_OFFSET 6
#define FLASH_MEMORY_MPU9250_MAG_X_OFFSET 7
#define FLASH_MEMORY_MPU9250_MAG_Y_OFFSET 8
#define FLASH_MEMORY_MPU9250_MAG_Z_OFFSET 9
//用户自定义
#define FLASH_MEMORY_STATUS1 10
#define FLASH_MEMORY_STATUS2 11
#define FLASH_MEMORY_STATUS3 12
#define FLASH_MEMORY_STATUS4 13
#define FLASH_MEMORY_STATUS5 14
#define FLASH_MEMORY_STATUS6 15
#define FLASH_MEMORY_STATUS7 16
#define FLASH_MEMORY_STATUS8 17
#define FLASH_MEMORY_STATUS9 18
#define FLASH_MEMORY_STATUS10 19

extern void Flash_Memory_Init(void);
extern void Flash_Memory_MPU6500_GYRO_Offset_Write(void);
extern void Flash_Memory_MPU6500_ACC_Offset_Write(void);

#endif
