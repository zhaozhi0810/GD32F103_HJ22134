

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



//��ʼ�� ir���ղ��ֳ�ʼ��
void IR_Recv_Init(void);




// 100ms ����1��
//ϵͳ�����ż�⣬�������Ͳ��ü���ˣ�����
// ���ⷢ��Ҳ������ġ�
void ir_irq9_detect_task(void);
#endif
