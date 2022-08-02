
#ifndef __TASK_CFG_H__
#define __TASK_CFG_H__


#include <gd32f10x.h>

//任务由main函数调用。

//使用宏定义任务的间隔时间，任务的定时由systick.c中处理

//任务调用的格式 void fun(void)

#define TASK_MAX 16   //目前最大的任务数

#define TASK1_TICKS_INTERVAL 10   //任务1，上电按钮开关扫描。
#define TASK2_TICKS_INTERVAL 20   //任务2 10ms的间隔,矩阵按键扫描
#define TASK3_TICKS_INTERVAL 10   //任务3，10ms扫描，检测操作系统关机和重启，读io引脚
#define TASK4_TICKS_INTERVAL 1000   //任务4，1000ms扫描，单片机内部温度读取任务
#define TASK5_TICKS_INTERVAL 500   //任务5，定时向龙芯cpu汇报任务，暂定500ms
#define TASK15_TICKS_INTERVAL 533   //任务5，喂狗，暂定533ms
#define TASK16_TICKS_INTERVAL 1000   //任务16，1s扫描，工作led闪烁任务.2021-12-01 任务删除

extern uint16_t g_task_id;   //每一个位对应一个任务，为1表示需要启动任务，在任务中清零该位
//2021-09-30增加task_allow,控制定时器，设置任务运行
//extern uint16_t task_allow; //每一个位对应一个任务，为1表示允许定时扫描该任务，关机后，可以将不必要的任务设置为不允许

typedef void(* task_t)(void);
#endif
