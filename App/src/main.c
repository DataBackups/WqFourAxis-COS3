#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "battery_voltage.h"
#include "led.h"
#include "motor.h"

int main(void)
{	 
	float temp;
	delay_init();	    	 						//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 							//串口初始化为115200
	Battery_Voltage_ADC_Init();  
	LED_Init();
	Motor_Init(999,2);      						//PWM输出初始化，电机PWM频率24000Hz
	//Motor_PWM_Flash(100,100,100,100);
	LED_Control.event = Event_Batter_Charge;
	while(1)
	{
		temp = Battery_Voltage_ReadValue();
		printf("电压值= %0.2f\n",temp);
		LED_Flash();
		delay_ms(500);
	}
}
