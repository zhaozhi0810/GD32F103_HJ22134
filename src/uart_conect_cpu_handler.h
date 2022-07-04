
#ifndef UART_CONECT_CPU_HANDLER_H
#define UART_CONECT_CPU_HANDLER_H


#include <gd32f10x.h>
#include "uart.h"

/*
2022-04-21 调整（不主动发送呢，就可以节省cpu和单片机的精力，不用老是处理）
	1. cpu发给单片机的都是定长，7个字节，包括帧头1+命令类型1+数据4+校验和1
	2. cpu发给单片机的类型是两种，一种是获取（温度，电压，其他状态），一种是设置（设置pwm值等）。
	3. 命令类型分为获取和设置
	4. 获取命令时，可以获取4路电压，4路电流等（参考uartcmd_bit_info_t中对应位的控制）
	
	3. 单片机发给cpu的数据是不定长的。包括帧头1+长度1+获取状态2+数据n+校验和1
	4. 单片机一般不主动发送数据给cpu（如果需要关机流程的话，需要主动发送，其他异常情况主动提醒也需要。）
	5. 

*/


//单片机向cpu回复数据，相对应的字节数。
#define VOL_DATA_BYTES 10   //10个字节,电压一共有5组，每组2个字节表示
#define CUR_DATA_BYTES 2   //电压一共有1组，共2个字节
#define LCDTEMP_DATA_BYTES 2   //两个温度，每个1个字节
#define LCDPWM_DATA_BYTES 1   //PWM 1个字节
#define CBTEMP_DATA_BYTES 2   //两个温度，每个1个字节
#define BITSTATUS_DATA_BYTES 2  //位状态信息
#define FANPWM_DATA_BYTES 1   //风扇的pwm 1个字节



typedef enum
{	
//	eMCU_LED_STATUS_TYPE=0,  //获得led的状态
//	eMCU_KEY_STATUS_TYPE,    //获得按键的状态
//	eMCU_FAN_DIV_DI_TYPE,    //获得风扇，视频源，故障灯的状态
//	eMCU_CB_TEMP_TYPE,       //获得cpu和主板温度
//	//eMCU_LCD_TYPE,			//获得lcd的温度，加热状态，亮度值
//	eMCU_VOL12_TYPE,			//获得两个电压值
//	eMCU_VOL34_TYPE,			//获得另外两个电压值
	eMCU_CMD_TYPE,          //命令模式
//	eMCU_LED_SETON_TYPE,    //设置对应的led亮
//	eMCU_LED_SETOFF_TYPE,    //设置对应的led灭
//	eMCU_DVI_SETSRC_TYPE,
//	eMCU_LCD_SETPWM_TYPE,
//	eMCU_KEY_CHANGE_TYPE,    //按键被修改上报
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
	eMCU_SHUTDOWNCPU_CMD,   //保留一个关机命令，单片机要求cpu关机
//	eMCU_ANSWER_CMD,      //应答命令
//	eMCU_SWITCH_DVI_SRC_CMD,
//	eMCU_LCD_SETPWM_UP_CMD,    //背光增亮
//	eMCU_LCD_SETPWM_DN_CMD,     //背光变暗
	eMCU_CPUGETINFO_CMD,		//cpu获取信息的命令。2022-04-21添加
	eMCU_CPUSET_CMD        //cpu的设置命令，目前包括lcd的背光pwm值，以及视频源切换
}mcu_cmd_type;


typedef enum
{
	eUART_SUCCESS = 0,
	eUART_ERR_CMD_UNKNOW = 1,  //错误码从1开始，0表示正常
	eUART_ERR_PARAM,		   //参数错误
	eUART_ERR_MCU_RUN_ERR    //单片机执行出错
}uart_err_t;


typedef enum  //cpu设置mcu的命令，
{
	eMCU_SWITCH_DVI_SRC_CMD,
	eMCU_LCD_SETPWM_CMD     //背光变暗 亮度根据第二个参数的正负增加或减少
}cpu_setmcu_t;


