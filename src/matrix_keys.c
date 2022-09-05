

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

// 6*6 �ļ��̾����ܹ���33������������������h�ļ��ж���
static BTN_INFO g_btns_info;

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
	
	memset(&g_btns_info,0,sizeof(g_btns_info));   //��������
	
	
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
		//	printf("ERROR: KEY_ROW_SCAN key_row_dat == 0x3f\r\n");
		}
	}
	else //iic��ȡʧ��
	{
		printf("ERROR: KEY_ROW_SCAN nca9555_read_inport\r\n");
		return 0xff;
	}
}




const uint8_t key_t_code[] = {1,7,13,19,25,31, 
							 2,8,14,20,26,32,
							 3,9,15,21,27,33,
							 4,10,16,22,28,34,
							5,11,17,23,29,35,
							6,12,18,24,30,36
							};

							
							
#if 0							
/***
 *��������KEY_SCAN
 *��  �ܣ�6*6����ɨ��
 *����ֵ��1~36����Ӧ36������,0��ʾû�м�⵽
 */
char matrix_keys_scan(void)
{    
    uint8_t Key_Num=0xff;            //1-16��Ӧ�İ�����
    uint8_t key_row_num=0;        //��ɨ������¼
    uint8_t i;
	
	key_row_num = matrix_keys_row_scan();
	if(key_row_num < 0x3f)   //��ȡ����һ����Ч�İ�������
	{
		for(i=0;i<6;i++)  //ÿһ��ɨ��һ��
		{
			if(nca9555_write_outport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,0, key_scan_line[i])) //P0�˿����0
			{
				printf("ERROR: KEY_ROW_SCAN nca9555_write_outport i=%d\r\n",i);
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
					Key_Num = i;
					break;
				case 0x3d:
					Key_Num = 6+i;
					break;
				case 0x3b:
					Key_Num = 12+i;
					break;
				case 0x37:
					Key_Num = 18+i;
					break;
				case 0x2f:
					Key_Num = 24+i;
					break;
				case 0x1f:	
					Key_Num = 30+i;
					break;
				default:
					Key_Num = 37;  //��ʾͬʱ���¶����
					continue;  //����
			}
			//��ȡ���˰���ֵ			
			if(Key_Num > 36)
			{
				break;
			}
		}
	}

	nca9555_write_outport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,0, 0);	
	
	if(Key_Num > 36)
	{
		if(Key_Num == 37)
			return 37;
		return 0;
	}
	return key_t_code[Key_Num];
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
	if(btn_start_scan ) //�ⲿ�����º��ɿ����ᴥ�����жϴ����󣬲�Ϊ0.
	{		
		ret = matrix_keys_scan();		
		if(ret && ret < 37) //�а���������
		{	
			if(btn_press_num == 0)   //��û�м�¼�ð���ֵ
			{
				scan_num++;
				if(scan_num>1)  //����ɨ�赽���Σ���������
				{					
					btn_press_num = ret;
					send_btn_change_to_cpu(btn_press_num,1); //���Ͱ�������
					//���������λ���ϱ�����״̬
					MY_PRINTF("task1_btn_scan ret = %d\r\n",ret);					
				}
			}
		}
		else if(ret==0) //�������ɿ�
		{
			btn_start_scan = 0;
			if(btn_press_num)
			{
				send_btn_change_to_cpu(btn_press_num,0);  //���Ͱ����ɿ�
				btn_press_num = 0;	  //�ɿ��󣬰���ֵΪ0
				scan_num = 0;	 //ɨ���������		
				MY_PRINTF("release\r\n");
			}
			
#ifdef 	BTNS_USE_INT	
			exti_interrupt_enable(EXTI_12);   //ɨ�����֮����ʹ��		
#endif
			//printf("release\r\n");
		}//if(ret)		
	}
	//else
		//;//scan_num = 0;     //����ʡ�԰ɣ���
}
//#endif





#else
/***
 *��������KEY_SCAN
 *��  �ܣ�6*6����ɨ��
 *����ֵ��1~36����Ӧ36������,0��ʾû�м�⵽
 */
