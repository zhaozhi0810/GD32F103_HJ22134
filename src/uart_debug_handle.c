
/*
用于处理与cpu之间的串口通信
	串口为GD32的串口1，115200，8N1


调试串口接收的命令：
0.软件编译时间字符串
1.电压电流
2.cpu和主板温度，液晶屏温度，及lcd加热状态，风扇pwm值（io模式下只有0和100）
3.lcd的亮度值，屏的加电引脚状态，pd_n的状态
4.4路di值，4路光通路信息
5.硬件看门狗状态，（信号源，暂无）
6.cpu运行状态。（开机关机，重启，进入pmon，进入系统等）

不能识别的命令也是打印提示和编译时间字符串
*/



#include "includes.h"



//#define RECV_BUF_LEN 64
#if 0
static Queue_UART_STRUCT g_Queue_Debug_Recv;   //接收Debug数据队列，用于接收中断

frame_buf_t g_com_debug_buf={{0},FRAME_LENGHT};    //数据处理缓存
#endif

//static char* g_Cpu_Run_Status_str[] = {
//	"LS3A_POWER_DISABLE",   //确认是断电状态
//	"LS3A_POWEROFF",    //关机，断电
//	"LS3A_REBOOT",    //重启
//	"LS3A_RUNNING",    //进入pmon
//	"LS3A_RUN_OS",      //离开pmon，进入操作系统
//	"LS3A_POWER_ENABLE"     //已经通电，但是没有进入PMON的前一段;
//};

//这个函数用来处理调试串口接收到的简单的调试命令
static void Com_Debug_Message_Handle1(uint8_t buf)
{
//	uint8_t t;
	switch(buf)
	{
		default:   //cmd打印的时候，可能超出了可显示字符的区间
			printf("ERROR: Command Unknow cmd = 0x%x!!!\n\r",buf);   //不能识别的命令
		case '0':
			printf("%s\n\r",g_build_time_str);  //打印编译的时间
		break;
		case '1':
//			printf("p0v95_vol = %d mv,p1v0_vol  = %d mv, p1v2_vol  = %d mv,p12v_vol = %d mv\n\r",g_p0v95_vol,g_p1v0_vol, g_p1v2_vol,g_p12v_vol);
			//打印电流值
		//	printf("g_p0v95_vol = %d mv,g_p1v0_vol  = %d mv, g_p1v2_vol  = %d mv,g_p12v_vol = %d mv\n\r",g_p0v95_vol,g_p1v0_vol, g_p1v2_vol,g_p12v_vol);
			break;
		case '2':
			//打印温度值
//			printf("cpu temp = %d,board temp = %d\n\r",g_cpu_temp>>4,g_board_temp>>4);
			#ifdef LCD_HEAT_ENABLE		
			printf("lcd1 temp = %d,lcd2 temp  = %d\n\r",g_lcd_temp[0]>>4,g_lcd_temp[1]>>4);
			printf("lcd heat %s,fan_pwm = %d\n\r",Get_Lcd_Heat_Status()?"on":"off",g_fan_pwm);
			#endif
			break;
		case '3':
//			printf("lcd Power %s\n\r",Get_Lcd_Power_Status()?"on":"off");  //lcd加电状态
//			printf("lcd Pd_n status %s\n\r",Get_Lcd_PdN_Status()?"on":"off");  //lcd ttl转换的状态
//			printf("lcd light pwm = %d\n\r",g_lcd_pwm);   //lcd的亮度pwm值
			break;
		case '4':
//			t = Get_Di_4Ttl_Status();   //4路开关量输入DI PB12-PB15
//			printf("4Di Di1 %s,Di2 %s,Di3 %s,Di4 %s \n\r",t&1?"on":"off",t&2?"on":"off",t&4?"on":"off",t&8?"on":"off");
//			t = Get_Optica_Switch_Status();  //4路光开关状态
//			printf("4 op switch D2_STATE2 %s ,D2_STATE1 %s,D1_STATE1 %s,D1_STATE2 %s \n\r",
//								t&1?"on":"off",t&2?"on":"off",t&4?"on":"off",t&8?"on":"off");
			break;
		case '5':
			printf("Watch Dog Status = %s\n\r","off");   //暂时没有开启
			break;
		case '6':
//			printf("Cpu Run Status = %s\n\r",g_Cpu_Run_Status_str[g_cpu_run_status-1]);
			break;
	}
}


