

#include "includes.h"

/*
	PC9  ir_out ������ն�   timer2ch2 ȫӳ��  TIMER7_CH3
	PC8  ���ⷢ���  		timer2ch3 ȫӳ��  TIMER7_CH2


	���ݺ����ԭ����ֻҪ����38kHZ���źţ�����ͷ�ͻ�Ĭ��Ϊ���յ��ߵ�ƽ���������ܺ�����͵�ƽ����

	��������Ϊ�͵�ƽ��������Ϊ�յ��˺��ⷢ����źţ�û�յ���Ϊ�ߵ�ƽ

	PC8 ����Ϊpwm 38KHZ(ռ�ձ�50%)����������  ��Ϊ��ʡ�����100ms����һ�Σ�ʹ�ö�ʱ��7
	PC9 ���룬����Ч����ʾ���յ����źţ���ʾδ�������������Ч����ʾû���յ��źţ���ʾ�Ѿ������
*/


#define TIMER2_FULL_MAP


#define PWM_PIN GPIO_PIN_8
#define PWM_PORT GPIOC
#define PWM_PORT_RCU RCU_GPIOC
#ifdef TIMER2_FULL_MAP
#define PWM_TIMER_RCU  RCU_TIMER2    //
#define PWM_TIMER  TIMER2
#else
#define PWM_TIMER_RCU  RCU_TIMER7    // ��Щ32û��timer7������Ҫע��
#define PWM_TIMER  TIMER7
#endif
#define PWM_TIMER_CH TIMER_CH_2

static uint16_t IR_PWM_DEGREE_MAX = (uint16_t)(1000/38);   //PWMƵ��  26Ϊ38KHZ   1us�Ƹ�����4000�����Ƶ�ʾ���1000000/4000=250Hz 



void ir_pwm_init(void)
{
	uint16_t degree = 100;    //ռ�ձ�50%
	//PB15 ͨ��
	timer_parameter_struct initpara;
	timer_oc_parameter_struct ocpara;
	
	//1. ��ʱ��ʱ��ʹ��
	rcu_periph_clock_enable(PWM_TIMER_RCU );  //��ʱ��ģ��ʱ��ʹ��
	rcu_periph_clock_enable(PWM_PORT_RCU); 
	rcu_periph_clock_enable(RCU_AF); 
	
	//2. io�������ø��ù���	
	gpio_init(PWM_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, PWM_PIN);   //���ù���	
#ifdef TIMER2_FULL_MAP
	//û��timer7��ʱ����Ҫʹ��ȫӳ��
	gpio_pin_remap_config(GPIO_TIMER2_FULL_REMAP, ENABLE);    //timer2ȫӳ��
#endif	
	

	
	if(degree > 100)
	{
		degree = 100;
	}
	
	//3. ��ʼ����ʱ�������ݽṹ  /* initialize TIMER init parameter struct */
	timer_struct_para_init(&initpara);
	initpara.period = IR_PWM_DEGREE_MAX-1;  //���ص����֣�Ƶ��38kHZ
	initpara.prescaler = (SystemCoreClock/1000000)-1;  //Ԥ��Ƶ�����õ���1Mhz������  
		
	//4. ��ʼ����ʱ��      /* initialize TIMER counter */
	timer_init(PWM_TIMER, &initpara);
		
	//5. ��ʼ����ʱ��ͨ�������ݽṹ /* initialize TIMER channel output parameter struct */
	timer_channel_output_struct_para_init(&ocpara);	
#ifndef PWM_TIMER_USE_CHN
	ocpara.outputstate  = TIMER_CCX_ENABLE;  //���ͨ��ʹ��	
#else
    ocpara.outputnstate = TIMER_CCXN_ENABLE;//ʹ�ܻ���ͨ�����
#endif
	//6. ��ʼ����ʱ��ͨ��   /* configure TIMER channel output function */
	timer_channel_output_config(PWM_TIMER, PWM_TIMER_CH, &ocpara);
			
	//7. ��ʼ����ʱ��ͨ�������ʽ����   /* configure TIMER channel output compare mode */
	timer_channel_output_mode_config(PWM_TIMER, PWM_TIMER_CH, TIMER_OC_MODE_PWM1);
	/* configure TIMER channel output pulse value */
	
	//8. ��ʼ����ʱ��ͨ�����������
	timer_channel_output_pulse_value_config(PWM_TIMER, PWM_TIMER_CH, (100-degree) * IR_PWM_DEGREE_MAX/100);

	//9. ��ʼ����ʱ��ͨ�����ʹ��
	//timer_channel_output_fast_config(TIMER2, TIMER_CH_0, TIMER_OC_FAST_ENABLE);
	timer_channel_output_shadow_config(PWM_TIMER, PWM_TIMER_CH, TIMER_OC_SHADOW_DISABLE);	  //stm32�ƺ��õ������0x8
	//10.��ʼ������ʱ����ʹ�� 2022-04-18	
	
	
	/* enable a TIMER */
	if(PWM_TIMER == TIMER0 || PWM_TIMER == TIMER7)
		timer_primary_output_config(PWM_TIMER, ENABLE);
	timer_auto_reload_shadow_enable(PWM_TIMER);
	
//	timer_enable(PWM_TIMER);   //������ʱ��}
}



