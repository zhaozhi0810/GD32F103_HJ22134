




#include "includes.h"


const char* g_build_time_str = "Buildtime :"__DATE__" "__TIME__;   //获得编译时间
uint16_t g_task_id;   //每一个位对应一个任务，为1表示需要启动任务，在任务中清零该位






//800ms 看门狗
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
	OePins_Control_Init();   //OE 可以用于屏幕熄灭
	LcdCtrl_Control_Init();  //lcd电源控制，初始化后设置低
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
//	Wxen_Control_Init();
	
	//3.LT9211 mcu控制端引脚初始化，并控制LT9211复位
	LT9211_Mcu_ControlPort_Init();
//	LT9211_Reset();
	// LT9211 开始工作
	LT9211_Config();

	//5. 矩阵按键扫描初始化
	matrix_keys_init();
	
	
	//6. 单片机内部温度采集
	ADC_Init();
	
	lcd_pwm_init(70);    //亮度默认为70% ，此时显示屏不开启！！！！
			
	//10. 初始化外部硬件看门狗，默认不开启
	hard_wtd_pins_init();
	
	// 11.led初始化
	Led_Show_Work_init();
	key_light_leds_init();
	
	//12. 电压电流，温度，iic的初始化
	lcd_reset_control_init();  //PD8 lcd复位引脚控制
		
	//13.//PD6  MicCtl 	
	MicCtl_Control_Init();


	key_light_allleds_control(SET);
//	Delay1ms(5000);
	
		
	//15. 启动单片机内部看门狗
//	iwdog_init();
//	Delay1ms(5);
//	Wxen_Control_Enable();    //1.1v wx1860使能,容易引发linux系统故障

//	LcdCtrl_Enable();   //lcd电源通电
//	Enable_LcdLight();    //对7寸屏的控制信号，背光使能和背光pwm控制,
	//9. lcd控制引脚初始化

	key_light_allleds_control(RESET);  //面板上所有的灯都熄灭
	
	OePins_Output_Hight(3);   //屏幕点亮 通过OE3控制的cpu发出的pwm
		
}





int main(void)
{
	uint8_t i;
	const task_t task[TASK_MAX]={0//Btn_Power_Change_Scan    //任务1，上电按钮扫描								
							,[1] = task_matrix_keys_scan       		//任务2，矩阵键盘扫描
							,[2] = hard_wtd_feed_task    //任务3，硬件看门狗喂狗任务 100ms
							,[3] = Int_Temp_task      //任务4，单片机ADC温度，1000ms调用一次
							,[4] = LT9211_Config  //任务5，lcd复位指令单独,不是定时器控制	
							,[5] = leds_flash_task    //任务6，led键灯闪烁控制任务，50ms一次
						//	,[6] = leds_run_pwm_task
						//	,[14] = iwdog_feed         //最后一个任务喂狗
					//	,0
						,[15]=Task_Led_Show_Work       //任务16，最后一个任务，让工作led灯闪烁,1s调用一次
					//因为工作灯不能正常使用，所以删除该任务。2021-12-01
	};
	
	
	//1. 初始化
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

		//LT9211_Config();  //lcd复位指令单独	
	}
}


