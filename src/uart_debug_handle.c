
/*
���ڴ�����cpu֮��Ĵ���ͨ��
	����ΪGD32�Ĵ���1��115200��8N1


���Դ��ڽ��յ����
0.�������ʱ���ַ���
1.��ѹ����
2.cpu�������¶ȣ�Һ�����¶ȣ���lcd����״̬������pwmֵ��ioģʽ��ֻ��0��100��
3.lcd������ֵ�����ļӵ�����״̬��pd_n��״̬
4.4·diֵ��4·��ͨ·��Ϣ
5.Ӳ�����Ź�״̬�����ź�Դ�����ޣ�
6.cpu����״̬���������ػ�������������pmon������ϵͳ�ȣ�

����ʶ�������Ҳ�Ǵ�ӡ��ʾ�ͱ���ʱ���ַ���
*/



#include "includes.h"



//#define RECV_BUF_LEN 64
#if 0
static Queue_UART_STRUCT g_Queue_Debug_Recv;   //����Debug���ݶ��У����ڽ����ж�

frame_buf_t g_com_debug_buf={{0},FRAME_LENGHT};    //���ݴ�����
#endif

//static char* g_Cpu_Run_Status_str[] = {
//	"LS3A_POWER_DISABLE",   //ȷ���Ƕϵ�״̬
//	"LS3A_POWEROFF",    //�ػ����ϵ�
//	"LS3A_REBOOT",    //����
//	"LS3A_RUNNING",    //����pmon
//	"LS3A_RUN_OS",      //�뿪pmon���������ϵͳ
//	"LS3A_POWER_ENABLE"     //�Ѿ�ͨ�磬����û�н���PMON��ǰһ��;
//};

//�����������������Դ��ڽ��յ��ļ򵥵ĵ�������
static void Com_Debug_Message_Handle1(uint8_t buf)
{
//	uint8_t t;
	switch(buf)
	{
		default:   //cmd��ӡ��ʱ�򣬿��ܳ����˿���ʾ�ַ�������
			printf("ERROR: Command Unknow cmd = 0x%x!!!\n\r",buf);   //����ʶ�������
		case '0':
			printf("%s\n\r",g_build_time_str);  //��ӡ�����ʱ��
		break;
		case '1':
//			printf("p0v95_vol = %d mv,p1v0_vol  = %d mv, p1v2_vol  = %d mv,p12v_vol = %d mv\n\r",g_p0v95_vol,g_p1v0_vol, g_p1v2_vol,g_p12v_vol);
			//��ӡ����ֵ
		//	printf("g_p0v95_vol = %d mv,g_p1v0_vol  = %d mv, g_p1v2_vol  = %d mv,g_p12v_vol = %d mv\n\r",g_p0v95_vol,g_p1v0_vol, g_p1v2_vol,g_p12v_vol);
			break;
		case '2':
			//��ӡ�¶�ֵ
//			printf("cpu temp = %d,board temp = %d\n\r",g_cpu_temp>>4,g_board_temp>>4);
			#ifdef LCD_HEAT_ENABLE		
			printf("lcd1 temp = %d,lcd2 temp  = %d\n\r",g_lcd_temp[0]>>4,g_lcd_temp[1]>>4);
			printf("lcd heat %s,fan_pwm = %d\n\r",Get_Lcd_Heat_Status()?"on":"off",g_fan_pwm);
			#endif
			break;
		case '3':
//			printf("lcd Power %s\n\r",Get_Lcd_Power_Status()?"on":"off");  //lcd�ӵ�״̬
//			printf("lcd Pd_n status %s\n\r",Get_Lcd_PdN_Status()?"on":"off");  //lcd ttlת����״̬
//			printf("lcd light pwm = %d\n\r",g_lcd_pwm);   //lcd������pwmֵ
			break;
		case '4':
//			t = Get_Di_4Ttl_Status();   //4·����������DI PB12-PB15
//			printf("4Di Di1 %s,Di2 %s,Di3 %s,Di4 %s \n\r",t&1?"on":"off",t&2?"on":"off",t&4?"on":"off",t&8?"on":"off");
//			t = Get_Optica_Switch_Status();  //4·�⿪��״̬
//			printf("4 op switch D2_STATE2 %s ,D2_STATE1 %s,D1_STATE1 %s,D1_STATE2 %s \n\r",
//								t&1?"on":"off",t&2?"on":"off",t&4?"on":"off",t&8?"on":"off");
			break;
		case '5':
			printf("Watch Dog Status = %s\n\r","off");   //��ʱû�п���
			break;
		case '6':
//			printf("Cpu Run Status = %s\n\r",g_Cpu_Run_Status_str[g_cpu_run_status-1]);
			break;
	}
}