//������ʱ������38KhzƵ�ʣ���ʱ������������Ϊ1�������ƽΪ0
void ir_send_high(void)
{
	gpio_init(PWM_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, PWM_PIN);
	timer_enable(PWM_TIMER);   //������ʱ��}
}

//�رն�ʱ�����ٷ���38KhzƵ�ʣ���ʱ������������Ϊ0�������ƽΪ1
void ir_send_low(void)
{
	timer_disable(PWM_TIMER);   //�رն�ʱ��}
	gpio_init(PWM_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, PWM_PIN);
	gpio_bit_reset(PWM_PORT, PWM_PIN);
}




//����������
__STATIC_INLINE void IR_NEC_Send_Guide(void)
{
    ir_send_high();   //������ʱ��}
    Delay1us(9000); // 9ms��
    ir_send_low();
    Delay1us(4500); // 4.5ms��
    //��13.5ms
}


//��������0
__STATIC_INLINE void IR_NEC_Send_0(void)
{
    ir_send_high();   //������ʱ��}
    Delay1us(560); // 0.56ms��
    ir_send_low();
    Delay1us(560); // 0.56ms��
    //��1.12ms
}


//��������1
__STATIC_INLINE void IR_NEC_Send_1(void)
{
    ir_send_high();   //������ʱ��}
    Delay1us(560); // 0.56ms��
    ir_send_low();
    Delay1us(1690); // 1.69ms��
    //��2.25ms
}

//����λ
void IR_NEC_Send_End(void)
{
    ir_send_high();
    Delay1us(650); // 6ms��
    ir_send_low();
}



// NEC���뷢��
void IR_NEC_Send_Code(const uint8_t *Dat, uint8_t Len)
{
    uint8_t zj;
	uint8_t j;
    IR_NEC_Send_Guide();          //������
    for (j = 0; j < Len; j++) //ѭ������
    {
        zj = Dat[j];
        for (int i = 0; i < 8; i++) //�Ӹߵ���
        {
            if (zj & (1))   //�ȷ������λ
            {
                IR_NEC_Send_1();
            }
            else
            {
                IR_NEC_Send_0();
            }
            zj >>= 1;  //�ȷ������λ
        }
    }
    IR_NEC_Send_End(); //����λ
//    Delay1ms(40);
}


//nec ir���͵�������4���ֽڣ�32bits����ַ����ַ���룬���ݣ����ݷ��루���ж��ǵ�λ�ȷ���
const  uint8_t ir_Send_DAT[5] = {0X10, 0X68, 0X80, 0X03, 0X0};  //ir���͵�����

//void ir_send_code(void)
//{	
//	IR_NEC_Send_Code(DAT, 4);
//}


