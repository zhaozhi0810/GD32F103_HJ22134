


#ifndef __MATRIX_KEYS_H__
#define __MATRIX_KEYS_H__

#include "gd32f10x.h"

#include <string.h>



typedef struct btn_info{
	uint8_t  value;	     //ֵ��0��ʾ�ɿ���1��ʾ����
	uint8_t  reportEn;   //1��������⵽�ˣ�0û�м�⵽����
	uint16_t  pressCnt;     //��������
}BTN_INFO;


#define KEY_MAX 36


void matrix_keys_init(void);
char matrix_keys_scan(void);
uint8_t matrix_keys_row_scan(void);
//void matrix_keys_function(void);
void task_matrix_keys_scan(void);

//#define KEY_CLO0_OUT_LOW  GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_RESET)
//#define KEY_CLO1_OUT_LOW  GPIO_WriteBit(GPIOB,GPIO_Pin_13,Bit_RESET)
//#define KEY_CLO2_OUT_LOW  GPIO_WriteBit(GPIOB,GPIO_Pin_14,Bit_RESET)
//#define KEY_CLO3_OUT_LOW  GPIO_WriteBit(GPIOB,GPIO_Pin_15,Bit_RESET)

//#define KEY_CLO0_OUT_HIGH  GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_SET) 
//#define KEY_CLO1_OUT_HIGH  GPIO_WriteBit(GPIOB,GPIO_Pin_13,Bit_SET)
//#define KEY_CLO2_OUT_HIGH  GPIO_WriteBit(GPIOB,GPIO_Pin_14,Bit_SET)
//#define KEY_CLO3_OUT_HIGH  GPIO_WriteBit(GPIOB,GPIO_Pin_15,Bit_SET)


#endif



