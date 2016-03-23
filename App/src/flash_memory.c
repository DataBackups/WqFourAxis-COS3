#include "flash_memory.h"
#include "eeprom.h"
#include "mpu6050.h"

static void Flash_Memory_MPU6500_GYRO_Offset_Read(void);
static void Flash_Memory_MPU6500_ACC_Offset_Read(void);

/*
 * ��������Flash_Memory_Init
 * ����  ��ϵͳ������ʼ��
 * ����  ����
 * ���  ����
 */  
void Flash_Memory_Init(void)
{
	//PID_Init();												//PID������ʼ��
	//Flash_Memory_MPU6500_GYRO_Offset_Read();					//������ԭʼ�����ܻ���Ӱ���ÿ������У׼
	Flash_Memory_MPU6500_ACC_Offset_Read();
}


/*
 * ��������Flash_Memory_MPU6500_GYRO_Offset_Write
 * ����  ��д��������ԭʼ������ģ��EEPROM��
 * ����  ����
 * ���  ����
 */  
void Flash_Memory_MPU6500_GYRO_Offset_Write(void)
{	
	EE_WriteVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_GYRO_X_OFFSET],MPU6500_Gyro_Offset.X);
	EE_WriteVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_GYRO_Y_OFFSET],MPU6500_Gyro_Offset.Y);
	EE_WriteVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_GYRO_Z_OFFSET],MPU6500_Gyro_Offset.Z);
}

/*
 * ��������Flash_Memory_MPU6500_ACC_Offset_Write
 * ����  ��д����ٶȼ�ԭʼ������ģ��EEPROM��
 * ����  ����
 * ���  ����
 */
void Flash_Memory_MPU6500_ACC_Offset_Write(void)
{
	EE_WriteVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_ACC_X_OFFSET],MPU6500_Acc_Offset.X);
	EE_WriteVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_ACC_Y_OFFSET],MPU6500_Acc_Offset.Y);
	EE_WriteVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_ACC_Z_OFFSET],MPU6500_Acc_Offset.Z);
}

/*
 * ��������Flash_Memory_MPU6500_GYRO_Offset_Read
 * ����  ����ģ��EEPROM�ж�ȡ������ԭʼ����
 * ����  ����
 * ���  ����
 */
void Flash_Memory_MPU6500_GYRO_Offset_Read(void)
{
	s16 temp[3];
	
	EE_ReadVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_GYRO_X_OFFSET], &temp[0]);
	EE_ReadVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_GYRO_Y_OFFSET], &temp[1]);
	EE_ReadVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_GYRO_Z_OFFSET], &temp[2]);
	
	MPU6500_Gyro_Offset.X = temp[0];
	MPU6500_Gyro_Offset.Y = temp[1];
	MPU6500_Gyro_Offset.Z = temp[2];
}

/*
 * ��������Flash_Memory_MPU6500_ACC_Offset_Read
 * ����  ����ģ��EEPROM�ж�ȡ���ٶȼ�ԭʼ����
 * ����  ����
 * ���  ����
 */
void Flash_Memory_MPU6500_ACC_Offset_Read(void)
{
	s16 temp[3];
	
	EE_ReadVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_ACC_X_OFFSET], &temp[0]);
	EE_ReadVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_ACC_Y_OFFSET], &temp[1]);
	EE_ReadVariable(VirtAddVarTab[FLASH_MEMORY_MPU6500_ACC_Z_OFFSET], &temp[2]);
	
	MPU6500_Acc_Offset.X = temp[0];
	MPU6500_Acc_Offset.Y = temp[1];
	MPU6500_Acc_Offset.Z = temp[2];
}

