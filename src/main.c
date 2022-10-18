




#include "includes.h"


const char* g_build_time_str = "Buildtime :"__DATE__" "__TIME__;   //��ñ���ʱ��
uint16_t g_task_id;   //ÿһ��λ��Ӧһ������Ϊ1��ʾ��Ҫ���������������������λ






//800ms ���Ź�
//static void iwdog_init(void)
//{
//	fwdgt_write_enable();
//	fwdgt_config(0xfff,FWDGT_PSC_DIV8);    //���÷���ϵ��,�819ms
//	
//	fwdgt_enable(); //ʹ�ܿ��Ź�
//}


//static  void iwdog_feed(void)
//{
////	if(mcu_reboot)  //����mcu��������ι����2021-12-17����
////		return ;
//	fwdgt_counter_reload();
//}




static void BoardInit(void)
{
	OePins_Control_Init();   //OE ����������ĻϨ��
	LcdCtrl_Control_Init();  //lcd��Դ���ƣ���ʼ�������õ�
	//0. �жϷ����ʼ��
	//NVIC_SetPriorityGrouping(4);  //��Ϊ4���ȼ�
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	
	//0.1 ���ù���ģ��ͨ��
    rcu_periph_clock_enable(RCU_AF);
	
	//ֻ����sw�ӿڣ���������GPIO�˿�
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
	//1.���ڳ�ʼ��
	//#define DEBUG_COM_NUM 0   //���Դ��ں�
	//#define TOCPU_COM_NUM 1   //��cpuͨ�ŵĴ���
	gd_eval_com_init(DEBUG_COM_NUM);  //���ڵ���
	gd_eval_com_init(TOCPU_COM_NUM);  //������cpu����ͨ��,�ĵ�cpu�ϵ���ٳ�ʼ��
	
	//2.systick ��ʼ��
	SystickConfig();
	
	//3.��Ѷ1860�ĵ�Դ1.1vʹ�ܿ���
//	Wxen_Control_Init();
	
	//3.LT9211 mcu���ƶ����ų�ʼ����������LT9211��λ
	LT9211_Mcu_ControlPort_Init();
//	LT9211_Reset();
	// LT9211 ��ʼ����
	LT9211_Config();

	//5. ���󰴼�ɨ���ʼ��
	matrix_keys_init();
	
	
	//6. ��Ƭ���ڲ��¶Ȳɼ�
	ADC_Init();
	
	lcd_pwm_init(70);    //����Ĭ��Ϊ70% ����ʱ��ʾ����������������
			
	//10. ��ʼ���ⲿӲ�����Ź���Ĭ�ϲ�����
	hard_wtd_pins_init();
	
	// 11.led��ʼ��
	Led_Show_Work_init();
	key_light_leds_init();
	
	//12. ��ѹ�������¶ȣ�iic�ĳ�ʼ��
	lcd_reset_control_init();  //PD8 lcd��λ���ſ���
		
	//13.//PD6  MicCtl 	
	MicCtl_Control_Init();


	key_light_allleds_control(SET);
//	Delay1ms(5000);
	
		
	//15. ������Ƭ���ڲ����Ź�
//	iwdog_init();
//	Delay1ms(5);
//	Wxen_Control_Enable();    //1.1v wx1860ʹ��,��������linuxϵͳ����

//	LcdCtrl_Enable();   //lcd��Դͨ��
//	Enable_LcdLight();    //��7�����Ŀ����źţ�����ʹ�ܺͱ���pwm����,
	//9. lcd�������ų�ʼ��

	key_light_allleds_control(RESET);  //��������еĵƶ�Ϩ��
	
	OePins_Output_Hight(3);   //��Ļ���� ͨ��OE3���Ƶ�cpu������pwm
		
}





int main(void)
{
	uint8_t i;
	const task_t task[TASK_MAX]={0//Btn_Power_Change_Scan    //����1���ϵ簴ťɨ��								
							,[1] = task_matrix_keys_scan       		//����2���������ɨ��
							,[2] = hard_wtd_feed_task    //����3��Ӳ�����Ź�ι������ 100ms
							,[3] = Int_Temp_task      //����4����Ƭ��ADC�¶ȣ�1000ms����һ��
							,[4] = LT9211_Config  //����5��lcd��λָ���,���Ƕ�ʱ������	
							,[5] = leds_flash_task    //����6��led������˸��������50msһ��
						//	,[6] = leds_run_pwm_task
						//	,[14] = iwdog_feed         //���һ������ι��
					//	,0
						,[15]=Task_Led_Show_Work       //����16�����һ�������ù���led����˸,1s����һ��
					//��Ϊ�����Ʋ�������ʹ�ã�����ɾ��������2021-12-01
	};
	
	
	//1. ��ʼ��
	BoardInit();

	printf("%s\r\n", g_build_time_str);
	printf("BoardInit done! 2022-07-01\r\n");
	
//	Delay1ms(2000);
//	for(i=0;i<32;i++)
//	{
//		printf("i = %d\r\n", i);
//		key_light_leds_control(i,SET);
//		Delay1ms(2000);
//	}

	
	while(1)
	{
		for(i=0;i<TASK_MAX && g_task_id;i++){
			if(g_task_id & (1<<i))   //��ʱʱ�䵽��Ҫִ��
			{
				g_task_id &= ~(1<<i);  //��Ӧ��λ�ñ����㣬�ȴ���ʱ������
			
				if(task[i])  //ָ�벻��Ϊ��
				{	
					task[i](); //ִ�и�����
					break;    //һ��ִֻ��һ����������ǰ�����ȼ��ߣ����񿿺�����ȼ���
				}				
			}
		}//end for	

		//LT9211_Config();  //lcd��λָ���	
	}
}


