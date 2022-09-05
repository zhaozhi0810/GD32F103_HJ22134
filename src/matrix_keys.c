

/*
	矩阵按键扫描
	
	6*6  通过iic扩展出来的。
	

*/




#include "includes.h"



//uint8_t Key_row[1]={0xff};   //定义一个数组，存放行扫描状态
		
	//只有6行，这些数字用于分别扫描每一行
const uint8_t key_scan_line[] = {0xfe,0xfd,0xfb,0xf7,0xef,0xdf};
//const uint8_t key_scan_linecode[] = {1,2,3,4,5,6};   //行编码



#define KEYS_IIC_ADDR (0)  //只表示A2A1A0 3个引脚的值
#define NCA9555_IIC_CONTROLER  IIC3_INDEX   //对应外部中断13  2021-12-07

// 6*6 的键盘矩阵，总共有33个按键，按键个数在h文件中定义
static BTN_INFO g_btns_info;

static uint8_t btn_press_num = 0;  //0表示没有按键被按下，1-36表示对应的按键被按下
uint8_t btn_start_scan = 0;   //0表示没有按键被按下，1表示按键触发中断





//端口的配置
void matrix_keys_init(void)
{
	uint8_t outcfg_dat[2]={0,0xff};   //IIC芯片GPIO输出模式，对应的位要设置为0
	//1. iic的初始化
	nca9555_init(NCA9555_IIC_CONTROLER);
		
	//矩阵按键，P0端口配置为输出，P1端口配置为输入，因为P1端口上用了上拉电阻
	nca9555_write_2config(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,outcfg_dat);

	nca9555_write_outport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,0, 0); //P0端口输出0
	
	//key_board cs引脚PE8，不需要？？
//	rcu_periph_clock_enable(RCU_GPIOE);			
//	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_8);	
//	gpio_bit_reset(GPIOE, GPIO_PIN_8);

#ifdef 	BTNS_USE_INT   //宏在btns_leds.h中定义
	//中断引脚初始化
	//2. 中断引脚的初始化 PB12，外部中断12
	//2.1 时钟使能
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_AF);		
	
	gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_12);	
	
	//2.2 复用为外部中断引脚，
	//GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_12);
	
	//设置触发方式，低电平触发
	exti_init(EXTI_12, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_interrupt_enable(EXTI_12);
	exti_interrupt_flag_clear(EXTI_12);
	//2.3 nvic允许中断
	//中断控制器使能，使用的是外部中断12
	nvic_irq_enable(EXTI10_15_IRQn,  1, 2);   //允许中断，并设置优先级

	//初始化之后读取一次
	matrix_keys_row_scan();	

#endif		
	
	memset(&g_btns_info,0,sizeof(g_btns_info));   //数据清零
	
	
}


#ifdef 	BTNS_USE_INT
//外部中断12的处理函数,按键按下和松开都会触发中断！！！！
void exint12_handle(void)
{
	btn_start_scan = 1;
	//并且禁止中断
	exti_interrupt_disable(EXTI_12);   //扫描完毕之后再使能
}

#endif





uint8_t matrix_keys_row_scan(void)
{
	uint8_t key_row_dat;
	
	if(nca9555_read_inport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,1,&key_row_dat) == 0)  //表示读取成功
	{
		if((key_row_dat&0x3f) != 0x3f)   //只判断低6位，不相等表示有按键按下
		{
			return key_row_dat&0x3f;
		}
		else
		{
			return 0x3f;
		//	printf("ERROR: KEY_ROW_SCAN key_row_dat == 0x3f\r\n");
		}
	}
	else //iic读取失败
	{
		printf("ERROR: KEY_ROW_SCAN nca9555_read_inport\r\n");
		return 0xff;
	}
}




const uint8_t key_t_code[] = {1,7,13,19,25,31, 
							 2,8,14,20,26,32,
							 3,9,15,21,27,33,
							 4,10,16,22,28,34,
							5,11,17,23,29,35,
							6,12,18,24,30,36
							};

							
							
#if 0							
/***
 *函数名：KEY_SCAN
 *功  能：6*6按键扫描
 *返回值：1~36，对应36个按键,0表示没有检测到
 */
