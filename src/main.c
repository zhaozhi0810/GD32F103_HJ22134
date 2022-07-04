




#include "includes.h"


const char* g_build_time_str = "Buildtime :"__DATE__" "__TIME__;   //��ñ���ʱ��
uint16_t g_task_id;   //ÿһ��λ��Ӧһ������Ϊ1��ʾ��Ҫ���������������������λ






////800ms ���Ź�
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
	Wxen_Control_Init();
	
	//3.LT9211 mcu���ƶ����ų�ʼ����������LT9211��λ
	LT9211_Mcu_ControlPort_Init();
	LT9211_Reset();
	
//	LcdHeat_Init();  //û�м��ȹ���ʱ��ҲҪ��֤��ʼ��������Ϊ��
#ifdef LCD_HEAT_ENABLE		
	//4.18B20��ʼ��
	DS18B20_Init(ds18b20_ind1);
	DS18B20_Init(ds18b20_ind2);
#endif	
	//5. ���󰴼�ɨ���ʼ��
	matrix_keys_init();
	
	//6. cpu����״̬������ų�ʼ��
//	Cpu_Run_Pins_Init();
	
	//7.ɢ�ȷ��ȳ�ʼ��
//	Fan_Control_Init();
	
	//8. 4·di�������������
//	Di_4Ttl_Pins_Init();
	
	//9. lcd�������ų�ʼ��
#ifdef LCD_PWM
	lcd_pwm_init(70);    //����Ĭ��Ϊ70% ����ʱ��ʾ����������������
#endif		
	//10. ��ʼ���ⲿӲ�����Ź���Ĭ�ϲ�����
//	Hard_WatchDog_Pins_Init();
	
	// 11.led��ʼ��
	Led_Show_Work_init();
	
	//12. ��ѹ�������¶ȣ�iic�ĳ�ʼ��
//	Vol_Temp_Api_Init();
		
	//13. ����
//	Msata_Destroy_Pin_Init();   //Ĭ�������
			
	//14. ����ϵͳ��Ĭ���ϵ�ʱ������ϵͳ�ġ�
//	Enable_Cpu_Poweron();
	
	// LT9211 ��ʼ����
//	LT9211_Config();
	
	
	//15. ������Ƭ���ڲ����Ź�
//	iwdog_init();
	Delay1ms(5);
	Wxen_Control_Enable();    //1.1v wx1860ʹ��
}





int main(void)
{
	uint8_t i;
	const task_t task[TASK_MAX]={0//Btn_Power_Change_Scan    //����1���ϵ簴ťɨ��								
							,[1] = task_matrix_keys_scan       		//����2����
						//	,[2] = Task_Check_CPU_Run_Status    //����3������״̬��⣬�ػ��������ƣ�������ȼ����Ե�һ��
						//	,[3] = Task_Get_Temp_Vol       //����4����ʪ�ȣ���ѹ��ض�ȡ����2000ms����һ��
					//		,[4] = Task_ReportTo_Cpu_status  //����5����ʱ��cpu�㱨��500msһ�� //2022-04-21������������	
					//		,[5] = com3_frame_handle    //����Ҫ
						//	,[14] = iwdog_feed         //���һ������ι��
					//	,0
						,[15]=Task_Led_Show_Work       //����16�����һ�������ù���led����˸,1s����һ��
					//��Ϊ�����Ʋ�������ʹ�ã�����ɾ��������2021-12-01
	};
	
	
	//1. ��ʼ��
	BoardInit();

	printf("%s\n\r", g_build_time_str);
	printf("BoardInit done! 2022-07-01\n\r");
	
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
	}
}

