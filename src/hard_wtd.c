
/*
	�ⲿ�Ŀ��Ź�оƬ�Ĵ���

	WDO --�� PA4  ���룬�͵�ƽ��ʾ��ʱ
	WDI --�� PA5  			�����ι��
	RESET --�� PA6  		������͵�ƽ��λ3399
	SYS_RESET_IN --�� PA7  	���������ߣ�3399��λ

	APP���õ�ι������
	
	ι���������APP���õ�ι������ι��
	app����ι������ι������ȴ�ʱ�䵽֮�󽫽���ι��
	

*/
#include "includes.h"



static uint8_t is_hwtd_enable = 0;   //Ĭ�Ͽ��Ź�������ʹ��
static uint8_t is_uartcmd_reboot_cpu = 0;  //����0��ʾ������������0��ʾ������2022-10-17
//���25�� == ���ֵ255������
volatile static uint16_t hwtd_timeout = 220;    //APP���õ�ι�����ڣ� ��С��λ100ms
static uint16_t hwtd_timeout_count = 0;  //ι����ʱֵ��

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

//�̶�ι����1sι��һ��
static void hard_wtd_feed_internel(void)
{
	if(gpio_output_bit_get(GPIOA, GPIO_PIN_5))  //ԭ���Ǹ�
	{
		gpio_bit_reset(GPIOA, GPIO_PIN_5); //����
	}
	else
		gpio_bit_set(GPIOA, GPIO_PIN_5);  //����
	
//	MY_PRINTF("hard_wtd_feed_internel\r\n");
}


void hard_wtd_feed(void)
{
	hwtd_timeout_count = hwtd_timeout;  //
	
	MY_PRINTF("hard_wtd_feed\r\n");
}


//��ÿ��Ź���״̬ 1��ʾ������0��ʾ�ر�
uint8_t get_hard_wtd_status(void)
{
	return is_hwtd_enable;
}



//���ÿ��Ź���ʱʱ�䣬��λ100ms
void hard_wtd_set_timeout(uint8_t timeout)
{
	MY_PRINTF("hard_wtd_set_timeout timeout = %d(*100ms)\r\n",timeout);
	hwtd_timeout = timeout;
}

//��ÿ��Ź���ʱʱ�䣬��λ100ms
uint8_t  hard_wtd_get_timeout(void)
{
	MY_PRINTF("hard_wtd_get_timeout timeout = %d(*100ms)\r\n",hwtd_timeout);
	return hwtd_timeout;
}


//3399��������
void hard_wtd_reset_3399board(uint8_t delaytime)
{	
	gpio_bit_reset(GPIOC, GPIO_PIN_2);  //OE3 �����
	
	hard_wtd_disable();   //���������󣬿��Ź��ر�
	
//	printf("delaytime = %d\r\n",delaytime);
	
//	if(delaytime < 3)
	{
	//	gpio_bit_reset(GPIOA, GPIO_PIN_6);
		is_uartcmd_reboot_cpu = 3;  //10-17��cpu ����	
	}
//	else   //����delaytime >= 3 ��ʱ��
//	{
//		is_uartcmd_reboot_cpu = 33;  //10-17��cpu ����,�ȴ�3s	
//	}	
//	Delay1ms(200);
//	gpio_bit_set(GPIOA, GPIO_PIN_6);
//	Delay1ms(200);
//	LT9211_Config();   //10-17��cpu ������9211 ����
}






//��ʼ��
void hard_wtd_pins_init(void)
{
	//ʱ��ʹ��
	rcu_periph_clock_enable(RCU_GPIOA);	
	
	gpio_bit_reset(GPIOA, GPIO_PIN_7);  //7 �Ǹ���Ч
	gpio_bit_set(GPIOA, GPIO_PIN_6);  //6�ǵ���Ч
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_5);	
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ,  GPIO_PIN_6);	
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7);	

	//4����WDO������ţ�����Ч������Ϊ�ж�ģʽ�ɣ���
	gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_4);	

#ifdef 	HWTD_USE_INT   //����includes.h�ж���
	//�ж����ų�ʼ��
	//2. �ж����ŵĳ�ʼ�� PA4���ⲿ�ж�4
	rcu_periph_clock_enable(RCU_AF);		
	
	//2.2 ����Ϊ�ⲿ�ж����ţ�
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_4);
	
	//���ô�����ʽ���½��ش���
	exti_init(EXTI_4, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_interrupt_enable(EXTI_4);
	exti_interrupt_flag_clear(EXTI_4);
	//2.3 nvic�����ж�
	//�жϿ�����ʹ�ܣ�ʹ�õ����ⲿ�ж�12
	nvic_irq_enable(EXTI4_IRQn,  1, 2);   //�����жϣ����������ȼ�

#endif	

}



#ifdef 	HWTD_USE_INT
//�ⲿ�ж�12�Ĵ�����,�������º��ɿ����ᴥ���жϣ�������
void exint4_handle(void)
{
	printf("exint4_handle reset core\r\n");
	if(is_hwtd_enable)   //�����Ź�������£�����
		hard_wtd_reset_3399board(1);
}

#endif


//800ms ���Ź�
static void iwdog_init(uint8_t delaytimes)
{	
	fwdgt_write_enable();
	
	if(delaytimes == 0)
	{
		fwdgt_config(0xfff,FWDGT_PSC_DIV8);    //���÷���ϵ��,�800ms		
	}
	else //��ʱû���õ���2022-10-18
	{
		fwdgt_config(0xfff,FWDGT_PSC_DIV64);    //���÷���ϵ��,�6s	
	}
	fwdgt_enable(); //ʹ�ܿ��Ź�
	
	while(1);  //���������ȴ���λ
}


//100ms����һ�ξͺ� SGM706��1.6��û��ι���ͻḴλ
//Ϊ�˽��ι��ʱ��������õ����⣬����ι������
//ι������������õ�ʱ��ι����
void hard_wtd_feed_task(void)
{
	static uint16_t count = 0;
	
	count++;

	if(count > 10)  //1s
	{
		count = 0;
		hard_wtd_feed_internel();   //��Ƭ������1s����ι��
	}
	
	if(is_uartcmd_reboot_cpu)
	{
		//printf("--is_uartcmd_reboot_cpu = %d\r\n",is_uartcmd_reboot_cpu);
		if(is_uartcmd_reboot_cpu == 3)
		{
		//	printf("is_uartcmd_reboot_cpu = 3\r\n");
			gpio_bit_reset(GPIOA, GPIO_PIN_6);
			is_uartcmd_reboot_cpu = 2;
		//	is_hwtd_enable = 0;   //����Ҫʹ�ÿ��Ź���
		//	hwtd_timeout_count = 220;  //���Ź���ʱ����������Ϊ22��
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
		//	g_task_id |= 1<<4 ; //10-17-->9211 ����
		//	LT9211_Config();   //10-17��cpu ������9211 ����
			is_uartcmd_reboot_cpu = 0;
			iwdog_init(0);   //��Ƭ����λ
			return;
		}
		else if(is_uartcmd_reboot_cpu)
			is_uartcmd_reboot_cpu--;   //��������򵹼�ʱ
	}
//	printf(",");
	if(is_hwtd_enable)
	{
//		printf(".");
		if(hwtd_timeout_count)  //ι������ʱʱ��δ����
		{
			hwtd_timeout_count--;		
			if(!hwtd_timeout_count) //��ֵ������0
			{
				printf("hard_wtd_feed_task timeout\r\n");
				hard_wtd_reset_3399board(1);  				
			}
		}
	}
}
