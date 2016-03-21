#include "battery_voltage.h"

u16 battery_voltege_adc_value;                                                   	//��ص�ѹADC����ֵ
u16 battery_voltege_adc_average;												   	//��ص�ѹADC����ƽ��ֵ
float battery_voltege_value;														//��ص�ѹ
u16 battery_voltege_adc_value_temp[BATTERY_VOLTAGE_FILTER_LENGTH] = {0};			//�洢��ص�ѹADC��ֵ����

/*
 * ��������Battery_Voltage_ADC_Init
 * ����  ����ص�ѹADC����ʼ������
 * ����  ����
 * ���  ����
 */
void Battery_Voltage_ADC_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1, ENABLE );	    //ʹ��GPIOA,ADC1ͨ��ʱ��

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);                                               //��Ƶ����6ʱ��Ϊ72M/6=12MHz
																					//SYSCLK/DIV2=12M ADCʱ������Ϊ12M,ADC���ʱ�Ӳ��ܳ���14M!
																					//���򽫵���ADC׼ȷ���½�!
	                        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;										//PB0 ��Ϊģ��ͨ���������� 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;									//ģ����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
    ADC_DeInit(ADC1);  																//������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ������λ

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;								//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//ģ��ת�������ڵ�ͨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;								//ģ��ת�������ڵ���ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;				//ת��������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;											//˳����й���ת����ADCͨ������Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);												//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���

    ADC_TempSensorVrefintCmd(ENABLE); 												//�����ڲ��¶ȴ�����

    ADC_Cmd(ADC1, ENABLE);															//ʹ��ָ����ADC1

    ADC_ResetCalibration(ADC1);														//����ָ����ADC1�ĸ�λ�Ĵ���
    while(ADC_GetResetCalibrationStatus(ADC1));										//��ȡADC1����У׼�Ĵ�����״̬,����״̬��ȴ�

    ADC_StartCalibration(ADC1);	 													//����ADУ׼
    while(ADC_GetCalibrationStatus(ADC1));											//��ȡָ��ADC1��У׼����,����״̬��ȴ�
}

/*
 * ��������Battery_Voltage_ADC_ReadValue
 * ����  ����ص�ѹADCԭʼ���ݶ�ȡ
 * ����  ��ch ADCͨ��
 * ���  ����
 */
u16  Battery_Voltage_ADC_ReadValue(u8 ch)
{
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	           //ADC1,ADCͨ��8��16,��һ��ת��,����ʱ��Ϊ239.5���ڣ���߲���ʱ�������߾�ȷ��    
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		                                   //ʹ��ָ����ADC1�����ת���������ܣ��������ת����ʹ�����ⲿ����(SWSTART)    
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));                                //�ȴ�ת������
	return ADC_GetConversionValue(ADC1);	                                       //�������һ��ADC1�������ת�����
}

/*
 * ��������Battery_Voltage_ReadValue
 * ����  ���˲���ĵ�ص�ѹ����������ʱ����Ϊ16us
 * ����  ����
 * ���  ����ص�ѹֵ
 */
float Battery_Voltage_ReadValue(void)
{
	static u8 filter_cnt	=	0;
	u8 cnt = 0;
	u32 temp = 0;
	
	battery_voltege_adc_value = Battery_Voltage_ADC_ReadValue(ADC_Channel_8);         //��ȡ��ص�ѹADCֵ
	battery_voltege_adc_value_temp[filter_cnt] = battery_voltege_adc_value;
	
	filter_cnt++;
	
	for(cnt	= 0;cnt < BATTERY_VOLTAGE_FILTER_LENGTH; cnt++)
	{
		temp += battery_voltege_adc_value_temp[cnt];
	}
	battery_voltege_adc_average = temp / BATTERY_VOLTAGE_FILTER_LENGTH;
	
	battery_voltege_value = ((float)battery_voltege_adc_average * 6.6f) / 4096.0f;    //ʵ�ʵ�ص�ѹֵ����
	if(filter_cnt	==	BATTERY_VOLTAGE_FILTER_LENGTH)	filter_cnt = 0;
	return battery_voltege_value;
}
