
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

static void  Com_Debug_Print_Help(void)
{
	printf("\r\nDebug cmd:\r\n");
	printf("0. print Program build time\r\n");
	if(Get_Lcd_Type()){  //����1��ʾ7������0��ʾ5����
		printf("1. 7 inch lcd PWM increace(5inch lcd has no effect)\r\n");
		printf("2. 7 inch lcd PWM decreace(5inch lcd has no effect)\r\n");
	}
	printf("3. reset core board!!\r\n");
	printf("4. reset LCD & 9211\r\n");
	printf("5. print Hard Watch Dog Status\r\n");
	printf("6. print Mcu internal_temp\r\n");
	printf("other. print help\r\n");
}




//�����������������Դ��ڽ��յ��ļ򵥵ĵ�������
static void Com_Debug_Message_Handle1(uint8_t buf)
{
//	uint8_t t;
	switch(buf)
	{
		default:   //cmd��ӡ��ʱ�򣬿��ܳ����˿���ʾ�ַ�������
			printf("ERROR: Command Unknow cmd = 0x%x!!!\r\n",buf);   //����ʶ�������
			Com_Debug_Print_Help();
		case '0':
			printf("%s\r\n",g_build_time_str);  //��ӡ�����ʱ��
			printf("Author:JC&DaZhi <vx:285408136>\r\n"); 
		break;
		case '1':
			if(Get_Lcd_Type()){  //����1��ʾ7������0��ʾ5����
				if(g_lcd_pwm < 100)
				{
					Lcd_pwm_out(g_lcd_pwm + 10);   //��Ļ���ȼ�10
					printf("increase 7 inch lcd PWM,g_lcd_pwm = %d \r\n",g_lcd_pwm);
				}
				else
					printf("g_lcd_pwm = 100\r\n");
			}
			break;
		case '2':
			//��ӡ�¶�ֵ
			if(Get_Lcd_Type()){  //����1��ʾ7������0��ʾ5����
				if(g_lcd_pwm >= 10)
				{
					Lcd_pwm_out(g_lcd_pwm - 10);   //��Ļ���ȼ�10
					printf("decrease 7 inch lcd PWM,g_lcd_pwm = %d \r\n",g_lcd_pwm);
				}
				else
					printf("g_lcd_pwm = 0\r\n");
			}
			break;
		case '3':
			printf("reset core board!!\r\n");  //lcd�ӵ�״̬
			hard_wtd_reset_3399board();
			break;
		case '4':
			printf("reset LCD & 9211\r\n");  //lcd�ӵ�״̬
			LT9211_Config();
			break;
		case '5':
			printf("Watch Dog Status = %s\r\n",get_hard_wtd_status()?"on":"off");   //��ʱû�п���
			break;
		case '6':
			printf("Mcu internal_temp = %d\r\n",get_internal_temp());
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


