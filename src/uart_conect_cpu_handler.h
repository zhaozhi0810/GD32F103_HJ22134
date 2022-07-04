
#ifndef UART_CONECT_CPU_HANDLER_H
#define UART_CONECT_CPU_HANDLER_H


#include <gd32f10x.h>
#include "uart.h"

/*
2022-04-21 �����������������أ��Ϳ��Խ�ʡcpu�͵�Ƭ���ľ������������Ǵ���
	1. cpu������Ƭ���Ķ��Ƕ�����7���ֽڣ�����֡ͷ1+��������1+����4+У���1
	2. cpu������Ƭ�������������֣�һ���ǻ�ȡ���¶ȣ���ѹ������״̬����һ�������ã�����pwmֵ�ȣ���
	3. �������ͷ�Ϊ��ȡ������
	4. ��ȡ����ʱ�����Ի�ȡ4·��ѹ��4·�����ȣ��ο�uartcmd_bit_info_t�ж�Ӧλ�Ŀ��ƣ�
	
	3. ��Ƭ������cpu�������ǲ������ġ�����֡ͷ1+����1+��ȡ״̬2+����n+У���1
	4. ��Ƭ��һ�㲻�����������ݸ�cpu�������Ҫ�ػ����̵Ļ�����Ҫ�������ͣ������쳣�����������Ҳ��Ҫ����
	5. 

*/


//��Ƭ����cpu�ظ����ݣ����Ӧ���ֽ�����
#define VOL_DATA_BYTES 10   //10���ֽ�,��ѹһ����5�飬ÿ��2���ֽڱ�ʾ
#define CUR_DATA_BYTES 2   //��ѹһ����1�飬��2���ֽ�
#define LCDTEMP_DATA_BYTES 2   //�����¶ȣ�ÿ��1���ֽ�
#define LCDPWM_DATA_BYTES 1   //PWM 1���ֽ�
#define CBTEMP_DATA_BYTES 2   //�����¶ȣ�ÿ��1���ֽ�
#define BITSTATUS_DATA_BYTES 2  //λ״̬��Ϣ
#define FANPWM_DATA_BYTES 1   //���ȵ�pwm 1���ֽ�



typedef enum
{	
//	eMCU_LED_STATUS_TYPE=0,  //���led��״̬
//	eMCU_KEY_STATUS_TYPE,    //��ð�����״̬
//	eMCU_FAN_DIV_DI_TYPE,    //��÷��ȣ���ƵԴ�����ϵƵ�״̬
//	eMCU_CB_TEMP_TYPE,       //���cpu�������¶�
//	//eMCU_LCD_TYPE,			//���lcd���¶ȣ�����״̬������ֵ
//	eMCU_VOL12_TYPE,			//���������ѹֵ
//	eMCU_VOL34_TYPE,			//�������������ѹֵ
	eMCU_CMD_TYPE,          //����ģʽ
//	eMCU_LED_SETON_TYPE,    //���ö�Ӧ��led��
//	eMCU_LED_SETOFF_TYPE,    //���ö�Ӧ��led��
//	eMCU_DVI_SETSRC_TYPE,
//	eMCU_LCD_SETPWM_TYPE,
//	eMCU_KEY_CHANGE_TYPE,    //�������޸��ϱ�
//    eMCU_BKLEDSET_TYPE,
//    eMCU_LEDSETALL_TYPE,
//    eMCU_DIV_CHANGE_TYPE,
//    eMCU_COREVOL_CHANGE_TYPE,
//    eMCU_7AVOL_CHANGE_TYPE,
//    eMCU_12VVOL_CHANGE_TYPE,
//	eMCU_LCD_TEMP_TYPE
}data_type;



typedef enum
{
//	eMCU_REBOOT_CMD = 0,
	eMCU_SHUTDOWNCPU_CMD,   //����һ���ػ������Ƭ��Ҫ��cpu�ػ�
//	eMCU_ANSWER_CMD,      //Ӧ������
//	eMCU_SWITCH_DVI_SRC_CMD,
//	eMCU_LCD_SETPWM_UP_CMD,    //��������
//	eMCU_LCD_SETPWM_DN_CMD,     //����䰵
	eMCU_CPUGETINFO_CMD,		//cpu��ȡ��Ϣ�����2022-04-21���
	eMCU_CPUSET_CMD        //cpu���������Ŀǰ����lcd�ı���pwmֵ���Լ���ƵԴ�л�
}mcu_cmd_type;


typedef enum
{
	eUART_SUCCESS = 0,
	eUART_ERR_CMD_UNKNOW = 1,  //�������1��ʼ��0��ʾ����
	eUART_ERR_PARAM,		   //��������
	eUART_ERR_MCU_RUN_ERR    //��Ƭ��ִ�г���
}uart_err_t;


typedef enum  //cpu����mcu�����
{
	eMCU_SWITCH_DVI_SRC_CMD,
	eMCU_LCD_SETPWM_CMD     //����䰵 ���ȸ��ݵڶ����������������ӻ����
}cpu_setmcu_t;


