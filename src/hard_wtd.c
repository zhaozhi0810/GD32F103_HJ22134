
/*
	外部的看门狗芯片的处理

	WDO --》 PA4  输入，低电平表示超时
	WDI --》 PA5  			输出，喂狗
	RESET --》 PA6  		输出，低电平复位3399
	SYS_RESET_IN --》 PA7  	输出，输出高，3399复位

	APP设置的喂狗周期
	
	喂狗任务根据APP设置的喂狗周期喂狗
	app不再喂狗，则喂狗任务等待时间到之后将结束喂狗
	

*/
#include "includes.h"



static uint8_t is_hwtd_enable = 0;   //默认看门狗不允许使能
static uint8_t is_uartcmd_reboot_cpu = 0;  //等于0表示不重启，大于0表示重启，2022-10-17
//最大25秒 == 最大值255！！！
volatile static uint16_t hwtd_timeout = 220;    //APP设置的喂狗周期， 最小单位100ms
static uint16_t hwtd_timeout_count = 0;  //喂狗计时值。

void hard_wtd_enable(void)
{
	is_hwtd_enable = 1;
	hwtd_timeout_count = hwtd_timeout;
	MY_PRINTF("hard_wtd_enable\r\n");
}


void hard_wtd_disable(void)
{
	is_hwtd_enable = 0;
	hwtd_timeout_count = 220;
//	MY_PRINTF("hard_wtd_disable\r\n");
}

//固定喂狗，1s喂狗一次
static void hard_wtd_feed_internel(void)
{
	if(gpio_output_bit_get(GPIOA, GPIO_PIN_5))  //原来是高
	{
		gpio_bit_reset(GPIOA, GPIO_PIN_5); //拉低
	}
	else
		gpio_bit_set(GPIOA, GPIO_PIN_5);  //拉高
	
//	MY_PRINTF("hard_wtd_feed_internel\r\n");
}


void hard_wtd_feed(void)
{
	hwtd_timeout_count = hwtd_timeout;  //
	
	MY_PRINTF("hard_wtd_feed\r\n");
}


//获得看门狗的状态 1表示开启，0表示关闭
uint8_t get_hard_wtd_status(void)
{
	return is_hwtd_enable;
}



//设置看门狗超时时间，单位100ms
void hard_wtd_set_timeout(uint8_t timeout)
{
	MY_PRINTF("hard_wtd_set_timeout timeout = %d(*100ms)\r\n",timeout);
	hwtd_timeout = timeout;
}

//获得看门狗超时时间，单位100ms
uint8_t  hard_wtd_get_timeout(void)
{
	MY_PRINTF("hard_wtd_get_timeout timeout = %d(*100ms)\r\n",hwtd_timeout);
	return hwtd_timeout;
}


//3399重启控制
void hard_wtd_reset_3399board(uint8_t delaytime)
{	
	gpio_bit_reset(GPIOC, GPIO_PIN_2);  //OE3 输出低
	
	hard_wtd_disable();   //主板重启后，看门狗关闭
	
//	printf("delaytime = %d\r\n",delaytime);
	
//	if(delaytime < 3)
	{
	//	gpio_bit_reset(GPIOA, GPIO_PIN_6);
		is_uartcmd_reboot_cpu = 3;  //10-17，cpu 重启	
	}
//	else   //参数delaytime >= 3 的时候
//	{
//		is_uartcmd_reboot_cpu = 33;  //10-17，cpu 重启,等待3s	
//	}	
//	Delay1ms(200);
//	gpio_bit_set(GPIOA, GPIO_PIN_6);
//	Delay1ms(200);
//	LT9211_Config();   //10-17，cpu 重启，9211 重启
}






//初始化
void hard_wtd_pins_init(void)
{
	//时钟使能
	rcu_periph_clock_enable(RCU_GPIOA);	
	
	gpio_bit_reset(GPIOA, GPIO_PIN_7);  //7 是高有效
	gpio_bit_set(GPIOA, GPIO_PIN_6);  //6是低有效
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_5);	
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ,  GPIO_PIN_6);	
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7);	

	//4脚是WDO的输出脚，低有效，设置为中断模式吧？？
	gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_4);	

