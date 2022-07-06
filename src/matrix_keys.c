

/*
	���󰴼�ɨ��
	
	6*6  ͨ��iic��չ�����ġ�
	

*/




#include "includes.h"



//uint8_t Key_row[1]={0xff};   //����һ�����飬�����ɨ��״̬
		
	//ֻ��6�У���Щ�������ڷֱ�ɨ��ÿһ��
const uint8_t key_scan_line[] = {0xfe,0xfd,0xfb,0xf7,0xef,0xdf};
//const uint8_t key_scan_linecode[] = {1,2,3,4,5,6};   //�б���



#define KEYS_IIC_ADDR (0)  //ֻ��ʾA2A1A0 3�����ŵ�ֵ
#define NCA9555_IIC_CONTROLER  IIC3_INDEX   //��Ӧ�ⲿ�ж�13  2021-12-07

// 6*6 �ļ��̾����ܹ���33������
//static BTN_INFO btn[BTN_MAX];

static uint8_t btn_press_num = 0;  //0��ʾû�а��������£�1-36��ʾ��Ӧ�İ���������
uint8_t btn_start_scan = 0;   //0��ʾû�а��������£�1��ʾ���������ж�





//�˿ڵ�����
void matrix_keys_init(void)
{
	uint8_t outcfg_dat[2]={0,0xff};   //IICоƬGPIO���ģʽ����Ӧ��λҪ����Ϊ0
	//1. iic�ĳ�ʼ��
	nca9555_init(NCA9555_IIC_CONTROLER);
		
	//���󰴼���P0�˿�����Ϊ�����P1�˿�����Ϊ���룬��ΪP1�˿���������������
	nca9555_write_2config(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,outcfg_dat);

	nca9555_write_outport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,0, 0); //P0�˿����0
	
	//key_board cs����PE8������Ҫ����
//	rcu_periph_clock_enable(RCU_GPIOE);			
//	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_8);	
//	gpio_bit_reset(GPIOE, GPIO_PIN_8);

#ifdef 	BTNS_USE_INT   //����btns_leds.h�ж���
	//�ж����ų�ʼ��
	//2. �ж����ŵĳ�ʼ�� PB12���ⲿ�ж�12
	//2.1 ʱ��ʹ��
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_AF);		
	
	gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_12);	
	
	//2.2 ����Ϊ�ⲿ�ж����ţ�
	//GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_12);
	
	//���ô�����ʽ���͵�ƽ����
	exti_init(EXTI_12, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_interrupt_enable(EXTI_12);
	exti_interrupt_flag_clear(EXTI_12);
	//2.3 nvic�����ж�
	//�жϿ�����ʹ�ܣ�ʹ�õ����ⲿ�ж�12
	nvic_irq_enable(EXTI10_15_IRQn,  1, 2);   //�����жϣ����������ȼ�

	//��ʼ��֮���ȡһ��
	matrix_keys_row_scan();	

#endif		
	
	
}


#ifdef 	BTNS_USE_INT
//�ⲿ�ж�12�Ĵ�����,�������º��ɿ����ᴥ���жϣ�������
void exint12_handle(void)
{
	btn_start_scan = 1;
	//���ҽ�ֹ�ж�
	exti_interrupt_disable(EXTI_12);   //ɨ�����֮����ʹ��
}

#endif





uint8_t matrix_keys_row_scan(void)
{
	uint8_t key_row_dat;
	
	if(nca9555_read_inport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,1,&key_row_dat) == 0)  //��ʾ��ȡ�ɹ�
	{
		if((key_row_dat&0x3f) != 0x3f)   //ֻ�жϵ�6λ������ȱ�ʾ�а�������
		{
			return key_row_dat&0x3f;
		}
		else
		{
			return 0x3f;
		//	printf("ERROR: KEY_ROW_SCAN key_row_dat == 0x3f\n\r");
		}
	}
	else //iic��ȡʧ��
	{
		printf("ERROR: KEY_ROW_SCAN nca9555_read_inport\n\r");
		return 0xff;
	}
}






/***
 *��������KEY_SCAN
 *��  �ܣ�6*6����ɨ��
 *����ֵ��1~36����Ӧ36������,0��ʾû�м�⵽
 */
char matrix_keys_scan(void)
{    
    uint8_t Key_Num=0;            //1-16��Ӧ�İ�����
    uint8_t key_row_num=0;        //��ɨ������¼
    uint8_t i;
	
	key_row_num = matrix_keys_row_scan();
	if(key_row_num < 0x3f)   //��ȡ����һ����Ч�İ�������
	{
		for(i=0;i<6;i++)  //ÿһ��ɨ��һ��
		{
			if(nca9555_write_outport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,0, key_scan_line[i])) //P0�˿����0
			{
				printf("ERROR: KEY_ROW_SCAN nca9555_write_outport i=%d\n\r",i);
				continue;  //д��ʧ�ܣ�ֱ����������
				//	return -1;
			}
			//�ٴζ�ȡ����
			key_row_num = matrix_keys_row_scan();
			
			if(key_row_num >= 0x3f)  //����ֵ��ʾû�а������»����
				continue;  //����
			
			switch(key_row_num)
			{
				case 0x3e:
					Key_Num = i+1;
					break;
				case 0x3d:
					Key_Num = 6+i+1;
					break;
				case 0x3b:
					Key_Num = 12+i+1;
					break;
				case 0x37:
					Key_Num = 18+i+1;
					break;
				case 0x2f:
					Key_Num = 24+i+1;
					break;
				case 0x1f:	
					Key_Num = 30+i+1;
					break;
				default:
					Key_Num = 0;
					continue;  //����
			}
			//��ȡ���˰���ֵ			
			if(Key_Num > 0)
			{
				break;
			}
		}
	}

	nca9555_write_outport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,0, 0);	
	return Key_Num;
}






/*
	main��������ɨ������30msһ��,��ѯ��ʽ

	btn_start_scan ���жϺ������ã����º��ɿ���������Ϊ��0
	����������
*/
void task_matrix_keys_scan(void)
{
	static uint8_t scan_num = 0;   //ɨ���������������
	uint8_t ret;

#ifndef BTNS_USE_INT	
	if(btn_start_scan == 0)
		btn_start_scan =1;   //��ʼɨ��
#endif	
	if(btn_start_scan) //�ⲿ�����º��ɿ����ᴥ�����жϴ����󣬲�Ϊ0.
	{		
		ret = matrix_keys_scan();		
		if(ret) //�а���������
		{	
			if(btn_press_num != ret)   //��û�м�¼�ð���ֵ
			{
				scan_num++;
				if(scan_num>1)  //����ɨ�赽���Σ���������
				{					
					btn_press_num = ret;
					
					//���������λ���ϱ�����״̬
					printf("task1_btn_scan ret = %d\n\r",ret);					
				}
			}
		}
		else  //�������ɿ�
		{
			btn_start_scan = 0;
			if(btn_press_num)
			{
				btn_press_num = 0;	  //�ɿ��󣬰���ֵΪ0
				scan_num = 0;	 //ɨ���������		
				printf("release\n\r");
			}
			
#ifdef 	BTNS_USE_INT	
			exti_interrupt_enable(EXTI_12);   //ɨ�����֮����ʹ��		
#endif
			//printf("release\n\r");
		}//if(ret)		
	}
	//else
		;//scan_num = 0;     //����ʡ�԰ɣ���
}
//#endif

