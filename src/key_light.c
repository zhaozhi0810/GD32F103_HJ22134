

/*
	按键灯的操作
	使用local bus的方式
	
	地址  A0~A5
	数据  D0
	CS    CS
7. PD11- 3V3_A0	
13.PE2 - 3V3_A1
14.PE3 - 3V3_A2
15.PE4 - 3V3_A3
16.PE5 - 3V3_A4
17.PE6 - 3V3_A5

9.PD14- 3V3_D0

19.PE8 - KEYBOARD_CS	
	
	状态灯，三色灯（这个由3399控制，不由单片机控制）
	每个引脚控制一个颜色，共阳


灯亮度控制，根据原理图，这个引脚与lcd的pwm冲突，无法同时使用定时器
20.PE9-PWM_LED （TIM0-CH0）//可以用io口模拟吧


*/

#include "includes.h"


#define ENABLE_KEYBOARD_CS 1
#define DISABLE_KEYBOARD_CS 0



void key_light_leds_init(void)
{
	uint32_t pin;
	//都是输出引脚
	//1. 时钟使能
	rcu_periph_clock_enable(RCU_GPIOD);
		
	//2.0 上电控制引脚  (D11  -- A0   D14  -- D0)
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_11 | GPIO_PIN_14);  //控制输出	
	//2. 初始化后，默认输出低
	gpio_bit_reset(GPIOD, GPIO_PIN_11 | GPIO_PIN_14);
	
	
	rcu_periph_clock_enable(RCU_GPIOE);
	pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;	
	//2.0 上电控制引脚
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pin);  //控制输出	
	//2. 初始化后，默认输出低
	gpio_bit_reset(GPIOE, pin);
	
	
//	rcu_periph_clock_enable(RCU_GPIOE);	
	//2.0 上电控制引脚
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_8 | GPIO_PIN_9);  //控制输出	
	//2. 初始化后，默认输出高
	gpio_bit_set(GPIOE, GPIO_PIN_8 | GPIO_PIN_9);
	
	//PE9 是pwm控制，现在输出高，全亮
}



/*
	status   0 表示禁止，输出高电平
			 非0表示选中，输出低电平
*/
static void key_light_cs(uint8_t status)
{
	if(status)
		gpio_bit_reset(GPIOE, GPIO_PIN_8 );
	else
		gpio_bit_set(GPIOE, GPIO_PIN_8 );
}




static void key_light_send_addr(uint8_t addr)
{
//	uint16_t val;
//	val = gpio_output_port_get(GPIOE);
//	
//	
//	val &= ~(0x1f<<2);   //清零
//	val |= ((addr&0x3e)<<1); 
//	
//	gpio_port_write(GPIOE, val);
	
	if(addr&1)
		gpio_bit_set(GPIOD, GPIO_PIN_11);   //A0
	else
		gpio_bit_reset(GPIOD, GPIO_PIN_11);
	
	if(addr&2)
		gpio_bit_set(GPIOE, GPIO_PIN_2);   //A1
	else
		gpio_bit_reset(GPIOE, GPIO_PIN_2);
	
	if(addr&4)
		gpio_bit_set(GPIOE, GPIO_PIN_3);   //A2
	else
		gpio_bit_reset(GPIOE, GPIO_PIN_3);
	
	if(addr&8)
		gpio_bit_set(GPIOE, GPIO_PIN_4);   //A3
	else
		gpio_bit_reset(GPIOE, GPIO_PIN_4);
	
	if(addr&0x10)
		gpio_bit_set(GPIOE, GPIO_PIN_5);   //A4
	else
		gpio_bit_reset(GPIOE, GPIO_PIN_5);
	
	if(addr&0x20)
		gpio_bit_set(GPIOE, GPIO_PIN_6);   //A5
	else
		gpio_bit_reset(GPIOE, GPIO_PIN_6);
	
}




/*
	whichled  32表示所有灯
			0-31 分别对应按键的灯
			 
	status   0 表示熄灭
			 非0表示点亮
*/
void key_light_leds_control(uint8_t whichled,uint8_t status)
{	

	if(whichled < 33)  //whichled>0 && 
	{
		key_light_send_addr(whichled);		
	}
	else
		return;
	
	key_light_cs(ENABLE_KEYBOARD_CS);
	if(status)	
		gpio_bit_set(GPIOD, GPIO_PIN_14);
	else
		gpio_bit_reset(GPIOD, GPIO_PIN_14);	
	
	Delay1ms(1);
	key_light_cs(DISABLE_KEYBOARD_CS);
}




void key_light_allleds_control(uint8_t status)
{
	key_light_leds_control(32,status);		
}















