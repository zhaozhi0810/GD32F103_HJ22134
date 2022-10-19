

/*
	�����ƵĲ���
	ʹ��local bus�ķ�ʽ
	
	��ַ  A0~A5
	����  D0
	CS    CS
7. PD11- 3V3_A0	
13.PE2 - 3V3_A1
14.PE3 - 3V3_A2
15.PE4 - 3V3_A3
16.PE5 - 3V3_A4
17.PE6 - 3V3_A5

9.PD14- 3V3_D0

19.PE8 - KEYBOARD_CS	
	
	״̬�ƣ���ɫ�ƣ������3399���ƣ����ɵ�Ƭ�����ƣ�
	ÿ�����ſ���һ����ɫ������


�����ȿ��ƣ�����ԭ��ͼ�����������lcd��pwm��ͻ���޷�ͬʱ(����)ʹ�ö�ʱ��
20.PE9-PWM_LED ��TIM0-CH0��//������io��ģ���


*/

#include "includes.h"


#define ENABLE_KEYBOARD_CS 1
#define DISABLE_KEYBOARD_CS 0
#define LEDS_NUM_MAX 40   //���ֵ40������ֵ1-40��Ч 


static uint8_t g_led_pwm = 100;
static uint64_t leds_status = 0;   //ÿһλ��ʾһ��led��״̬������32����1��ʾ����0��ʾ��
#define PWM_HZ 100   //led��pwmΪ100HZ������ʱ��ÿ10ms����һ�Σ���Ϊ100HZ ���Ҫ��Ƶ�ʵøĶ�ʱ�������ʱ��

#define ARRAY_NUM (LEDS_NUM_MAX/32 + !!(LEDS_NUM_MAX%32))
//static uint32_t g_leds_flash_control[ARRAY_NUM] = {0};  //led����˸���ƣ�ÿһλ��Ӧһ��led����˸��1��ʾ��˸��0��ʾ����˸
static uint8_t g_leds_flash_time[LEDS_NUM_MAX] = {0};  //led ��Ҫ��˸��ʱ��ֵ��0��ʾ����˸��5��ʾ500ms��8��ʾ800ms��10��ʾ1s��20��ʾ2s
static uint8_t g_leds_flash_time_already[4] = {0};   //led�Ѿ���˸��ʱ��,ֻ��¼ĳ����˸��ʱ�䣬����ĳһ��
static uint8_t g_leds_flash_action;      // 0-3λ���ֱ��ʾ5��8��10��20�����������1��ʾ����0��ʾ��
static uint8_t g_leds_flash_control;	//0��ʾû�е���Ҫ��˸�ˣ���0��ʾ���е���Ҫ��˸


#define LEDS_PWM_HZ 20   //20��ʾled��pwmΪ50HZ����ʱ��ÿ1ms����һ��



//2022-09-16 ����timer1 �����ж��źţ����޸�pwm�����ֵ
//ֻ�����ڼ�ʱ����ʹ��tim1�ˣ�
static void TIM1_Led_Pwm_Init(uint16_t arr,uint16_t psc);


void key_light_leds_init(void)
{
	uint32_t pin;
	//�����������
	//1. ʱ��ʹ��
	rcu_periph_clock_enable(RCU_GPIOD);
		
	//2.0 �ϵ��������  (D11  -- A0   D14  -- D0)
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_11 | GPIO_PIN_14);  //�������	
	//2. ��ʼ����Ĭ�������
	gpio_bit_reset(GPIOD, GPIO_PIN_11 | GPIO_PIN_14);
	
	
	rcu_periph_clock_enable(RCU_GPIOE);
	pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;	
	//2.0 �ϵ��������
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pin);  //�������	
	//2. ��ʼ����Ĭ�������
	gpio_bit_reset(GPIOE, pin);
	
	
//	rcu_periph_clock_enable(RCU_GPIOE);	
	//2.0 �ϵ��������
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_8 | GPIO_PIN_9);  //�������	
	//2. ��ʼ����Ĭ�������
	gpio_bit_set(GPIOE, GPIO_PIN_8 | GPIO_PIN_9);
	
	//���ڿ���PE9��PWM��ʱ����ʼ������û�п�����ʱ������
	TIM1_Led_Pwm_Init(1000-1,(SystemCoreClock/1000000)-1);  //1ms ��ʱ
}



