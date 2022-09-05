


#include "includes.h"


/*
	��ȡ��Ƭ���ڲ��¶�
*/


static short g_int_temp = 0;  //�¶�ֵ��������100��



#if 1

//ADC12_IN13
//static void pc3_adc_test_init(void)
//{
//	rcu_periph_clock_enable(RCU_GPIOC);
//	gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
//}



//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3	
void ADC_Init(void)  //ADCͨ����ʼ��
{
    // ADCʱ��ʹ��
    rcu_periph_clock_enable(RCU_ADC0);
    // ADCʱ��8��Ƶ�����14MHz
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);
    
  
    /*------------------ADC����ģʽ����------------------*/
    // ֻʹ��һ��ADC,���ڶ���ģʽ
    adc_mode_config(ADC_MODE_FREE);
    // ��ͨ����ɨ��ģʽ
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);
    // ��ͨ��������ת��ģʽ
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    
	adc_discontinuous_mode_config(ADC0, ADC_REGULAR_CHANNEL, 1);   //����ģʽ��1��ͨ��
	
    // ���ת���Ҷ���
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    // ת��ͨ��1��
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);
    
	//�ڲ��¶�ͨ��
	adc_tempsensor_vrefint_enable();  //�¶�ͨ������
	adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_16, ADC_SAMPLETIME_55POINT5);
//	pc3_adc_test_init();
	// ����ADCͨ��ת��˳�򣬲���ʱ��Ϊ55.5��ʱ������
//	adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_13, ADC_SAMPLETIME_55POINT5);
	
    // �����ⲿ����ת���������������
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
//    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
    
    // ʹ��ADC
    adc_enable(ADC0);
    Delay1ms(1);      // �ȴ�1ms
    // ʹ��ADCУ׼
    adc_calibration_enable(ADC0);
  
    // ����û�в����ⲿ����������ʹ���������ADCת��
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL); 	
}



uint16_t T_Get_Adc(uint8_t ch)   
{
    uint16_t adcValue = 0;
//    uint16_t  n = 0;
	
	

	
//    while(!adc_flag_get(ADC0, ADC_FLAG_EOC))
//	{
//		n++;
//		Delay1ms(1);
//		if(n>2000)
//		{	
//			printf("ERROR: adc time out\r\n");
//			return 0;
//		}			
//	}                       // �ȴ��������
    adc_flag_clear(ADC0, ADC_FLAG_EOC);                             // ���������־
    
    adcValue = adc_regular_data_read(ADC0);                         // ��ȡADC����

     // ����ADCͨ��ת��˳�򣬲���ʱ��Ϊ55.5��ʱ������
    adc_regular_channel_config(ADC0, 0, ch, ADC_SAMPLETIME_55POINT5);
    // ����û�в����ⲿ����������ʹ���������ADCת��
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);  

	return adcValue;

}

//�õ�ADC�����ڲ��¶ȴ�������ֵ
//ȡ10��,Ȼ��ƽ��
uint16_t T_Get_Temp(void)
{
	uint16_t temp_val=0;
	uint8_t t;
	for(t=0;t<10;t++)
	{
		temp_val+=T_Get_Adc(ADC_CHANNEL_16);	  //TampSensor
		Delay1ms(5);
	}
	return temp_val/10;
}

 //��ȡͨ��ch��ת��ֵ
//ȡtimes��,Ȼ��ƽ��
uint16_t T_Get_Adc_Average(uint8_t ch,uint8_t times)
{
	uint32_t temp_val=0;
	uint8_t t;
	for(t=0;t<times;t++)
	{
		temp_val+=T_Get_Adc(ch);
		Delay1ms(5);
	}
	return temp_val/times;
} 	   

//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��,��λ:��.)
short Get_Temprate(void)	//��ȡ�ڲ��¶ȴ������¶�ֵ
{
	uint32_t adcx;
	short result;
 	double temperate;
	adcx=T_Get_Adc_Average(ADC_CHANNEL_16,2);	//��ȡͨ��16,20��ȡƽ��
	temperate=(float)adcx*(3.3/4096);		//��ѹֵ 
	temperate=(1.43-temperate)/0.0043+10;	//ת��Ϊ�¶�ֵ 	 
	result=temperate*=100;					//����100��.
	return result;
}




//uint16_t pc3_adc_test_get(void)
//{
//	uint16_t ret ;
//	ret = T_Get_Adc(ADC_CHANNEL_13) ;
//	
//	MY_PRINTF("ret = %d, vol = %f \r\n",ret,ret/4096.0*3.3);
//	
//	return ret;
//}



#else
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
//static uint16_t getAdcAverage(uint8_t ch,uint8_t times)
//{
////	uint32_t temp_val=0;
////	uint8_t t;
////	for(t=0;t<times;t++)
////	{
////		temp_val+=ADC_Read(ch);
////		Delay1ms(5);
////		//delay_1ms(5);
////	}
////	return temp_val/times;
//} 
	 


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


#endif
short get_internal_temp(void)
{
	MY_PRINTF("get_internal_temp  g_int_temp = %d\r\n",g_int_temp);
	
	return g_int_temp;
}






//��ص�ѹ�������,1000ms����һ��
void Int_Temp_task(void)
{
//	uint16_t temp;   //�¶�ֵ
	/* ADC software trigger enable */
    //adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
	
	g_int_temp = Get_Temprate();   //����¶�ֵ(������100��,��λ:��.)
	
	//adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);  /* ADC software trigger enable */
//	MY_PRINTF("g_int_temp = %d\r\n",g_int_temp);
//	pc3_adc_test_get();
}



