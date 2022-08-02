


#include "includes.h"


/*
	��ȡ��ص�ѹ
	
	PC0
*/


static short g_int_temp = 0;  //�¶�ֵ��������100��

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 @brief ADC������ʼ��
 @param ��
 @return ��
*/
void ADC_Init(void)
{
    /*------------------ʱ������------------------*/
    // GPIOʱ��ʹ��
//    rcu_periph_clock_enable(RCU_GPIOC);
    // ADCʱ��ʹ��
    rcu_periph_clock_enable(RCU_ADC0);
    // ADCʱ��8��Ƶ�����14MHz
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV12);

    /*------------------ADC GPIO����------------------*/
    /* ADC SCAN function enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE,ENABLE);  
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_2_EXTTRIG_INSERTED_NONE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);  
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 1);

    /* ADC temperature sensor channel config */
    adc_inserted_channel_config(ADC0, 0, ADC_CHANNEL_16, ADC_SAMPLETIME_71POINT5);//ADC_SAMPLETIME_239POINT5);
    /* ADC internal reference voltage channel config */
    //adc_inserted_channel_config(ADC0, 1, ADC_CHANNEL_17, ADC_SAMPLETIME_239POINT5);

    /* ADC external trigger enable */
    adc_external_trigger_config(ADC0, ADC_INSERTED_CHANNEL,ENABLE);

    /* ADC temperature and Vrefint enable */
    adc_tempsensor_vrefint_enable();
    
    /* enable ADC interface */
    adc_enable(ADC0);
    Delay1ms(1);    
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
	
	adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);  /* ADC software trigger enable */
}

/**
 @brief ADC��ȡ
 @param channel -[in] ADCͨ��
 @return ADC����ֵ
*/
uint16_t ADC_Read(uint8_t channel)
{
 //   uint16_t adcValue = 0;
    
    	
	return ADC_IDATA0(ADC0);
	
//    return adcValue;
}



//��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//ch:ͨ�����
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
static uint16_t getAdcAverage(uint8_t ch,uint8_t times)
{
//	uint32_t temp_val=0;
//	uint8_t t;
//	for(t=0;t<times;t++)
//	{
//		temp_val+=ADC_Read(ch);
//		Delay1ms(5);
//		//delay_1ms(5);
//	}
//	return temp_val/times;
} 
	 


/*
	��ص�ѹ������4.2v�����ǽ�����1/2��ѹ�������ٿ�����
	��ѹֵ�ļ��� 3.3*val /1024 �͵õ���������ĵ�ѹ
*/
//uint16_t ADCgetBatVol(void)
//{	
//	return getAdcAverage(ADC_CHANNEL_0,3)* 66 /1024;   //��Ϊ2��ѹ�ˣ����Գ�3.3*2,����10�����Ͳ���ʹ��С��
//}


/*********************************************************************************************************
** Function name:           Get_Temprate
** Descriptions:            ��ȡSTM32�ڲ��¶ȴ�������ֵ
** input parameters:        none
** Returned value:          �¶�ֵ(������100��,��λ:��.)
*********************************************************************************************************/
static short Get_Temprate(void)    //��ȡ�ڲ��¶ȴ������¶�ֵ
{
    uint32_t adcx;
    short result;
    float temperature;
    adcx = ADC_Read(ADC_CHANNEL_16);//(ADC_CHANNEL_16,5);  //��ȡͨ��16,5��ȡƽ��
//    MY_PRINTF("adcx = %d\r\n",adcx);
	temperature = (1.43 - adcx*3.3/4096) * 1000 / 4.3 + 10;     //25 --> 10
    result = temperature*=100;                  //����100��.
    return result;
}



short get_internal_temp(void)
{
	return g_int_temp;
}



//��ص�ѹ�������,1000ms����һ��
void Int_Temp_task(void)
{
//	uint16_t temp;   //�¶�ֵ
	/* ADC software trigger enable */
    //adc_software_trigger_enable(ADC1, ADC_INSERTED_CHANNEL);
	
	g_int_temp = Get_Temprate();   //����¶�ֵ(������100��,��λ:��.)
	
	adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);  /* ADC software trigger enable */
	MY_PRINTF("g_int_temp = %d\r\n",g_int_temp);
	
}



