
#include "includes.h"

/* ����Ľ��ղ��� */
//PC9  ir_out �����������   TIMER7_CH3
//����timer7 ������ĳЩ��Ƭ�������ã������ʱ��ʱ����ΪTIMER3��2022-08-23

//uint16_t pin;
//GPIO_TypeDef *gpio;

#define IR_DETECT_USE_IRQ   //�������ʹ���ⲿ�ж��źţ�����ʹ�ã�


static ir_nec_state ir_nec_current_state;
//uint8_t cnt = 0;

static unsigned int ir_recv_data, ir_recv_prev_data;
static uint16_t ir_nec_pulse_lengths[6];
static bool ir_nec_repeat_last_command = FALSE;


//���ݽṹ��ʼ��
static void ir_nec_init(void) {
//	pin = p;
//	gpio = g;
	ir_nec_current_state = IR_NEC_NONE;

	ir_nec_pulse_lengths[IR_NEC_NONE] = 0;
	ir_nec_pulse_lengths[IR_NEC_FIRST_BURST] = 9000;    //9ms
	ir_nec_pulse_lengths[IR_NEC_SECOND_BURST] = 4500;   //4.5ms
	ir_nec_pulse_lengths[IR_NEC_1] = 560;               //560us
	ir_nec_pulse_lengths[IR_NEC_NDEF] = 1;				//1us
	ir_nec_pulse_lengths[IR_NEC_SECOND_BURST_REPEAT] = 2250;  //2.25ms
}

static void ir_print_data(unsigned int p_data) {
	uint8_t dev_id;
//	static uint8_t dev_id2;
	uint8_t cmd_id;
	uint8_t cmd_id2;

	dev_id =  (p_data & 0xFF000000) >> 24;
//	dev_id2 = (p_data & 0x00FF0000) >> 16;
	cmd_id =  (p_data & 0x0000FF00) >> 8;
	cmd_id2 = (p_data & 0x000000FF);

	if (cmd_id + cmd_id2 == 0xFF) {
		printf("%u %u\r\n", dev_id, cmd_id);
	}
}

static void ir_nec_reset_transmission() {
	
	if (ir_nec_current_state != IR_NEC_NONE) {
		ir_nec_current_state = IR_NEC_NONE;

		if (ir_nec_repeat_last_command) {
			ir_print_data(ir_recv_prev_data);
		} else {
			ir_print_data(ir_recv_data);
			ir_recv_prev_data = ir_recv_data;
		}

		ir_recv_data = 0;
//		cnt = 0;
		ir_nec_repeat_last_command = FALSE;
	}
}



//ʶ�������ʱ��
static bool ir_nec_check_tolerance(unsigned int received, ir_nec_state current_state) {
	unsigned int expected = ir_nec_pulse_lengths[current_state];

	if (current_state == IR_NEC_NONE || current_state == IR_NEC_NDEF) {  //IR_NEC_NDEF ���ݽ׶Σ����ж�ʱ�䣬ֱ�ӷ���true
		return TRUE;
	} else if (current_state == IR_NEC_SECOND_BURST) {
		// We can receive long (4.5 ms) or short (2.25 ms) burst.
		if (received < 3000) {
			ir_nec_current_state = IR_NEC_SECOND_BURST_REPEAT;
			// ^ nasty hack, but we are can determine type of burst after receiving
			expected = ir_nec_pulse_lengths[IR_NEC_SECOND_BURST_REPEAT];
		} else {
			expected = ir_nec_pulse_lengths[IR_NEC_SECOND_BURST];
		}
	}

	uint32_t min = expected - (TOLERANCE / 100.0) * expected;   //�ȱ�׼ֵС20%
	uint32_t max = expected + (TOLERANCE / 100.0) * expected;   //�ȱ�׼ֵ��20%
	if ((received >= min) && (received <= max)) {
		return TRUE;
	}
	return FALSE;
}

