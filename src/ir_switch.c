

#include "includes.h"

/*
	PC9  ir_out 红外接收段   timer2ch2 全映射  TIMER7_CH3
	PC8  红外发射管  		timer2ch3 全映射  TIMER7_CH2


	根据红外的原理，我只要发出38kHZ的信号，接收头就会默认为接收到高电平，经三极管后，输出低电平即可

	接收引脚为低电平，即可认为收到了红外发射的信号，没收到则为高电平

	PC8 设置为pwm 38KHZ(占空比50%)，持续发送  （为了省电可以100ms发送一次）使用定时器7
	PC9 输入，低有效（表示接收到了信号，表示未佩戴），高则无效（表示没有收到信号，表示已经佩戴）
*/


#define TIMER2_FULL_MAP


#define PWM_PIN GPIO_PIN_8
#define PWM_PORT GPIOC
#define PWM_PORT_RCU RCU_GPIOC
#ifdef TIMER2_FULL_MAP
#define PWM_TIMER_RCU  RCU_TIMER2    //
#define PWM_TIMER  TIMER2
#else
#define PWM_TIMER_RCU  RCU_TIMER7    // 有些32没得timer7，这需要注意
#define PWM_TIMER  TIMER7
#endif
#define PWM_TIMER_CH TIMER_CH_2

static uint16_t IR_PWM_DEGREE_MAX = (uint16_t)(1000/38);   //PWM频率  26为38KHZ   1us计个数，4000计算的频率就是1000000/4000=250Hz 



void ir_pwm_init(void)
{
	uint16_t degree = 100;    //占空比50%
	//PB15 通道
	timer_parameter_struct initpara;
	timer_oc_parameter_struct ocpara;
	
	//1. 定时器时钟使能
	rcu_periph_clock_enable(PWM_TIMER_RCU );  //定时器模块时钟使能
	rcu_periph_clock_enable(PWM_PORT_RCU); 
	rcu_periph_clock_enable(RCU_AF); 
	
	//2. io引脚设置复用功能	
	gpio_init(PWM_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, PWM_PIN);   //复用功能	
#ifdef TIMER2_FULL_MAP
	//没有timer7的时候，需要使用全映射
	gpio_pin_remap_config(GPIO_TIMER2_FULL_REMAP, ENABLE);    //timer2全映射
#endif	
	

	
	if(degree > 100)
	{
		degree = 100;
	}
	
	//3. 初始化定时器的数据结构  /* initialize TIMER init parameter struct */
	timer_struct_para_init(&initpara);
	initpara.period = IR_PWM_DEGREE_MAX-1;  //重载的数字，频率38kHZ
	initpara.prescaler = (SystemCoreClock/1000000)-1;  //预分频数，得到是1Mhz的脉冲  
		
	//4. 初始化定时器      /* initialize TIMER counter */
	timer_init(PWM_TIMER, &initpara);
		
	//5. 初始化定时器通道的数据结构 /* initialize TIMER channel output parameter struct */
	timer_channel_output_struct_para_init(&ocpara);	
#ifndef PWM_TIMER_USE_CHN
	ocpara.outputstate  = TIMER_CCX_ENABLE;  //输出通道使能	
#else
    ocpara.outputnstate = TIMER_CCXN_ENABLE;//使能互补通道输出
#endif
	//6. 初始化定时器通道   /* configure TIMER channel output function */
	timer_channel_output_config(PWM_TIMER, PWM_TIMER_CH, &ocpara);
			
	//7. 初始化定时器通道输出方式设置   /* configure TIMER channel output compare mode */
	timer_channel_output_mode_config(PWM_TIMER, PWM_TIMER_CH, TIMER_OC_MODE_PWM1);
	/* configure TIMER channel output pulse value */
	
	//8. 初始化定时器通道输出脉冲宽带
	timer_channel_output_pulse_value_config(PWM_TIMER, PWM_TIMER_CH, (100-degree) * IR_PWM_DEGREE_MAX/100);

	//9. 初始化定时器通道输出使能
	//timer_channel_output_fast_config(TIMER2, TIMER_CH_0, TIMER_OC_FAST_ENABLE);
	timer_channel_output_shadow_config(PWM_TIMER, PWM_TIMER_CH, TIMER_OC_SHADOW_DISABLE);	  //stm32似乎用的是这个0x8
	//10.初始化，定时器不使能 2022-04-18	
	
	
	/* enable a TIMER */
	if(PWM_TIMER == TIMER0 || PWM_TIMER == TIMER7)
		timer_primary_output_config(PWM_TIMER, ENABLE);
	timer_auto_reload_shadow_enable(PWM_TIMER);
	
//	timer_enable(PWM_TIMER);   //开启定时器}
}



//开启定时器则发送38Khz频率，此时接收器的数据为1，输出电平为0
void ir_send_high(void)
{
	gpio_init(PWM_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, PWM_PIN);
	timer_enable(PWM_TIMER);   //开启定时器}
}

//关闭定时器则不再发送38Khz频率，此时接收器的数据为0，输出电平为1
void ir_send_low(void)
{
	timer_disable(PWM_TIMER);   //关闭定时器}
	gpio_init(PWM_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PWM_PIN);
	gpio_bit_reset(PWM_PORT, PWM_PIN);
}




//发送引导码
__STATIC_INLINE void IR_NEC_Send_Guide(void)
{
    ir_send_high();   //开启定时器}
    Delay1us(9000); // 9ms闪
    ir_send_low();
    Delay1us(4500); // 4.5ms灭
    //共13.5ms
}


//发送数据0
__STATIC_INLINE void IR_NEC_Send_0(void)
{
    ir_send_high();   //开启定时器}
    Delay1us(560); // 0.56ms闪
    ir_send_low();
    Delay1us(560); // 0.56ms灭
    //共1.12ms
}


//发送数据1
__STATIC_INLINE void IR_NEC_Send_1(void)
{
    ir_send_high();   //开启定时器}
    Delay1us(560); // 0.56ms闪
    ir_send_low();
    Delay1us(1690); // 1.69ms灭
    //共2.25ms
}

//结束位
void IR_NEC_Send_End(void)
{
    ir_send_high();
    Delay1us(650); // 6ms亮
    ir_send_low();
}



// NEC编码发送
void IR_NEC_Send_Code(const uint8_t *Dat, uint8_t Len)
{
    uint8_t zj;
	uint8_t j;
    IR_NEC_Send_Guide();          //引导码
    for (j = 0; j < Len; j++) //循环数组
    {
        zj = Dat[j];
        for (int i = 0; i < 8; i++) //从高到低
        {
            if (zj & (1))   //先发送最低位
            {
                IR_NEC_Send_1();
            }
            else
            {
                IR_NEC_Send_0();
            }
            zj >>= 1;  //先发送最低位
        }
    }
    IR_NEC_Send_End(); //结束位
//    Delay1ms(40);
}


//nec ir发送的数据是4个字节（32bits）地址，地址反码，数据，数据反码（所有都是低位先发）
const  uint8_t ir_Send_DAT[5] = {0X10, 0X68, 0X80, 0X03, 0X0};  //ir发送的数据

//void ir_send_code(void)
//{	
//	IR_NEC_Send_Code(DAT, 4);
//}


