
#ifndef __TASK_CFG_H__
#define __TASK_CFG_H__


#include <gd32f10x.h>

//������main�������á�

//ʹ�ú궨������ļ��ʱ�䣬����Ķ�ʱ��systick.c�д���

//������õĸ�ʽ void fun(void)

#define TASK_MAX 16   //Ŀǰ����������

#define TASK1_TICKS_INTERVAL 10   //����1���ϵ簴ť����ɨ�衣
#define TASK2_TICKS_INTERVAL 20   //����2 10ms�ļ��,���󰴼�ɨ��
#define TASK3_TICKS_INTERVAL 100   //����3��Ӳ�����Ź�ι������ 100ms
#define TASK4_TICKS_INTERVAL 1000   //����4��1000msɨ�裬��Ƭ���ڲ��¶ȶ�ȡ����
#define TASK5_TICKS_INTERVAL -1  //����5���������ö�ʱ����������
#define TASK6_TICKS_INTERVAL 100   //����6��led������˸���ƣ�100ms
//#define TASK7_TICKS_INTERVAL 1   //����7��led����pwm���ã�ģ��pwm��10msһ��
#define TASK15_TICKS_INTERVAL 533   //����15��ι�����ݶ�533ms
#define TASK16_TICKS_INTERVAL 1000   //����16��1sɨ�裬����led��˸����.2021-12-01 ����ɾ��

extern uint16_t g_task_id;   //ÿһ��λ��Ӧһ������Ϊ1��ʾ��Ҫ���������������������λ
//2021-09-30����task_allow,���ƶ�ʱ����������������
//extern uint16_t task_allow; //ÿһ��λ��Ӧһ������Ϊ1��ʾ����ʱɨ������񣬹ػ��󣬿��Խ�����Ҫ����������Ϊ������

typedef void(* task_t)(void);
#endif
