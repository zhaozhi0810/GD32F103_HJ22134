




#include "includes.h"


const char* g_build_time_str = "Buildtime :"__DATE__" "__TIME__;   //获得编译时间
uint16_t g_task_id;   //每一个位对应一个任务，为1表示需要启动任务，在任务中清零该位






////800ms 看门狗
//static void iwdog_init(void)
//{
//	fwdgt_write_enable();
//	fwdgt_config(0xfff,FWDGT_PSC_DIV8);    //设置分配系数,最长819ms
//	
//	fwdgt_enable(); //使能看门狗
//}


//static  void iwdog_feed(void)
//{
////	if(mcu_reboot)  //设置mcu重启，不喂狗。2021-12-17增加
////		return ;
//	fwdgt_counter_reload();
//}




static void BoardInit(void)
{
	//0. 中断分组初始化
	//NVIC_SetPriorityGrouping(4);  //均为4个等级
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	
	//0.1 复用功能模块通电
    rcu_periph_clock_enable(RCU_AF);
	
	//只保留sw接口，其他用于GPIO端口
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
	//1.串口初始化
	//#define DEBUG_COM_NUM 0   //调试串口号
	//#define TOCPU_COM_NUM 1   //与cpu通信的串口
	gd_eval_com_init(DEBUG_COM_NUM);  //用于调试
	gd_eval_com_init(TOCPU_COM_NUM);  //用于与cpu数据通信,改到cpu上电后再初始化
	
	//2.systick 初始化
	SystickConfig();
	
	//3.网讯1860的电源1.1v使能控制
	Wxen_Control_Init();
	
	//3.LT9211 mcu控制端引脚初始化，并控制LT9211复位
	LT9211_Mcu_ControlPort_Init();
	LT9211_Reset();
	
//	LcdHeat_Init();  //没有加热功能时，也要保证初始化后引脚为低
#ifdef LCD_HEAT_ENABLE		
	//4.18B20初始化
	DS18B20_Init(ds18b20_ind1);
	DS18B20_Init(ds18b20_ind2);
#endif	
	//5. 矩阵按键扫描初始化
	matrix_keys_init();
	
	//6. cpu运行状态监控引脚初始化
//	Cpu_Run_Pins_Init();
	
	//7.散热风扇初始化
//	Fan_Control_Init();
	
	//8. 4路di输入的引脚配置
//	Di_4Ttl_Pins_Init();
	
	//9. lcd控制引脚初始化
#ifdef LCD_PWM
	lcd_pwm_init(70);    //亮度默认为70% ，此时显示屏不开启！！！！
#endif		
	//10. 初始化外部硬件看门狗，默认不开启
//	Hard_WatchDog_Pins_Init();
	
	// 11.led初始化
	Led_Show_Work_init();
	key_light_leds_init();
	
	//12. 电压电流，温度，iic的初始化
//	Vol_Temp_Api_Init();
		
	//13. 销毁
//	Msata_Destroy_Pin_Init();   //默认输出高
			
	//14. 启动系统，默认上电时是启动系统的。
//	Enable_Cpu_Poweron();


	key_light_allleds_control(SET);
//	Delay1ms(5000);
	// LT9211 开始工作
	LT9211_Config();
		
	//15. 启动单片机内部看门狗
//	iwdog_init();
//	Delay1ms(5);
	Wxen_Control_Enable();    //1.1v wx1860使能

	key_light_allleds_control(RESET);
}





int main(void)
{
	uint8_t i;
	const task_t task[TASK_MAX]={0//Btn_Power_Change_Scan    //任务1，上电按钮扫描								
							,[1] = task_matrix_keys_scan       		//任务2，无
						//	,[2] = Task_Check_CPU_Run_Status    //任务3，运行状态检测，关机重启控制，这个优先级可以低一点
						//	,[3] = Task_Get_Temp_Vol       //任务4，温湿度，电压监控读取任务，2000ms调用一次
					//		,[4] = Task_ReportTo_Cpu_status  //任务5，定时向cpu汇报，500ms一次 //2022-04-21不再主动发送	
					//		,[5] = com3_frame_handle    //无需要
						//	,[14] = iwdog_feed         //最后一个任务喂狗
					//	,0
						,[15]=Task_Led_Show_Work       //任务16，最后一个任务，让工作led灯闪烁,1s调用一次
					//因为工作灯不能正常使用，所以删除该任务。2021-12-01
	};
	
	
	//1. 初始化
	BoardInit();

	printf("%s\n\r", g_build_time_str);
	printf("BoardInit done! 2022-07-01\n\r");
	
//	Delay1ms(2000);
//	for(i=0;i<32;i++)
//	{
//		printf("i = %d\n\r", i);
//		key_light_leds_control(i,SET);
//		Delay1ms(2000);
//	}

	
	while(1)
	{
		for(i=0;i<TASK_MAX && g_task_id;i++){
			if(g_task_id & (1<<i))   //定时时间到，要执行
			{
				g_task_id &= ~(1<<i);  //对应的位置被清零，等待定时器设置
			
				if(task[i])  //指针不能为空
				{	
					task[i](); //执行该任务
					break;    //一次只执行一个任务，任务靠前的优先级高，任务靠后的优先级低
				}				
			}
		}//end for		
	}
}


