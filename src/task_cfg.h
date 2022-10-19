
#ifndef __TASK_CFG_H__
#define __TASK_CFG_H__


#include <gd32f10x.h>

//任务由main函数调用。

//使用宏定义任务的间隔时间，任务的定时由systick.c中处理

//任务调用的格式 void fun(void)

#define TASK_MAX 16   //目前最大的任务数

#define TASK1_TICKS_INTERVAL 10   //任务1，上电按钮开关扫描。
#define TASK2_TICKS_INTERVAL 20   //任务2 10ms的间隔,矩阵按键扫描
#define TASK3_TICKS_INTERVAL 100   //任务3，硬件看门狗喂狗任务 100ms
#define TASK4_TICKS_INTERVAL 1000   //任务4，1000ms扫描，单片机内部温度读取任务
#define TASK5_TICKS_INTERVAL -1  //任务5，不能设置定时器！！！！
#define TASK6_TICKS_INTERVAL 100   //任务6，led键灯闪烁控制，100ms
//#define TASK7_TICKS_INTERVAL 1   //任务7，led亮度pwm设置（模拟pwm）10ms一次
#define TASK15_TICKS_INTERVAL 533   //任务15，喂狗，暂定533ms
#define TASK16_TICKS_INTERVAL 1000   //任务16，1s扫描，工作led闪烁任务.2021-12-01 任务删除

extern uint16_t g_task_id;   //每一个位对应一个任务，为1表示需要启动任务，在任务中清零该位
//2021-09-30增加task_allow,控制定时器，设置任务运行
//extern uint16_t task_allow; //每一个位对应一个任务，为1表示允许定时扫描该任务，关机后，可以将不必要的任务设置为不允许

typedef void(* task_t)(void);
#endif