char matrix_keys_scan(void)
{    
    uint8_t Key_Num=0xff;            //1-16对应的按键数
    uint8_t key_row_num=0;        //行扫描结果记录
    uint8_t i;
	
	key_row_num = matrix_keys_row_scan();
	if(key_row_num < 0x3f)   //读取到了一个有效的按键触发
	{
		for(i=0;i<6;i++)  //每一行扫描一次
		{
			if(nca9555_write_outport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,0, key_scan_line[i])) //P0端口输出0
			{
				printf("ERROR: KEY_ROW_SCAN nca9555_write_outport i=%d\r\n",i);
				continue;  //写入失败，直接往下试试
				//	return -1;
			}
			//再次读取输入
			key_row_num = matrix_keys_row_scan();
			
			if(key_row_num >= 0x3f)  //返回值表示没有按键按下或出错
				continue;  //继续
			
			switch(key_row_num)
			{
				case 0x3e:
					Key_Num = i;
					break;
				case 0x3d:
					Key_Num = 6+i;
					break;
				case 0x3b:
					Key_Num = 12+i;
					break;
				case 0x37:
					Key_Num = 18+i;
					break;
				case 0x2f:
					Key_Num = 24+i;
					break;
				case 0x1f:	
					Key_Num = 30+i;
					break;
				default:
					Key_Num = 37;  //表示同时按下多个键
					continue;  //继续
			}
			//读取到了按键值			
			if(Key_Num > 36)
			{
				break;
			}
		}
	}

	nca9555_write_outport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,0, 0);	
	
	if(Key_Num > 36)
	{
		if(Key_Num == 37)
			return 37;
		return 0;
	}
	return key_t_code[Key_Num];
}






/*
	main函数周期扫描任务，30ms一次,查询方式

	btn_start_scan 由中断函数设置，按下和松开都会设置为非0
	抖动触发。
*/
void task_matrix_keys_scan(void)
{
	static uint8_t scan_num = 0;   //扫描计数，用于消抖
	uint8_t ret;

#ifndef BTNS_USE_INT	
	if(btn_start_scan == 0)
		btn_start_scan =1;   //开始扫描
#endif	
	if(btn_start_scan ) //外部（按下和松开都会触发）中断触发后，不为0.
	{		
		ret = matrix_keys_scan();		
		if(ret && ret < 37) //有按键被按下
		{	
			if(btn_press_num == 0)   //还没有记录该按键值
			{
				scan_num++;
				if(scan_num>1)  //至少扫描到两次，用于消抖
				{					
					btn_press_num = ret;
					send_btn_change_to_cpu(btn_press_num,1); //发送按键按下
					//可以在这个位置上报按键状态
					MY_PRINTF("task1_btn_scan ret = %d\r\n",ret);					
				}
			}
		}
		else if(ret==0) //按键已松开
		{
			btn_start_scan = 0;
			if(btn_press_num)
			{
				send_btn_change_to_cpu(btn_press_num,0);  //发送按键松开
				btn_press_num = 0;	  //松开后，按键值为0
				scan_num = 0;	 //扫描次数清零		
				MY_PRINTF("release\r\n");
			}
			
#ifdef 	BTNS_USE_INT	
			exti_interrupt_enable(EXTI_12);   //扫描完毕之后再使能		
#endif
			//printf("release\r\n");
		}//if(ret)		
	}
	//else
		//;//scan_num = 0;     //可以省略吧？？
}
//#endif





#else
/***
 *函数名：KEY_SCAN
 *功  能：6*6按键扫描
 *返回值：1~36，对应36个按键,0表示没有检测到
 */