static void ir_nec_state_machine(unsigned int time) {
//	BitAction bit = 1 - GPIO_ReadInputDataBit(gpio, pin); // Invert received value
//	GPIO_WriteBit(GPIOC, GPIO_Pin_8, bit);
	static uint8_t cnt = 0;
//	printf("timer = %d\r\n",time);
	
	if (!ir_nec_check_tolerance(time, ir_nec_current_state)) {
		ir_nec_reset_transmission();
		return;
	}

	switch (ir_nec_current_state) {
		case IR_NEC_NONE:
			ir_nec_current_state = IR_NEC_FIRST_BURST;
			cnt = 0;
//			printf("0\r\n");
			break;
		case IR_NEC_FIRST_BURST:
			ir_nec_current_state = IR_NEC_SECOND_BURST;
//			printf("1\r\n");
			break;
		case IR_NEC_SECOND_BURST:
			ir_nec_current_state = IR_NEC_1;
//			printf("2\r\n");
			break;
		case IR_NEC_1:
			ir_nec_current_state = IR_NEC_NDEF; // we can receive either 0 or 1
//			printf("3\r\n");
			break;
		case IR_NEC_NDEF:
//			printf("4\r\n");
			ir_nec_current_state = IR_NEC_1;
			ir_recv_data >>= 1;   //�ʼ���͵������λ
			//data = data << 1;
			if (time > 1000) {  //�ߵ�ƽ��ʱ�����1.9ms ���͵�ƽ��0.5ms(����ܷ����ز��ź�ʱ���ߵ�ƽ�������Ǻ����յı����ǵͣ��Է������ǵͣ�������ձ���Ϊ��)
				ir_recv_data |= 0x80000000;   //���λ��1 
			//	data = data | 1;
			} 
//			else {
//				data = data << 1 | 0;
//			}
			++cnt;
//			if (cnt > 30)
//			{
//				printf("cnt = %d data = %#x\r\n",cnt,ir_recv_data);
//			}
			if (cnt >= 32) { //all data received
				printf("cnt = %d data = %#x\r\n",cnt,ir_recv_data);
				cnt = 0;
				ir_recv_prev_data = ir_recv_data;
			}
			break;
		case IR_NEC_SECOND_BURST_REPEAT:
			// repeat last message
//			ir_nec_repeat_last_command = TRUE;
			ir_nec_current_state = IR_NEC_1;
			break;
		default:
			ir_nec_reset_transmission();
			break;
	}
}





//����������ţ�����Ϊ�жϷ�ʽ��
static void ir_detect_pin_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);	
	rcu_periph_clock_enable(RCU_AF);   //���ǵ�����ֲ�ԣ���Ҫ���	
	gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, GPIO_PIN_9);	  //PC9����Ϊ����ģʽ

//����Ϊ�жϷ�ʽ��
#ifdef IR_DETECT_USE_IRQ
	// �������ȼ�
    nvic_irq_enable(EXTI5_9_IRQn, 2U, 2U);
    
    // ����EXTI����Դ
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_9);

    // �½����ж�
    exti_init(EXTI_9, EXTI_INTERRUPT, EXTI_TRIG_BOTH); //˫��Ե����
    // ���жϱ�־
    exti_interrupt_flag_clear(EXTI_9);
	exti_interrupt_enable(EXTI_9);
#endif	

}







//void IR_Init(void) {
//	/* Enable GPIO clock */
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

//	// Use PC6 as input from IR receiver
//	GPIO_InitTypeDef GPIO_InitStructure;

//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);

//	// Enable clock and its interrupts
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);

//    NVIC_InitTypeDef NVIC_InitStructure;
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

//	EXTI_InitTypeDef EXTI_InitStructure;
//    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXTI_InitStructure);


//}

//ֻ�����ڼ�ʱ���Ͳ�ʹ�ö�ʱ��7�ˣ���������ʹ��tim4�ˣ��ܿ�TIMER3
static void TIM3_IR_RECIVER_Init(uint16_t arr,uint16_t psc)
{
//	timer_ic_parameter_struct icpara;
	timer_parameter_struct initpara;
	//���ղ���
	rcu_periph_clock_enable(RCU_TIMER3);  //��ʱ��ģ��ʱ��ʹ��

	//3. ��ʼ����ʱ�������ݽṹ  /* initialize TIMER init parameter struct */
	timer_struct_para_init(&initpara);
	initpara.period = arr;  //���ص����֣�
	initpara.prescaler = psc;  //Ԥ��Ƶ�����õ���1Mhz����
	//4. ��ʼ����ʱ��      /* initialize TIMER counter */
	timer_init(TIMER3, &initpara);

	//5.���벶���ʼ��
//	timer_channel_input_struct_para_init(&icpara);
//	icpara.icfilter    = 3U;
//	timer_input_capture_config(TIMER7, TIMER_CH_3, &icpara);

//	nvic_irq_enable(TIMER7_Channel_IRQn, 1U, 1U);
//	timer_interrupt_enable(TIMER7, TIMER_INT_CH3);
	nvic_irq_enable(TIMER3_IRQn, 1U, 1U);
	timer_interrupt_enable(TIMER3, TIMER_INT_UP);

	//������ʱ��2
	timer_enable(TIMER3);
}


