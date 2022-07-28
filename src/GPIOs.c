
#include "includes.h"

//PD7 
void Wxen_Control_Init(void)
{
	//1. ʱ��ʹ��
	rcu_periph_clock_enable(RCU_GPIOD);
		
	//2.0 �ϵ��������
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7);  //�������	
	//2. ��ʼ����Ĭ�������
	gpio_bit_reset(GPIOD, GPIO_PIN_7);
	
}



//OE���ŵĳ�ʼ�� 
/*
23.PC0 - OE1  ��·������ƣ�rk3399���Դ��ڣ�rk3399��gd32�Ĵ��ڣ�rk3399��Һ��ͨ�Ŵ��ڣ�SAI8159iic4�ӿ�
24.PC1 - OE2  ��·������ƣ�i2s��i2s0��CB_RESETn��WDT_OUT
25.PC2 - OE3  ��·������ƣ�iic3�ӿڣ�iic1�ӿڣ�GPO6��PCIE_WAKEn��LED_PWM_7IN
26.PC3 - OE4 ��·������ƣ�PTT��GPI5��MIC_DET��PTT1��CODEC_GPI2��GPO4

OE3 �͵�ƽ����3������
*/
void OePins_Control_Init(void)
{
	//1. ʱ��ʹ��
	rcu_periph_clock_enable(RCU_GPIOC);
		
	//2.0 �ϵ��������
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  //�������	
	//2. ��ʼ����Ĭ�������
	gpio_bit_reset(GPIOC, GPIO_PIN_2);  //OE3 �����
	
	gpio_bit_set(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3);  //���� �����
}


//PE15����ʼ���������
void LcdCtrl_Control_Init(void)
{
	//1. ʱ��ʹ��
	rcu_periph_clock_enable(RCU_GPIOE);
		
	//2.0 �ϵ��������
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_15);  //�������	
	//2. ��ʼ����Ĭ�������
	gpio_bit_reset(GPIOE, GPIO_PIN_15);  //LcdCtrl �����

}

//ʹ�ܸ����ţ�ͨ��Lcd��Դ
void LcdCtrl_Enable(void)
{
	gpio_bit_set(GPIOE, GPIO_PIN_15);  //�ߵ�ƽʹ��
}

//��ֹ�����ţ��ϵ�Lcd��Դ
void LcdCtrl_Disable(void)
{
	gpio_bit_reset(GPIOE, GPIO_PIN_15);  //LcdCtrl �����
}


//ʹ�ܸ����ţ�ͨ��1.1v
void Wxen_Control_Enable(void)
{
	gpio_bit_set(GPIOD, GPIO_PIN_7);  //�ߵ�ƽʹ��
}


//��ֹ�����ţ��ϵ�1.1v
void Wxen_Control_Disable(void)
{
	gpio_bit_reset(GPIOD, GPIO_PIN_7);   //�͵�ƽ��Ч
}


//which 1-4 �ֱ��ʾoe1-oe4
void OePins_Output_Hight(uint8_t which)
{
	if(which >0 && which < 5)
	{
		gpio_bit_set(GPIOC, BIT(which-1));  //�����
	}	
}


//which 1-4 �ֱ��ʾoe1-oe4
void OePins_Output_Low(uint8_t which)
{
	if(which >0 && which < 5)
	{
		gpio_bit_reset(GPIOC, BIT(which-1));  //�����
	}
}