/*
	�������ݽ����жϣ�
		ǰ�᣺ÿһ֡����7���ֽڡ�
		�����б���֡ͷ���к�������ݺ�У��ͣ���7���ֽڣ�
*/
void Com_Debug_Rne_Int_Handle(void)
{
	uint8_t dat;

	dat = (uint8_t)usart_data_receive(EVAL_COM0);//(USART3);  
	Com_Debug_Message_Handle1(dat);   //ֱ�Ӵ���
//	QueueUARTDataInsert(&g_Queue_Debug_Recv,dat);   //���յ����ݴ�������С�
}


/*
	�����յ������Ĵ������ڵĿ����жϴ���������
		ǰ�᣺ �յ����������ݰ���У�����ȷ��

	��Ƭ���ܹ��յ������
	// 1.������ƵԴ,û�иù���
	4.����lcd��pwm�����ȣ�
	5.�ػ����������

*/

#if 0

static void Com_Debug_Message_Handle(uint8_t* buf)
{		
	com_frame_t* pdata = (com_frame_t*)(buf+1);    //+1������֡ͷ��ʹ�ýṹ���ʼ��
	int8_t t;
	
	switch(pdata->data_type)
    {
        case eMCU_CMD_TYPE:    //cpu���͸���Ƭ���Ķ���cmd����
            t = pdata->data.cmd.cmd;
            switch(t)
            {
				case eMCU_CPUGETINFO_CMD:   //��ȡ�豸��Ϣ������
				//	AnswerCpu_GetInfo(pdata->data.cmd.param1<<8 | pdata->data.cmd.param2); //ʹ�ú�������������������
					break;
				case eMCU_CPUSET_CMD:    //������Ļ����
					if(pdata->data.cmd.param1 == eMCU_LCD_SETPWM_CMD)
					{
						t = pdata->data.cmd.param2;   //���ֵ�����ɸ���������������������������
						t = g_lcd_pwm + t;   //����ó��µĽ��
						Lcd_pwm_out(t);     //��������pwm��ֵ
				//		AnswerCpu_Status(eUART_SUCCESS);   //Ӧ��ɹ�
					}
					else if(pdata->data.cmd.param1 == eMCU_SWITCH_DVI_SRC_CMD) //�л���ƵԴ
					{
						t = pdata->data.cmd.param2;  //0 Ϊ���أ�1Ϊ�ⲿ
//						if(t)
//							dvi_switch_set(DVI_OTHER);   //���ú���ϱ���cpu
//						else
//							dvi_switch_set(DVI_LOONGSON);   //������Ƶ
				//		AnswerCpu_Status(eUART_SUCCESS);   //Ӧ��ɹ�
					}
					else	
				//		AnswerCpu_Status(eUART_ERR_PARAM);  //Ӧ���������				
				break;
                default:
					DBG_PRINTF("ERROR: %s\n","eUART_ERR_PARAM");
				//	AnswerCpu_Status(eUART_ERR_PARAM);  //Ӧ���������
                break;
            }

        break;
        default:
			DBG_PRINTF("ERROR: %s\n","eUART_ERR_CMD_UNKNOW");
		//	AnswerCpu_Status(eUART_ERR_CMD_UNKNOW);  //Ӧ������δ֪ 
        break;
    }	
}
#endif



/*
	���ڿ����жϵĴ���,���Դ��ڲ��ٿ��������ж�

	1.�жϽ��յ����ֽ�����>=7 ��ʾ����
	2.�����ͼ�����������7���ֽڣ�����У��ͣ�
	3.У�����ȷ����������
*/
void Com_Debug_Idle_Int_Handle(void)
{
//	Com_Frame_Handle(&g_com_debug_buf, &g_Queue_Debug_Recv,Com_Debug_Message_Handle);
}