typedef enum   //(预留16个位定义，其他未定义的部分保留)
{
	eBITS_VOL = 0,   //4个电压值8
	eBITS_CUR,		//4个电流值8
	eBITS_CB_TMP,   //主板cpu温度2
	eBITS_LCD_TMP,      //液晶屏温度2
	eBITS_LCD_PWM,      //1
	eBITS_BITSTATUS,    //位状态，div源，屏加热，硬件看门狗，4位开关量 2
	eBITS_FAN_PWM,     //风扇pwm值，io模式下只有100和0 1
	eBITS_STATUS = 15,     //表示状态，用于mcu返回给cpu的状态，1表示成功，0表示失败
}uartcmd_bit_info_t;


//这里最多可以放两个字节
typedef struct
{
	unsigned char dvi_src:1; 		//视频源 0本地，1外部
	//unsigned char breakdownLed_status:1;   //0熄灭，1点亮
	unsigned char watch_dog_status:1; //硬件看门狗状态，0关闭，1开启
	unsigned char lcd_beat:1;   //lcd加热状态
	unsigned char di_4ttl:4;    //4位开关量上报	
	unsigned char res;   //保留之后使用
}bitstatus_t;

//2021-12-15,修改改结构体
typedef struct {
	unsigned char fan_pwm;      //0-100 风扇的pwm值
	unsigned char lcd_pwm;     //0-100 lcd的pwm值
	bitstatus_t bstatus;		//其他的位状态 
}fan_div_di_t;

//液晶屏温度，cpu和主板温度，分别使用
typedef struct {
	short temp1;  
	short temp2;
}temp_t;


//电压，分两次使用吧
typedef struct{
	short vol1;
	short vol2;
}vol_t;


//typedef struct{
//		unsigned int vol_core : 10;     //三个电压正常在1.1v左右，adc得出结果12位，去掉最高位，去掉最低位，保留中间10位
//		unsigned int vol_7A : 10;
//		unsigned int vol_12V : 10;
//	}vol_t;


typedef struct {
		unsigned char cmd;      //串口关机，重启命令，视频切换，应答
		unsigned char param_len;      //有几个字节参数，0-3，0表示没有参数
		unsigned char param1;   //有参数的话，设置参数值，无参数时忽略。！！！！应答时，表示应答结果，0表示正常，非零表示错误
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


//#pragma pack(1) 这个会设置全局的，注释掉

typedef struct
{
	unsigned char data_type;
	mcu_data_t data;
	unsigned char crc;     //校验和
}__attribute__((packed))com_frame_t;    //注意对齐方式


#define FRAME_LENGHT (8)    //数据帧的字节数


typedef struct frame_buf
{
	uint8_t com_handle_buf[FRAME_LENGHT];   //接收缓存
	uint8_t datalen;            //帧长-缓存中的数据长度，即下一次要读的字节数
}frame_buf_t;


//消息处理的函数指针
typedef void (*message_handle)(uint8_t );

//中断处理函数
void Com_Cpu_Rne_Int_Handle(void);
//帧数据处理函数
void Com_Frame_Handle(frame_buf_t* buf, Queue_UART_STRUCT* Queue_buf,message_handle handle);
//中断处理函数
void Com_Cpu_Idle_Int_Handle(void);




void Send_Fan_Div_Status_ToCpu(bitstatus_t b_status,uint8_t fan_pwm,uint8_t lcd_pwm);
#if 0
//发送2个电压
void Send_Vol_ToCpu(data_type type,short vol1,short vol2);
//发送温度
void Send_Temp_ToCpu(data_type type,short cpu_temp,short board_temp);

#endif
//发送dvi视频被切换的数据到cpu
//source 1（本地）或者2（外部）
void Send_Dvi_Change_ToCpu(int source);

//发送命令数据到cpu
//cmd请参考uart.h中宏定义
//param 参数。
void Send_Cmd_ToCpu(int cmd,int param);

//应答cpu的获取信息的请求
void AnswerCpu_GetInfo(uint16_t ask);
//应答cpu的设置信息的请求 errcode为0表示成功，其他值为错误码 应小于0x7f
void AnswerCpu_Status(uart_err_t errcode);


//缓存初始化
void Com_Cpu_Recive_Buff_Init(void);

#endif