typedef enum   //(Ԥ��16��λ���壬����δ����Ĳ��ֱ���)
{
	eBITS_VOL = 0,   //4����ѹֵ8
	eBITS_CUR,		//4������ֵ8
	eBITS_CB_TMP,   //����cpu�¶�2
	eBITS_LCD_TMP,      //Һ�����¶�2
	eBITS_LCD_PWM,      //1
	eBITS_BITSTATUS,    //λ״̬��divԴ�������ȣ�Ӳ�����Ź���4λ������ 2
	eBITS_FAN_PWM,     //����pwmֵ��ioģʽ��ֻ��100��0 1
	eBITS_STATUS = 15,     //��ʾ״̬������mcu���ظ�cpu��״̬��1��ʾ�ɹ���0��ʾʧ��
}uartcmd_bit_info_t;


//���������Է������ֽ�
typedef struct
{
	unsigned char dvi_src:1; 		//��ƵԴ 0���أ�1�ⲿ
	//unsigned char breakdownLed_status:1;   //0Ϩ��1����
	unsigned char watch_dog_status:1; //Ӳ�����Ź�״̬��0�رգ�1����
	unsigned char lcd_beat:1;   //lcd����״̬
	unsigned char di_4ttl:4;    //4λ�������ϱ�	
	unsigned char res;   //����֮��ʹ��
}bitstatus_t;

//2021-12-15,�޸ĸĽṹ��
typedef struct {
	unsigned char fan_pwm;      //0-100 ���ȵ�pwmֵ
	unsigned char lcd_pwm;     //0-100 lcd��pwmֵ
	bitstatus_t bstatus;		//������λ״̬ 
}fan_div_di_t;

//Һ�����¶ȣ�cpu�������¶ȣ��ֱ�ʹ��
typedef struct {
	short temp1;  
	short temp2;
}temp_t;


//��ѹ��������ʹ�ð�
typedef struct{
	short vol1;
	short vol2;
}vol_t;


//typedef struct{
//		unsigned int vol_core : 10;     //������ѹ������1.1v���ң�adc�ó����12λ��ȥ�����λ��ȥ�����λ�������м�10λ
//		unsigned int vol_7A : 10;
//		unsigned int vol_12V : 10;
//	}vol_t;


typedef struct {
		unsigned char cmd;      //���ڹػ������������Ƶ�л���Ӧ��
		unsigned char param_len;      //�м����ֽڲ�����0-3��0��ʾû�в���
		unsigned char param1;   //�в����Ļ������ò���ֵ���޲���ʱ���ԡ���������Ӧ��ʱ����ʾӦ������0��ʾ�����������ʾ����
		unsigned char param2;
//		unsigned char param3;
}cmd_t;


typedef union 
{
	fan_div_di_t fdd;
	temp_t cb_temp;
	temp_t lcd_temp;
	vol_t vol_12;
//	vol_t vol_34;
	cmd_t cmd;
}mcu_data_t;


//#pragma pack(1) ���������ȫ�ֵģ�ע�͵�

typedef struct
{
	unsigned char data_type;
	mcu_data_t data;
	unsigned char crc;     //У���
}__attribute__((packed))com_frame_t;    //ע����뷽ʽ


#define FRAME_LENGHT (8)    //����֡���ֽ���


typedef struct frame_buf
{
	uint8_t com_handle_buf[FRAME_LENGHT];   //���ջ���
	uint8_t datalen;            //֡��-�����е����ݳ��ȣ�����һ��Ҫ�����ֽ���
}frame_buf_t;


//��Ϣ����ĺ���ָ��
typedef void (*message_handle)(uint8_t );

//�жϴ�����
void Com_Cpu_Rne_Int_Handle(void);
//֡���ݴ�����
void Com_Frame_Handle(frame_buf_t* buf, Queue_UART_STRUCT* Queue_buf,message_handle handle);
//�жϴ�����
void Com_Cpu_Idle_Int_Handle(void);




void Send_Fan_Div_Status_ToCpu(bitstatus_t b_status,uint8_t fan_pwm,uint8_t lcd_pwm);
#if 0
//����2����ѹ
void Send_Vol_ToCpu(data_type type,short vol1,short vol2);
//�����¶�
void Send_Temp_ToCpu(data_type type,short cpu_temp,short board_temp);

#endif
//����dvi��Ƶ���л������ݵ�cpu
//source 1�����أ�����2���ⲿ��
void Send_Dvi_Change_ToCpu(int source);

//�����������ݵ�cpu
//cmd��ο�uart.h�к궨��
//param ������
void Send_Cmd_ToCpu(int cmd,int param);

//Ӧ��cpu�Ļ�ȡ��Ϣ������
void AnswerCpu_GetInfo(uint16_t ask);
//Ӧ��cpu��������Ϣ������ errcodeΪ0��ʾ�ɹ�������ֵΪ������ ӦС��0x7f
void AnswerCpu_Status(uart_err_t errcode);


//�����ʼ��
void Com_Cpu_Recive_Buff_Init(void);

#endif
