

#ifndef __GPIOS_H__
#define __GPIOS_H__

#include <gd32f10x.h>


void Wxen_Control_Init(void);
void Wxen_Control_Enable(void);
//禁止该引脚，断电1.1v
void Wxen_Control_Disable(void);


void OePins_Control_Init(void);
//which 1-4 分别表示oe1-oe4
void OePins_Output_Hight(uint8_t which);
//which 1-4 分别表示oe1-oe4
void OePins_Output_Low(uint8_t which);


//PE15，初始化后输出低
void LcdCtrl_Control_Init(void);
//使能该引脚，通电Lcd电源
void LcdCtrl_Enable(void);
//禁止该引脚，断电Lcd电源
void LcdCtrl_Disable(void);



//PD8  lcd-reset ,不知道是高电平复位还是低电平复位
//先假设低电平复位吧。
void lcd_reset_control_init(void);
//触发一次lcd的复位信号
void lcd_reset_control(void);
#endif