char matrix_keys_scan(void)
{    
    uint8_t Key_Num=0xff;            //1-16��Ӧ�İ�����
    uint8_t key_row_num=0;        //��ɨ������¼
    uint8_t i,j;
	uint8_t index;   //
	static uint8_t release_report = 0;  //�ɿ��ϱ���
	
	key_row_num = matrix_keys_row_scan();
	if(key_row_num < 0x3f)   //��ȡ����һ����Ч�İ�������
	{
		for(i=0;i<COL_NUM;i++)  //ÿһ��ɨ��һ��
		{
			if(nca9555_write_outport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,0, key_scan_line[i])) //P0�˿����0
			{
				printf("ERROR: KEY_ROW_SCAN nca9555_write_outport i=%d\r\n",i);
				continue;  //д��ʧ�ܣ�ֱ����������
				//	return -1;
			}
			//�ٴζ�ȡ����
			key_row_num = matrix_keys_row_scan();
			
//			if(key_row_num >= 0x3f)  //����ֵ��ʾû�а������»����
//				continue;  //����
			
//			printf("i = %d ,key_row_num = %#x\r\n",i,key_row_num);
			
			
			for(j=0;j<ROW_NUM;j++)  //ÿһ��ɨ��һ��
			{
				index = 6*i+j;
				if(!((key_row_num>>j)&(1))) //����
				{
					if(g_btns_info.pressCnt[index] < 2)
					{	
						g_btns_info.pressCnt[index]++;
						if(g_btns_info.pressCnt[index] == 2)//��⵽��ֹ1��
						{   //���������ϱ�һ��
							g_btns_info.value[index] = 1;
						//	g_btns_info.reportEn[index] = 1;  //�����ϱ�
							send_btn_change_to_cpu(index+1,1); //���Ͱ�������/�ɿ�
						//	printf("----btn:%d press\r\n",index+1);
							release_report = 1;   //��¼��Ҫ�ͷű�־
						}
					}
				}
				else //�ɿ�
				{
					if(g_btns_info.value[index]) //֮ǰ��״̬�ǰ���
					{
						g_btns_info.value[index] = 0;
					//	g_btns_info.reportEn[index] = 2;   //�ɿ��ϱ�
						send_btn_change_to_cpu(index+1,0); //���Ͱ�������/�ɿ�
					//	printf("++++btn:%d release\r\n",index);
						g_btns_info.pressCnt[index] = 0;
					}
					
					//if(g_btns_info.pressCnt[index])   //�����������
						
				}
			}
			
//			switch(key_row_num)
//			{
//				case 0x3e:
//					Key_Num = i;
//					break;
//				case 0x3d:
//					Key_Num = 6+i;
//					break;
//				case 0x3b:
//					Key_Num = 12+i;
//					break;
//				case 0x37:
//					Key_Num = 18+i;
//					break;
//				case 0x2f:
//					Key_Num = 24+i;
//					break;
//				case 0x1f:	
//					Key_Num = 30+i;
//					break;
//				default:
//					Key_Num = 37;  //��ʾͬʱ���¶����
//					continue;  //����
//			}
//			//��ȡ���˰���ֵ			
//			if(Key_Num > 36)
//			{
//				break;
//			}
		}
	}
	else
	{
		if(release_report)  //��Ҫ�ϱ��ͷ���Ϣ��
		{		
			for(index=0;index<COL_NUM*ROW_NUM;index++)
			{
				if(g_btns_info.value[index]) //֮ǰ��״̬�ǰ���
				{
					g_btns_info.value[index] = 0;
				//	g_btns_info.reportEn[index] = 2;   //�ɿ��ϱ�
					send_btn_change_to_cpu(index+1,0); //���Ͱ�������/�ɿ�
				//	printf("++++btn:%d release\r\n",index);
					g_btns_info.pressCnt[index] = 0;
				}
				
			}
			release_report = 0;
		}
	}

	nca9555_write_outport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,0, 0);	
	
//	if(Key_Num > 36)
//	{
//		if(Key_Num == 37)
//			return 37;
//		return 0;
//	}
//	return key_t_code[Key_Num];
	return 0;
}







/*
	main��������ɨ������30msһ��,��ѯ��ʽ

	btn_start_scan ���жϺ������ã����º��ɿ���������Ϊ��0
	����������
*/
void task_matrix_keys_scan(void)
{
	uint8_t i = 0;   //ɨ���������������
	uint8_t ret;

#ifndef BTNS_USE_INT	
	if(btn_start_scan == 0)
		btn_start_scan =1;   //��ʼɨ��
#endif	
//	if(btn_start_scan ) //�ⲿ�����º��ɿ����ᴥ�����жϴ����󣬲�Ϊ0.
	{	
		matrix_keys_scan();
		
//		for(i=0;i<ROW_NUM*COL_NUM;i++)
//		{
//			if(g_btns_info.reportEn[i])
//			{
//				g_btns_info.reportEn[i] = 0;    //������Ҫ�ϱ���־
//				send_btn_change_to_cpu(i,g_btns_info.value[i]); //���Ͱ�������/�ɿ�
//				printf("++++btn:%d %s\r\n",i,g_btns_info.value[i]==1?"press":"release");
//			}
//		}

#ifdef 	BTNS_USE_INT	
		exti_interrupt_enable(EXTI_12);   //ɨ�����֮����ʹ��		
#endif
//			//printf("release\r\n");
//		}//if(ret)			
//		ret = matrix_keys_scan();		
//		if(ret && ret < 37) //�а���������
//		{	
//			if(btn_press_num == 0)   //��û�м�¼�ð���ֵ
//			{
//				scan_num++;
//				if(scan_num>1)  //����ɨ�赽���Σ���������
//				{					
//					btn_press_num = ret;
//					send_btn_change_to_cpu(btn_press_num,1); //���Ͱ�������
//					//���������λ���ϱ�����״̬
//					MY_PRINTF("task1_btn_scan ret = %d\r\n",ret);					
//				}
//			}
//		}
//		else if(ret==0) //�������ɿ�
//		{
//			btn_start_scan = 0;
//			if(btn_press_num)
//			{
//				send_btn_change_to_cpu(btn_press_num,0);  //���Ͱ����ɿ�
//				btn_press_num = 0;	  //�ɿ��󣬰���ֵΪ0
//				scan_num = 0;	 //ɨ���������		
//				MY_PRINTF("release\r\n");
//			}
//			
//#ifdef 	BTNS_USE_INT	
//			exti_interrupt_enable(EXTI_12);   //ɨ�����֮����ʹ��		
//#endif
//			//printf("release\r\n");
//		}//if(ret)		
	}
	//else
		//;//scan_num = 0;     //����ʡ�԰ɣ���
}


#endif