#ifdef 	HWTD_USE_INT   //宏在includes.h中定义
	//中断引脚初始化
	//2. 中断引脚的初始化 PA4，外部中断4
	rcu_periph_clock_enable(RCU_AF);		
	
	//2.2 复用为外部中断引脚，
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_4);
	
	//设置触发方式，下降沿触发
	exti_init(EXTI_4, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_interrupt_enable(EXTI_4);
	exti_interrupt_flag_clear(EXTI_4);
	//2.3 nvic允许中断
	//中断控制器使能，使用的是外部中断12
	nvic_irq_enable(EXTI4_IRQn,  1, 2);   //允许中断，并设置优先级

#endif	

}



#ifdef 	HWTD_USE_INT
//外部中断12的处理函数,按键按下和松开都会触发中断！！！！
void exint4_handle(void)
{
	printf("exint4_handle reset core\r\n");
	if(is_hwtd_enable)   //允许看门狗的情况下，重启
		hard_wtd_reset_3399board(1);
}

#endif


//800ms 看门狗
static void iwdog_init(uint8_t delaytimes)
{	
	fwdgt_write_enable();
	
	if(delaytimes == 0)
	{
		fwdgt_config(0xfff,FWDGT_PSC_DIV8);    //设置分配系数,最长800ms		
	}
	else //暂时没有用到。2022-10-18
	{
		fwdgt_config(0xfff,FWDGT_PSC_DIV64);    //设置分配系数,最长6s	
	}
	fwdgt_enable(); //使能看门狗
	
	while(1);  //程序卡死，等待复位
}


//100ms进入一次就好 SGM706是1.6秒没有喂狗就会复位
//为了解决喂狗时间可以设置的问题，增加喂狗任务
//喂狗任务根据设置的时间喂狗，
void hard_wtd_feed_task(void)
{
	static uint16_t count = 0;
	
	count++;

	if(count > 10)  //1s
	{
		count = 0;
		hard_wtd_feed_internel();   //单片机自身1s周期喂狗
	}
	
	if(is_uartcmd_reboot_cpu)
	{
		//printf("--is_uartcmd_reboot_cpu = %d\r\n",is_uartcmd_reboot_cpu);
		if(is_uartcmd_reboot_cpu == 3)
		{
		//	printf("is_uartcmd_reboot_cpu = 3\r\n");
			gpio_bit_reset(GPIOA, GPIO_PIN_6);
			is_uartcmd_reboot_cpu = 2;
		//	is_hwtd_enable = 0;   //不需要使用看门狗了
		//	hwtd_timeout_count = 220;  //看门狗的时间重新设置为22秒
			return;
		}
		else if(is_uartcmd_reboot_cpu == 2)
		{
			gpio_bit_set(GPIOA, GPIO_PIN_6);
		//	printf("is_uartcmd_reboot_cpu = 2\r\n");
			is_uartcmd_reboot_cpu = 1;
			return;
		}
		else if(is_uartcmd_reboot_cpu == 1)
		{			
		//	g_task_id |= 1<<4 ; //10-17-->9211 重启
		//	LT9211_Config();   //10-17，cpu 重启，9211 重启
			is_uartcmd_reboot_cpu = 0;
			iwdog_init(0);   //单片机复位
			return;
		}
		else if(is_uartcmd_reboot_cpu)
			is_uartcmd_reboot_cpu--;   //其他情况则倒计时
	}
//	printf(",");
	if(is_hwtd_enable)
	{
//		printf(".");
		if(hwtd_timeout_count)  //喂狗倒计时时间未清零
		{
			hwtd_timeout_count--;		
			if(!hwtd_timeout_count) //数值被减到0
			{
				printf("hard_wtd_feed_task timeout\r\n");
				hard_wtd_reset_3399board(1);  				
			}
		}
	}
}
