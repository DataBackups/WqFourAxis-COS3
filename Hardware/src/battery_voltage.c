#include "battery_voltage.h"

u16 battery_voltege_adc_value;                                                   	//电池电压ADC采样值
u16 battery_voltege_adc_average;												   	//电池电压ADC采样平均值
float battery_voltege_value;														//电池电压
u16 battery_voltege_adc_value_temp[BATTERY_VOLTAGE_FILTER_LENGTH] = {0};			//存储电池电压ADC数值队列
Battery_Voltage Battery;															//实例化一个电压信息结构体
/*
 * 函数名：Battery_Voltage_ADC_Init
 * 描述  ：电池电压ADC检测初始化函数
 * 输入  ：无
 * 输出  ：无
 */
void Battery_Voltage_ADC_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1, ENABLE );	    //使能GPIOA,ADC1通道时钟

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);                                               //分频因子6时钟为72M/6=12MHz
																					//SYSCLK/DIV2=12M ADC时钟设置为12M,ADC最大时钟不能超过14M!
																					//否则将导致ADC准确度下降!
	                        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;										//PB0 作为模拟通道输入引脚 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;									//模拟输入引脚
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
    ADC_DeInit(ADC1);  																//将外设 ADC1 的全部寄存器重设为缺省值，即复位

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;								//ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//模数转换工作在单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;								//模数转换工作在单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;				//转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;											//顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC1, &ADC_InitStructure);												//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

    ADC_TempSensorVrefintCmd(ENABLE); 												//开启内部温度传感器

    ADC_Cmd(ADC1, ENABLE);															//使能指定的ADC1

    ADC_ResetCalibration(ADC1);														//重置指定的ADC1的复位寄存器
    while(ADC_GetResetCalibrationStatus(ADC1));										//获取ADC1重置校准寄存器的状态,设置状态则等待

    ADC_StartCalibration(ADC1);	 													//开启AD校准
    while(ADC_GetCalibrationStatus(ADC1));											//获取指定ADC1的校准程序,设置状态则等待
	
	Battery.Battery_Real_Voltage = 4.20;											//单位为v 电池实际电压     校准电压时修改
    Battery.ADC_Input_Voltage = 2.08;												//单位为v R3和R4连接处电压 校准电压时修改
    Battery.MCU_Real_Voltage   = 3.29;												//单位为v 单片机供电电压   校准电压时修改
    Battery.Battery_K   = Battery.Battery_Real_Voltage / Battery.ADC_Input_Voltage;	//计算电压计算系数
    Battery.Battery_ADC_Min = 2000;													//电压门限AD值
}

/*
 * 函数名：Battery_Voltage_ADC_ReadValue
 * 描述  ：电池电压ADC原始数据读取
 * 输入  ：ch ADC通道
 * 输出  ：无
 */
u16  Battery_Voltage_ADC_ReadValue(u8 ch)
{
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	           //ADC1,ADC通道8和16,第一个转换,采样时间为239.5周期，提高采样时间可以提高精确度    
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		                                   //使能指定的ADC1的软件转换启动功能，软件控制转换，使用用外部触发(SWSTART)    
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));                                //等待转换结束
	return ADC_GetConversionValue(ADC1);	                                       //返回最近一次ADC1规则组的转换结果
}

/*
 * 函数名：Battery_Voltage_ADC_Average
 * 描述  ：获取通道ch的转换值，取times次,然后平均
 * 输入  ：ch ADC通道 times读取次数
 * 输出  ：返回值:通道ch的times次转换结果平均值
 */
u16 Battery_Voltage_ADC_Average(u8 ch,u8 times)
{
    u32 temp_value=0;
    u8 i;
    for(i = 0; i < times; i++)
    {
        temp_value += Battery_Voltage_ADC_ReadValue(ch);
    }
    return temp_value / times;
}

/*
 * 函数名：Battery_Voltage_ReadValue
 * 描述  ：滤波后的电池电压，函数运行时间大概为16us
 * 输入  ：无
 * 输出  ：电池电压值
 */
float Battery_Voltage_ReadValue(void)
{
	static u8 filter_cnt	=	0;
	u8 cnt = 0;
	u32 temp = 0;
	
	battery_voltege_adc_value = Battery_Voltage_ADC_ReadValue(ADC_Channel_8);         //读取电池电压ADC值
	battery_voltege_adc_value_temp[filter_cnt] = battery_voltege_adc_value;
	
	filter_cnt++;
	
	for(cnt	= 0;cnt < BATTERY_VOLTAGE_FILTER_LENGTH; cnt++)
	{
		temp += battery_voltege_adc_value_temp[cnt];
	}
	battery_voltege_adc_average = temp / BATTERY_VOLTAGE_FILTER_LENGTH;
	
	battery_voltege_value = ((float)battery_voltege_adc_average * 6.6f) / 4096.0f;    //实际电池电压值计算
	if(filter_cnt	==	BATTERY_VOLTAGE_FILTER_LENGTH)	filter_cnt = 0;
	return battery_voltege_value;
}

//检测电池电压
void BatteryCheck(void)
{
    Battery.Battery_ADC  = Battery_Voltage_ADC_Average(ADC_Channel_8,BATTERY_VOLTAGE_FILTER_LENGTH);            				//电池电压检测
    Battery.Battery_Test_Value = Battery.Battery_K * (Battery.Battery_ADC/4096.0) * Battery.MCU_Real_Voltage;					//实际电压 值计算
    if(1)
    {
        if(Battery.Battery_ADC <= Battery.Battery_ADC_Min)
        {
            Battery.Battery_Alarm = 1;
        }
        else
            Battery.Battery_Alarm = 0;
    }
    else
    {
        if((Battery.Battery_Test_Value < BATTERY_VOLTAGE_ALARM_VAL)&&(Battery.Battery_Test_Value > BATTERY_VOLTAGE_CHARGE_VAL))	//低于3.7v 且大于充电检测电压 BATTERY_VOLTAGE_CHARGE_VAL
            Battery.Battery_Alarm=1;
        else
            Battery.Battery_Alarm=0;
    }

    if(Battery.Battery_Test_Value < BATTERY_VOLTAGE_CHARGE_VAL) 																//正在充电	
    {
        Battery.Battery_Charge_State = 1;
    }
    else
        Battery.Battery_Charge_State = 0;

}
