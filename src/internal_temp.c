


#include "includes.h"


/*
	获取单片机内部温度
*/


static short g_int_temp = 0;  //温度值，扩大了100倍



#if 1

//ADC12_IN13
//static void pc3_adc_test_init(void)
//{
//	rcu_periph_clock_enable(RCU_GPIOC);
//	gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
//}



//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3	
void ADC_Init(void)  //ADC通道初始化
{
    // ADC时钟使能
    rcu_periph_clock_enable(RCU_ADC0);
    // ADC时钟8分频，最大14MHz
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);
    
  
    /*------------------ADC工作模式配置------------------*/
    // 只使用一个ADC,属于独立模式
    adc_mode_config(ADC_MODE_FREE);
    // 多通道用扫描模式
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);
    // 单通道用连续转换模式
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    
	adc_discontinuous_mode_config(ADC0, ADC_REGULAR_CHANNEL, 1);   //规则模式下1个通道
	
    // 结果转换右对齐
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    // 转换通道1个
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);
    
	//内部温度通道
	adc_tempsensor_vrefint_enable();  //温度通道开启
	adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_16, ADC_SAMPLETIME_55POINT5);
//	pc3_adc_test_init();
	// 配置ADC通道转换顺序，采样时间为55.5个时钟周期
//	adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_13, ADC_SAMPLETIME_55POINT5);
	
    // 不用外部触发转换，软件开启即可
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
//    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
    
    // 使能ADC
    adc_enable(ADC0);
    Delay1ms(1);      // 等待1ms
    // 使能ADC校准
    adc_calibration_enable(ADC0);
  
    // 由于没有采用外部触发，所以使用软件触发ADC转换
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
//	}                       // 等待采样完成
    adc_flag_clear(ADC0, ADC_FLAG_EOC);                             // 清除结束标志
    
    adcValue = adc_regular_data_read(ADC0);                         // 读取ADC数据

     // 配置ADC通道转换顺序，采样时间为55.5个时钟周期
    adc_regular_channel_config(ADC0, 0, ch, ADC_SAMPLETIME_55POINT5);
    // 由于没有采用外部触发，所以使用软件触发ADC转换
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);  

	return adcValue;

}

//得到ADC采样内部温度传感器的值
//取10次,然后平均
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

 //获取通道ch的转换值
//取times次,然后平均
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

//得到温度值
//返回值:温度值(扩大了100倍,单位:℃.)
short Get_Temprate(void)	//获取内部温度传感器温度值
{
	uint32_t adcx;
	short result;
 	double temperate;
	adcx=T_Get_Adc_Average(ADC_CHANNEL_16,2);	//读取通道16,20次取平均
	temperate=(float)adcx*(3.3/4096);		//电压值 
	temperate=(1.43-temperate)/0.0043+10;	//转换为温度值 	 
	result=temperate*=100;					//扩大100倍.
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
 @brief ADC驱动初始化
 @param 无
 @return 无
*/
void ADC_Init(void)
{
    /*------------------时钟配置------------------*/
    // GPIO时钟使能
//    rcu_periph_clock_enable(RCU_GPIOC);
    // ADC时钟使能
    rcu_periph_clock_enable(RCU_ADC0);
    // ADC时钟8分频，最大14MHz
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV12);

    /*------------------ADC GPIO配置------------------*/
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
 @brief ADC读取
 @param channel -[in] ADC通道
 @return ADC采样值
*/
uint16_t ADC_Read(uint8_t channel)
{
 //   uint16_t adcValue = 0;
    
    	
	return ADC_IDATA0(ADC0);
	
//    return adcValue;
}



//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
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
	电池电压，正常4.2v，但是进行了1/2分压，调试再看看吧
	电压值的计算 3.3*val /1024 就得到计算出来的电压
*/
//uint16_t ADCgetBatVol(void)
//{	
//	return getAdcAverage(ADC_CHANNEL_0,3)* 66 /1024;   //因为2分压了，所以乘3.3*2,扩大10倍，就不再使用小数
//}


/*********************************************************************************************************
** Function name:           Get_Temprate
** Descriptions:            获取STM32内部温度传感器的值
** input parameters:        none
** Returned value:          温度值(扩大了100倍,单位:℃.)
*********************************************************************************************************/
static short Get_Temprate(void)    //获取内部温度传感器温度值
{
    uint32_t adcx;
    short result;
    float temperature;
    adcx = ADC_Read(ADC_CHANNEL_16);//(ADC_CHANNEL_16,5);  //读取通道16,5次取平均
//    MY_PRINTF("adcx = %d\r\n",adcx);
	temperature = (1.43 - adcx*3.3/4096) * 1000 / 4.3 + 10;     //25 --> 10
    result = temperature*=100;                  //扩大100倍.
    return result;
}


#endif
short get_internal_temp(void)
{
	MY_PRINTF("get_internal_temp  g_int_temp = %d\r\n",g_int_temp);
	
	return g_int_temp;
}






//电池电压检测任务,1000ms进入一次
void Int_Temp_task(void)
{
//	uint16_t temp;   //温度值
	/* ADC software trigger enable */
    //adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
	
	g_int_temp = Get_Temprate();   //获得温度值(扩大了100倍,单位:℃.)
	
	//adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);  /* ADC software trigger enable */
//	MY_PRINTF("g_int_temp = %d\r\n",g_int_temp);
//	pc3_adc_test_get();
}