/*
	status   0 ��ʾ��ֹ������ߵ�ƽ
			 ��0��ʾѡ�У�����͵�ƽ
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
#if 1
	uint16_t val;
	val = gpio_output_port_get(GPIOE);
		
	val &= ~(0x1f<<2);   //����
	val |= ((addr&0x3e)<<1); 
	
	gpio_port_write(GPIOE, val);
#else	
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
#endif
	if(addr&1)
		gpio_bit_set(GPIOD, GPIO_PIN_11);   //A0
	else
		gpio_bit_reset(GPIOD, GPIO_PIN_11);

}



static void key_light_leds_control2(uint8_t whichled,uint8_t status)
{		
	if(whichled <= LEDS_NUM_MAX)  //whichled>0 && 
	{
		key_light_send_addr(whichled);		
	}
	else
		return;

	key_light_cs(ENABLE_KEYBOARD_CS);
	if(status)	
	{
		gpio_bit_set(GPIOD, GPIO_PIN_14);
		
		if(whichled == 40)  //��¼led��״̬
			leds_status = ~0ULL;	 //ȫ������
		else
			leds_status |= 1<<whichled;
	}
	else
	{
		gpio_bit_reset(GPIOD, GPIO_PIN_14);	
		
		if(whichled == 40)  //��¼led��״̬
			leds_status = 0;	 //ȫ������
		else
			leds_status &= ~(1<<whichled);
	}
	
	Delay1ms(1);
	key_light_cs(DISABLE_KEYBOARD_CS);
}

/*
	whichled  32��ʾ���е�
			0-31 �ֱ��Ӧ�����ĵ�
			 
	status   0 ��ʾϨ��
			 ��0��ʾ����
*/
void key_light_leds_control(uint8_t whichled,uint8_t status)
{		
	
	if(whichled > LEDS_NUM_MAX)  //whichled>0 && 	
		return;

#ifdef LEDS_FLASH_TASK	
//	printf("key_light_leds_control g_leds_flash_control = %#x\r\n",g_leds_flash_control[0]);
//	if(whichled<32)	
//		g_leds_flash_control[0] &= ~(1<<whichled);
//	else if((whichled < 64) && (32 < LEDS_NUM_MAX))
//		g_leds_flash_control[1] &= ~(1<<(whichled-32)); //ȥ��led����˸����
//	printf("2key_light_leds_control g_leds_flash_control = %#x\r\n",g_leds_flash_control[0]);
#endif	
	key_light_leds_control2(whichled,status);
	
//	if(whichled < LEDS_NUM_MAX)  //whichled>0 && 
//	{
//		key_light_send_addr(whichled);		
//	}
//	else
//		return;
//	
//	key_light_cs(ENABLE_KEYBOARD_CS);
//	if(status)	
//	{
//		gpio_bit_set(GPIOD, GPIO_PIN_14);
//		
//		if(whichled == 40)  //��¼led��״̬
//			leds_status = ~0ULL;	 //ȫ������
//		else
//			leds_status |= 1<<whichled;
//	}
//	else
//	{
//		gpio_bit_reset(GPIOD, GPIO_PIN_14);	
//		
//		if(whichled == 40)  //��¼led��״̬
//			leds_status = 0;	 //ȫ������
//		else
//			leds_status &= ~(1<<whichled);
//	}
//	
//	Delay1ms(1);
//	key_light_cs(DISABLE_KEYBOARD_CS);
}






//���ĳһ���Ƶ�״̬
//����255��ʾ����0��1��ʾ��ȷ
uint8_t get_led_status(uint8_t whichled)
{
	if(whichled > 50)  //whichled>0 && 
	{
		return 255;		
	}	
	return (leds_status>>whichled) & 1;	
}



//�԰������������led�Ŀ���
void key_light_allleds_control(uint8_t status)
{
	key_light_leds_control(40,status);		
}


//����led������ [0-100]
//void set_Led_Pwm(uint8_t pwm)
//{
//	if(pwm > 100)
//		pwm = 100;
//	
//	g_led_pwm = pwm;
//	
//	MY_PRINTF("set_Led_Pwm  g_led_pwm = %d\r\n",g_led_pwm);
//}



