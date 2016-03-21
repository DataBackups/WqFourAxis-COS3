#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "battery_voltage.h"

int main(void)
{	 
	float temp;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	Battery_Voltage_ADC_Init();     
	while(1)
	{
		temp = Battery_Voltage_ReadValue();
		printf("��ѹֵ= %0.2f\n",temp);
		delay_ms(500);
	}
}
