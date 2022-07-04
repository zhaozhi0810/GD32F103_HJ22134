
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


