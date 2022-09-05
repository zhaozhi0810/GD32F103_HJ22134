

#ifndef __IR_REMOTE_H__
#define __IR_REMOTE_H__


#define TOLERANCE 20

typedef enum {
	IR_NEC_NONE = 0,
	IR_NEC_NDEF,
	IR_NEC_FIRST_BURST,
	IR_NEC_SECOND_BURST,
	IR_NEC_SECOND_BURST_REPEAT,
	IR_NEC_1
} ir_nec_state;



//初始化 ir接收部分初始化
void IR_Recv_Init(void);




// 100ms 进入1次
//系统开机才检测，不开机就不用检测了！！！
// 红外发送也是在这的。
void ir_irq9_detect_task(void);
#endif
