#ifndef __DATE_PC_H
#define __DATE_PC_H
	
#include "stm32f10x.h"

//取一个数据的地址强制转换成指向char型的，然后取前八位
#define BYTEL(dwTemp)       (*(char *)(&dwTemp))
#define BYTEH(dwTemp)       (*((char *)(&dwTemp) + 1))
	
#define OUT_DATA_LENGTH 24	//定义数据长度

void Data_Send_To_PC(void);

#endif