////�԰�������� led_PWM ���ŵĿ���
//static void led_pwm_pin_control(uint8_t status)
//{
//	if(status)
//		gpio_bit_set(GPIOE, GPIO_PIN_9);   //����ߵ�ƽ
//	else
//		gpio_bit_reset(GPIOE, GPIO_PIN_9);  //����͵�ƽ
//}



#ifdef LEDS_FLASH_TASK
//����ĳ��led����˸
/*
	whichled ����λ��ʾ��˸������
			 ��6λ��ʾ�ĸ�����˸��[1-40]
*/
static const uint8_t g_const_led_flash_time[4] = {5,8,10,20};


void light_leds_add_flash(uint8_t whichled)
{
//	printf("light_leds_add_flash whichled = %d\r\n",whichled);
	uint8_t flash_freq = whichled >> 6;  
	uint8_t i;
	whichled = (whichled & 0x3f) - 1;   //����λ��ʾĳ���ƣ�ԭ����ȡֵ��1-40������Ϊ0-39

		
	if(whichled < LEDS_NUM_MAX)
	{		
		if(whichled == 39)  //40��ʾȫ���ĵ�
		{
//			g_leds_flash_control[0] |= ~0;   //ȫ��������
//			if(32 < LEDS_NUM_MAX)
//				g_leds_flash_control[1] |= ~0; 
//			
			for(i=0;i<LEDS_NUM_MAX;i++)
			{
				g_leds_flash_time[i] = g_const_led_flash_time[flash_freq];
			}
			
		}
		else{			
		//	printf("light_leds_add_flash whichled = %d\r\n",whichled);
//			if(whichled<32)	
//				g_leds_flash_control[0] |= 1<<whichled;
//			else if((whichled < 64) && (32 < LEDS_NUM_MAX))
//				g_leds_flash_control[1] |= 1<<(whichled-32);
//			
//			
			g_leds_flash_time[whichled] = g_const_led_flash_time[flash_freq];
			
		//g_leds_flash_control |= 1<<whichled;
		}
	}
//	else
//		g_leds_flash_control |= ~0L;   //ȫ��������
}
#endif





//100HZ��Ƶ�ʣ�10ms����һ��
//void leds_run_pwm_task(void)
//{
//	static uint16_t count = 0;
////	uint8_t i;
//		
//	
//	if(count <= PWM_HZ)
//	{		
//		//ֻ��ĳһ�����������������
//		if(g_led_pwm == count) //����ֵcount���趨ֵled_pwmҪ�󣬹ر�
//		{  //
//			led_pwm_pin_control(0);   //�����
//		}
//		else if(0 == count)  //��0�����
//			led_pwm_pin_control(1);  //�����
//	}
////	else
////	{
////		count = 0;   //һ�����ڽ��������¿�ʼ��һ������
////		return;   //�ո�����Ͳ���ȥ����
////	}
//	count++;
//	if(count>PWM_HZ)
//		count = 0;
//}






#ifdef LEDS_FLASH_TASK
//������˸���� //50ms����һ��
void leds_flash_task(void)
{
	uint8_t i;
		
	for(i=0;i<LEDS_NUM_MAX;i++)
	{
		if(g_leds_flash_time_already[i] == 0)  //��תһ��
		{
		
		}
	}
	
	for(i=0;i<4;i++)
	{
		g_leds_flash_time_already[i]++;
		if(g_leds_flash_time_already[i] >= g_const_led_flash_time[i])
		{
			g_leds_flash_time_already[i] = 0;   //�Ѿ���˸��ʱ��
		}
	}
	
	
//	static uint8_t count = 0;
//	uint8_t i;
//	
//	if(!g_leds_flash_control[0] && !g_leds_flash_control[1])  //û�м�����˸���Ƶ���Ҫ
//		return;
//	
//	count ++;
////	printf("g_leds_flash_control[0] = %#x,g_leds_flash_control[1]= %#x\r\n",g_leds_flash_control[0],g_leds_flash_control[1]);
//	if(count == 1)
//	{
//	//	printf("leds_flash_task g_leds_flash_control = %#x\r\n",(int)g_leds_flash_control);
//		for(i=0;i<LEDS_NUM_MAX;i++)
//			if(i<32){
//				if(g_leds_flash_control[0] & (1<<i))
//				{
//				//	printf("g_leds_flash_control[0] = %#x , i = %d\r\n",g_leds_flash_control[0],i);
//					key_light_leds_control2(i,1); //����
//				}
//			}
//			else if(i<64)
//			{
//				if(g_leds_flash_control[1] & (1<<(i-32)))
//				{
//					key_light_leds_control2(i,1); //����
//				}
//			}
//	}
//	else if(count == 6){
//		for(i=0;i<LEDS_NUM_MAX;i++)
//			if(i<32){
//				if(g_leds_flash_control[0] & (1<<i))
//				{
//				//	printf("g_leds_flash_control[0] = %#x , i = %d\r\n",g_leds_flash_control[0],i);
//					key_light_leds_control2(i,0); //����
//				}
//			}
//			else if(i<64)
//			{
//				if(g_leds_flash_control[1] & (1<<(i-32)))
//				{
//					key_light_leds_control2(i,0); //����
//				}
//			}
//	}
//	else if(count > 10)
//	{
//		count = 0;
//	}
}
#endif