//��ʼ�� ir���ղ��ֳ�ʼ��
void IR_Recv_Init(void)
{
	ir_detect_pin_init();   //���ų�ʼ��
	ir_nec_init();    //nec���ݽṹ��ʼ��
	TIM3_IR_RECIVER_Init(10000-1,(SystemCoreClock/1000000)-1);   //10ms��ʱ   1MHz == 1us
}




//void TIM_Init(void) {
//	// Enable clock and its interrupts
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

//    NVIC_InitTypeDef NVIC_InitStructure;
//    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

//	TIM_TimeBaseInitTypeDef TIM_InitStructure;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
//	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_InitStructure.TIM_Prescaler = 24 - 1;
//	TIM_InitStructure.TIM_Period = 10000 - 1; // Update event every 10000 us / 10 ms
//	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_InitStructure.TIM_RepetitionCounter = 0;
//	TIM_TimeBaseInit(TIM2, &TIM_InitStructure);
//	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//	TIM_Cmd(TIM2, ENABLE);
//}

//int main(void)
//{
//	LED_Init();
//	IR_Init();
//	TIM_Init();
//	USART1_Init(921600);
//	ir_nec_init(GPIO_Pin_6, GPIOC);

//	printf("\r\nWorking\r\n ");

//    while(1)
//    {
//    }
//}


//��ʱ��4���жϴ���Ŀǰ�����ں�����ռ�ʱ
void TIMER3_IRQHandler(void)
{
	if(timer_interrupt_flag_get(TIMER3,TIMER_INT_FLAG_UP)!=RESET)
	{
		ir_nec_reset_transmission();    //��ʱ����ʱ��λ����
	}
	timer_interrupt_flag_clear(TIMER3,TIMER_INT_FLAG_UP);	
}


#ifdef IR_DETECT_USE_IRQ

//static volatile uint8_t ir_detect_time_out = 5;    //�жϽ��ճ�ʱʱ��

void ir_irq9_handle(void)
{
	unsigned int counter;
	// Restart Timer
	counter = timer_counter_read(TIMER3);//TIM_GetCounter(TIM2);
	timer_counter_value_config(TIMER3, 0);//TIM_SetCounter(TIM2, 0);
	ir_nec_state_machine(counter);
//	printf("count= %d\r\n",counter);
}

#endif



//void EXTI9_5_IRQHandler(void)
//{
//    static unsigned int counter;

//    if (EXTI_GetITStatus(EXTI_Line6) != RESET)
//    {
//        uint8_t bit = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6);
//        if (bit) {
//        	GPIO_ResetBits(GPIOC, GPIO_Pin_9);
//        } else {
//        	GPIO_SetBits(GPIOC, GPIO_Pin_9);
//        }

//        // Restart Timer
//        counter = TIM_GetCounter(TIM2);
//        TIM_SetCounter(TIM2, 0);
//        ir_nec_state_machine(counter);

//        EXTI_ClearITPendingBit(EXTI_Line6);
//    }
//}



static uint8_t check_ir_recv_data(void)
{
	uint8_t data[4];
	
	
	if(ir_recv_prev_data == 0)
		return 0;
		
	data[3] =  (ir_recv_prev_data & 0xFF000000) >> 24;
	data[2] = (ir_recv_prev_data & 0x00FF0000) >> 16;
	data[1] =  (ir_recv_prev_data & 0x0000FF00) >> 8;
	data[0] = (ir_recv_prev_data & 0x000000FF);
	
	ir_recv_prev_data = 0; //������������
	
	if((ir_Send_DAT[0] == data[0]) &&
		(ir_Send_DAT[1] == data[1]) &&
		(ir_Send_DAT[2] == data[2]) &&
		(ir_Send_DAT[3] == data[3]))
		return 1;   //���͵ĺͽ��յ�������ͬ������1
	else
	{
		printf("ERROR:recv data[0] = %#x,data[1] = %#x,data[2] = %#x,data[3] = %#x\r\n",data[0],data[1],data[2],data[3]);
	}
	return 0;
}



// 500ms ����1��
//ϵͳ�����ż�⣬�������Ͳ��ü���ˣ�����
// ���ⷢ��Ҳ������ġ�
void ir_irq9_detect_task(void)
{
	static uint8_t cn = 0;
//	static uint16_t n = 0;   //δ���ʱ���ʱ
//	static uint16_t count = 0; //���ڼ�ʱ�����ʱ��
//	static uint16_t k = 0;
	
	
	
	if(cn++%2 == 0)
	{
		IR_NEC_Send_Code(ir_Send_DAT, 4);
	}
	else
	{
		if(check_ir_recv_data())  //�յ������ź���
		{
			printf("ok:check_ir_recv_data \r\n");
		}
		else  //û���յ������ź�
		{
			printf("failed:check_ir_recv_data \r\n");
		}
	}
}

