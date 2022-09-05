
#ifndef __IR_SWITCH_H__
#define __IR_SWITCH_H__

#define IR_DETECT_USE_IRQ   //红外检测使用中断方式
#include <gd32f10x.h>


extern const  uint8_t ir_Send_DAT[5];

void ir_pwm_init(void);

////开启定时器则发送38Khz频率，此时接收器的数据为1，输出电平为0
//void ir_send_high(void);

////关闭定时器则不再发送38Khz频率，此时接收器的数据为0，输出电平为1
//void ir_send_low(void);


////红外接收引脚，设置为中断方式。
//void ir_detect_init(void);


//// 100ms 进入1次
////系统开机才检测，不开机就不用检测了！！！
//void ir_irq9_detect_task(void);


////关闭irq检测，关机时就不需要了。
//void ir_detect_off(void);


//// NEC编码发送
void IR_NEC_Send_Code(const uint8_t *Dat, uint8_t Len);


//void ir_send_code(void);
#endif