//-----------------------------------------------------------------------------------
//led��pwm����

//��ʱ���������߹ر� 1Ϊ������0Ϊ�ر�
static void Pwm_Timer_Control(uint8_t enable)
{
	if(enable)
	{
		//������ʱ��1
		timer_enable(TIMER1);
	}
	else
		timer_disable(TIMER1);
}


//�԰�������� led_PWM ���ŵĿ���
static void led_pwm_pin_control(uint8_t status)
{
	if(status)
		gpio_bit_set(GPIOE, GPIO_PIN_9);   //����ߵ�ƽ
	else
		gpio_bit_reset(GPIOE, GPIO_PIN_9);  //����͵�ƽ
}



//����led������ [0-100]
void set_Led_Pwm(uint8_t pwm)
{
	if(pwm > 100)
		pwm = 100;
	
	g_led_pwm = pwm/5;  //������0-20

	if(pwm == 100) //���ÿ���ʱ��
	{
		Pwm_Timer_Control(RESET);  //��ʱ���ر�
		led_pwm_pin_control(SET);  //���������
	}
	else if(pwm == 0) //���ÿ���ʱ��
	{
		Pwm_Timer_Control(RESET);  //��ʱ���ر�
		led_pwm_pin_control(RESET);  //���������
	}
	else //������ʱ��
	{
		Pwm_Timer_Control(SET);  //��ʱ������
	}
	
	MY_PRINTF("set_Led_Pwm  g_led_pwm = %d\r\n",pwm);
}







//2022-09-16 ����timer1 �����ж��źţ����޸�pwm�����ֵ
//ֻ�����ڼ�ʱ����ʹ��tim1�ˣ�
static void TIM1_Led_Pwm_Init(uint16_t arr,uint16_t psc)
{
	timer_parameter_struct initpara;
	//���ղ���
	rcu_periph_clock_enable(RCU_TIMER1);  //��ʱ��ģ��ʱ��ʹ��

	//3. ��ʼ����ʱ�������ݽṹ  /* initialize TIMER init parameter struct */
	timer_struct_para_init(&initpara);
	initpara.period = arr;  //���ص����֣�
	initpara.prescaler = psc;  //Ԥ��Ƶ�����õ���1Mhz����
	//4. ��ʼ����ʱ��      /* initialize TIMER counter */
	timer_init(TIMER1, &initpara);

	nvic_irq_enable(TIMER1_IRQn, 7U, 0U);  //����ֻ����ռ���ȼ��ˡ�
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);   //��ʱ�ж�	
}


//��ʱ��1���жϴ�����Ŀǰ��pwm
void TIMER1_IRQHandler(void)
{
	static uint8_t count = 0;
	
	if(timer_interrupt_flag_get(TIMER1,TIMER_INT_FLAG_UP)!=RESET)
	{		
		count++;  //1ms �ѹ�ȥ			
		if(count <= LEDS_PWM_HZ)
		{
			//ֻ��ĳһ�����������������
			if(g_led_pwm == count) //����ֵcount���趨ֵled_pwmҪ�󣬹ر�
			{  //
				led_pwm_pin_control(0);   //�����
			}
			else if(1 == count)  //��0�����
				led_pwm_pin_control(1);  //�����
			}
		else 
		{
			count = 0;   //һ�����ڽ��������¿�ʼ��һ������
		//	return;   //�ո�����Ͳ���ȥ����
		}				
	}
	timer_interrupt_flag_clear(TIMER1,TIMER_INT_FLAG_UP);	
}