char matrix_keys_scan(void)
{    
    uint8_t Key_Num=0xff;            //1-16对应的按键数
    uint8_t key_row_num=0;        //行扫描结果记录
    uint8_t i,j;
	uint8_t index;   //
	static uint8_t release_report = 0;  //松开上报。
	
	key_row_num = matrix_keys_row_scan();
	if(key_row_num < 0x3f)   //读取到了一个有效的按键触发
	{
		for(i=0;i<COL_NUM;i++)  //每一列扫描一次
		{
			if(nca9555_write_outport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,0, key_scan_line[i])) //P0端口输出0
			{
				printf("ERROR: KEY_ROW_SCAN nca9555_write_outport i=%d\r\n",i);
				continue;  //写入失败，直接往下试试
				//	return -1;
			}
			//再次读取输入
			key_row_num = matrix_keys_row_scan();
			
//			if(key_row_num >= 0x3f)  //返回值表示没有按键按下或出错
//				continue;  //继续
			
//			printf("i = %d ,key_row_num = %#x\r\n",i,key_row_num);
			
			
			for(j=0;j<ROW_NUM;j++)  //每一行扫描一次
			{
				index = 6*i+j;
				if(!((key_row_num>>j)&(1))) //按下
				{
					if(g_btns_info.pressCnt[index] < 2)
					{	
						g_btns_info.pressCnt[index]++;
						if(g_btns_info.pressCnt[index] == 2)//检测到不止1次
						{   //条件限制上报一次
							g_btns_info.value[index] = 1;
						//	g_btns_info.reportEn[index] = 1;  //按下上报
							send_btn_change_to_cpu(index+1,1); //发送按键按下/松开
						//	printf("----btn:%d press\r\n",index+1);
							release_report = 1;   //记录需要释放标志
						}
					}
				}
				else //松开
				{
					if(g_btns_info.value[index]) //之前的状态是按下
					{
						g_btns_info.value[index] = 0;
					//	g_btns_info.reportEn[index] = 2;   //松开上报
						send_btn_change_to_cpu(index+1,0); //发送按键按下/松开
					//	printf("++++btn:%d release\r\n",index);
						g_btns_info.pressCnt[index] = 0;
					}
					
					//if(g_btns_info.pressCnt[index])   //消抖次数检测
						
				}
			}
			
//			switch(key_row_num)
//			{
//				case 0x3e:
//					Key_Num = i;
//					break;
//				case 0x3d:
//					Key_Num = 6+i;
//					break;
//				case 0x3b:
//					Key_Num = 12+i;
//					break;
//				case 0x37:
//					Key_Num = 18+i;
//					break;
//				case 0x2f:
//					Key_Num = 24+i;
//					break;
//				case 0x1f:	
//					Key_Num = 30+i;
//					break;
//				default:
//					Key_Num = 37;  //表示同时按下多个键
//					continue;  //继续
//			}
//			//读取到了按键值			
//			if(Key_Num > 36)
//			{
//				break;
//			}
		}
	}
	else
	{
		if(release_report)  //需要上报释放信息。
		{		
			for(index=0;index<COL_NUM*ROW_NUM;index++)
			{
				if(g_btns_info.value[index]) //之前的状态是按下
				{
					g_btns_info.value[index] = 0;
				//	g_btns_info.reportEn[index] = 2;   //松开上报
					send_btn_change_to_cpu(index+1,0); //发送按键按下/松开
				//	printf("++++btn:%d release\r\n",index);
					g_btns_info.pressCnt[index] = 0;
				}
				
			}
			release_report = 0;
		}
	}

	nca9555_write_outport(NCA9555_IIC_CONTROLER,KEYS_IIC_ADDR,0, 0);	
	
//	if(Key_Num > 36)
//	{
//		if(Key_Num == 37)
//			return 37;
//		return 0;
//	}
//	return key_t_code[Key_Num];
	return 0;
}







/*
	main函数周期扫描任务，30ms一次,查询方式

	btn_start_scan 由中断函数设置，按下和松开都会设置为非0
	抖动触发。
*/
void task_matrix_keys_scan(void)
{
	uint8_t i = 0;   //扫描计数，用于消抖
	uint8_t ret;

#ifndef BTNS_USE_INT	
	if(btn_start_scan == 0)
		btn_start_scan =1;   //开始扫描
#endif	
//	if(btn_start_scan ) //外部（按下和松开都会触发）中断触发后，不为0.
	{	
		matrix_keys_scan();
		
//		for(i=0;i<ROW_NUM*COL_NUM;i++)
//		{
//			if(g_btns_info.reportEn[i])
//			{
//				g_btns_info.reportEn[i] = 0;    //清零需要上报标志
//				send_btn_change_to_cpu(i,g_btns_info.value[i]); //发送按键按下/松开
//				printf("++++btn:%d %s\r\n",i,g_btns_info.value[i]==1?"press":"release");
//			}
//		}

#ifdef 	BTNS_USE_INT	
		exti_interrupt_enable(EXTI_12);   //扫描完毕之后再使能		
#endif
//			//printf("release\r\n");
//		}//if(ret)			
//		ret = matrix_keys_scan();		
//		if(ret && ret < 37) //有按键被按下
//		{	
//			if(btn_press_num == 0)   //还没有记录该按键值
//			{
//				scan_num++;
//				if(scan_num>1)  //至少扫描到两次，用于消抖
//				{					
//					btn_press_num = ret;
//					send_btn_change_to_cpu(btn_press_num,1); //发送按键按下
//					//可以在这个位置上报按键状态
//					MY_PRINTF("task1_btn_scan ret = %d\r\n",ret);					
//				}
//			}
//		}
//		else if(ret==0) //按键已松开
//		{
//			btn_start_scan = 0;
//			if(btn_press_num)
//			{
//				send_btn_change_to_cpu(btn_press_num,0);  //发送按键松开
//				btn_press_num = 0;	  //松开后，按键值为0
//				scan_num = 0;	 //扫描次数清零		
//				MY_PRINTF("release\r\n");
//			}
//			
//#ifdef 	BTNS_USE_INT	
//			exti_interrupt_enable(EXTI_12);   //扫描完毕之后再使能		
//#endif
//			//printf("release\r\n");
//		}//if(ret)		
	}
	//else
		//;//scan_num = 0;     //可以省略吧？？
}


#endif