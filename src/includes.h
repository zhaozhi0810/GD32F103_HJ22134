
#ifndef INCLUDES_H
#define INCLUDES_H

//�����ܵ�ͷ�ļ�����

#include <gd32f10x.h>
#include <stdio.h>


//���ܿ��ƺ궨�壺
#define LCD_PWM   //lcd���ȿ��ƣ�ʹ��pwm�ķ�ʽ
//#define LCD_PWM_HEAT   //LCDʹ��pwm���ȣ�ע�͸ú��ʾ��ʹ��pwm
#define BTNS_USE_INT   //����ɨ��ʹ���жϷ�ʽ



//����lcd����ʱ���м��ȴ���
// #define LCD_HEAT_ENABLE    //����Һ�������ȹ��ܣ�ע��֮���û�м��ȹ���

extern const char* g_build_time_str;

#define DEBUG_COM_NUM 0   //���Դ��ں�
#define TOCPU_COM_NUM 1   //��cpuͨ�ŵĴ���

 #include "iic_app.h"
//#include "sys.h"
#include "systick.h"     //��ʱ����
#include "uart.h"        //���ڴ���
#include "gpios.h"       //�ߵ͵�ƽ���Ƶ�
// #include "sm2990.h"      //��ѹ��ؼ��¶ȶ�ȡ
// #ifdef LCD_HEAT_ENABLE
// #include "ds18b20.h"     //Һ�����¶ȶ�ȡ
// #endif
// #include "lcdheat_pwm.h"     //lcd���ȿ���
#ifdef LCD_PWM
#include "lcd_pwm.h"     //Һ�������ȿ���
#endif
// #include "cpu_run_states.h"  //cpu�ϵ�����״̬�ļ��
// #include "power_btn.h"    //cpu�ϵ縴λ����������������
// #include "fan_control.h"   //���ȿ��Ʋ���
// #include "vol_temp_control.h"   //��ѹ�¶ȶ�ȡ��lcd���ȿ��Ƶ�

#include "task_cfg.h"    //������صĺ궨��
#include "led_show_work.h"    //���Ե�
// #include "di_4ttl.h"     //4·����������
 #include "uart_conect_cpu_handler.h"   //��cpuͨ�ŵĴ��ڽ��շ��ʹ���
 #include "uart_debug_handle.h"        //���Դ��ڵĽ��մ���
// #include "optica_switch_status.h"        //���Դ��ڵĽ��մ���
// #include "hard_wtg.h"     //Ӳ�����Ź�
// #include "msata_destroy.h"     //Ӳ������
// #include "power_data_iic.h"     //��ȡ��Դiic����

#include "lt9211.h"
#include "matrix_keys.h"
#include "nca9555.h"     //����֮ǰ����Ŀ�еġ�
#include "key_light.h"   //�����ƵĿ���
#endif

