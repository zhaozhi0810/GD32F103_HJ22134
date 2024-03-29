
#include "includes.h"





//PD7 
void Wxen_Control_Init(void)
{
	//1. 时钟使能
	rcu_periph_clock_enable(RCU_GPIOD);
		
	//2.0 上电控制引脚
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7);  //控制输出	
	//2. 初始化后，默认输出高
	gpio_bit_reset(GPIOD, GPIO_PIN_7);
	
}


//PD6  MicCtl
void MicCtl_Control_Init(void)
{
	//1. 时钟使能
	rcu_periph_clock_enable(RCU_GPIOD);
		
	//2.0 上电控制引脚
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_6);  //控制输出	
	//2. 初始化后，默认输出低
	gpio_bit_reset(GPIOD, GPIO_PIN_6);	
}

//PD6  MicCtl 输出控制(参数status 非0输出高，0输出低)
void MicCtl_Control_OutHigh(uint8_t status)
{
	MY_PRINTF("MicCtl_Control_OutHigh status = %d\r\n",status);
	if(status)
		gpio_bit_set(GPIOD, GPIO_PIN_6);
	else
		gpio_bit_reset(GPIOD, GPIO_PIN_6);
}

////PD6  MicCtl输出低
//void MicCtl_Control_OutLow(void)
//{
//	gpio_bit_reset(GPIOD, GPIO_PIN_6);
//}







//PD8  lcd-reset ,不知道是高电平复位还是低电平复位
//先假设低电平复位吧。
void lcd_reset_control_init(void)
{
	//1. 时钟使能
	rcu_periph_clock_enable(RCU_GPIOD);
		
	//2.0 上电控制引脚
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_8);  //控制输出	
	//2. 初始化后，默认输出高
	gpio_bit_set(GPIOD, GPIO_PIN_8);	
}


//触发一次lcd的复位信号
void lcd_reset_control(void)
{
	gpio_bit_reset(GPIOD, GPIO_PIN_8);
	Delay1ms(100);
	gpio_bit_set(GPIOD, GPIO_PIN_8);
}





//OE引脚的初始化 
/*
23.PC0 - OE1  电路输出控制，rk3399调试串口，rk3399与gd32的串口，rk3399与液晶通信串口，SAI8159iic4接口
24.PC1 - OE2  电路输出控制，i2s，i2s0，CB_RESETn，WDT_OUT
25.PC2 - OE3  电路输出控制，iic3接口，iic1接口，GPO6，PCIE_WAKEn，LED_PWM_7IN
26.PC3 - OE4 电路输出控制，PTT，GPI5，MIC_DET，PTT1，CODEC_GPI2，GPO4

OE3 低电平保持3秒左右
*/
void OePins_Control_Init(void)
{
	//1. 时钟使能
	rcu_periph_clock_enable(RCU_GPIOC);
		
	//2.0 上电控制引脚
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  //控制输出	
	//2. 初始化后，默认输出高
	gpio_bit_reset(GPIOC, GPIO_PIN_2);  //OE3 输出低
	
	gpio_bit_set(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3);  //其他 输出高
}


//PE15，初始化后输出低
void LcdCtrl_Control_Init(void)
{
	//1. 时钟使能
	rcu_periph_clock_enable(RCU_GPIOE);
		
	//2.0 上电控制引脚
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_15);  //控制输出	
	//2. 初始化后，默认输出高
	gpio_bit_reset(GPIOE, GPIO_PIN_15);  //LcdCtrl 输出低

}

//使能该引脚，通电Lcd电源
void LcdCtrl_Enable(void)
{
	gpio_bit_set(GPIOE, GPIO_PIN_15);  //高电平使能
}

//禁止该引脚，断电Lcd电源
void LcdCtrl_Disable(void)
{
	gpio_bit_reset(GPIOE, GPIO_PIN_15);  //LcdCtrl 输出低
}


//使能该引脚，通电1.1v
void Wxen_Control_Enable(void)
{
	gpio_bit_set(GPIOD, GPIO_PIN_7);  //高电平使能
}


//禁止该引脚，断电1.1v
void Wxen_Control_Disable(void)
{
	gpio_bit_reset(GPIOD, GPIO_PIN_7);   //低电平无效
}


//which 1-4 分别表示oe1-oe4
void OePins_Output_Hight(uint8_t which)
{
	if(which >0 && which < 5)
	{
		gpio_bit_set(GPIOC, BIT(which-1));  //输出高
	}	
}


//which 1-4 分别表示oe1-oe4
void OePins_Output_Low(uint8_t which)
{
	if(which >0 && which < 5)
	{
		gpio_bit_reset(GPIOC, BIT(which-1));  //输出低
	}
}


//获得lcd屏幕的类型，2022-09-21之后新做的底板，通过与按键板的三个引脚相连
//读取引脚的电平，判断lcd的类型，之前是由3399的引脚判断的。
//返回值0表示5寸屏，非0表示7寸屏
//2022-09-21 目前还没有新的底板用于判断，默认返回0（表示5寸屏）
uint8_t Get_Lcd_Type(void)
{
	return 0;   //2022-09-21 目前还没有新的底板用于判断，默认返回0（表示5寸屏）
}