/*
	串口数据接收中断：
		前提：每一帧都是7个字节。
		队列中保存帧头，有后面的数据和校验和（共7个字节）
*/
void Com_Debug_Rne_Int_Handle(void)
{
	uint8_t dat;

	dat = (uint8_t)usart_data_receive(EVAL_COM0);//(USART3);  
	Com_Debug_Message_Handle1(dat);   //直接处理
//	QueueUARTDataInsert(&g_Queue_Debug_Recv,dat);   //接收的数据存入队列中。
}


/*
	串口收到命令后的处理。串口的空闲中断处理函数调用
		前提： 收到完整的数据包，校验和正确。

	单片机能够收到的命令：
	// 1.设置视频源,没有该功能
	4.设置lcd的pwm（亮度）
	5.关机或重启命令。

*/

#if 0

static void Com_Debug_Message_Handle(uint8_t* buf)
{		
	com_frame_t* pdata = (com_frame_t*)(buf+1);    //+1是跳过帧头，使用结构体初始化
	int8_t t;
	
	switch(pdata->data_type)
    {
        case eMCU_CMD_TYPE:    //cpu发送给单片机的都是cmd！！
            t = pdata->data.cmd.cmd;
            switch(t)
            {
				case eMCU_CPUGETINFO_CMD:   //获取设备信息的命令
				//	AnswerCpu_GetInfo(pdata->data.cmd.param1<<8 | pdata->data.cmd.param2); //使用函数解析，并返回数据
					break;
				case eMCU_CPUSET_CMD:    //设置屏幕亮度
					if(pdata->data.cmd.param1 == eMCU_LCD_SETPWM_CMD)
					{
						t = pdata->data.cmd.param2;   //这个值可正可负，根据它的正负来调亮或者灭
						t = g_lcd_pwm + t;   //计算得出新的结果
						Lcd_pwm_out(t);     //重新设置pwm的值
				//		AnswerCpu_Status(eUART_SUCCESS);   //应答成功
					}
					else if(pdata->data.cmd.param1 == eMCU_SWITCH_DVI_SRC_CMD) //切换视频源
					{
						t = pdata->data.cmd.param2;  //0 为本地，1为外部
//						if(t)
//							dvi_switch_set(DVI_OTHER);   //设置后会上报给cpu
//						else
//							dvi_switch_set(DVI_LOONGSON);   //本地视频
				//		AnswerCpu_Status(eUART_SUCCESS);   //应答成功
					}
					else	
				//		AnswerCpu_Status(eUART_ERR_PARAM);  //应答参数错误				
				break;
                default:
					DBG_PRINTF("ERROR: %s\n","eUART_ERR_PARAM");
				//	AnswerCpu_Status(eUART_ERR_PARAM);  //应答参数错误
                break;
            }

        break;
        default:
			DBG_PRINTF("ERROR: %s\n","eUART_ERR_CMD_UNKNOW");
		//	AnswerCpu_Status(eUART_ERR_CMD_UNKNOW);  //应答命令未知 
        break;
    }	
}
#endif



/*
	串口空闲中断的处理,调试串口不再开启空闲中断

	1.判断接收到的字节数，>=7 表示正常
	2.正常就继续处理，读出7个字节，计算校验和，
	3.校验和正确，则处理命令
*/
void Com_Debug_Idle_Int_Handle(void)
{
//	Com_Frame_Handle(&g_com_debug_buf, &g_Queue_Debug_Recv,Com_Debug_Message_Handle);
}


