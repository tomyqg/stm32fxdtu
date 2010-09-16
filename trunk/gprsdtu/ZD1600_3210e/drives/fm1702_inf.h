/******************************* (C) Embest ***********************************
* File Name          : fm1702_inf.h
* Author             : tary
* Date               : 2009-06-25
* Version            : 0.4u
* Description        : fm1702 device low level operations 
******************************************************************************/

#ifndef __FM1702_INF_H__
#define __FM1702_INF_H__

#include "gpio.h"
#include "systick.h"
#include "aux_lib.h"


/* 需要根据具体的控制引脚来定义 */ 
#define RCC_APB2Periph_NSS	RCC_APB2Periph_GPIOB
#define FM1702_NSS_PORT		GPIOB
#define FM1702_NSS_PIN		GPIO_Pin_12

/* 需要根据具体的控制引脚来定义 */ 
#define RCC_APB2Periph_RST	RCC_APB2Periph_GPIOG
#define FM1702_RST_PORT		GPIOG
#define FM1702_RST_PIN		GPIO_Pin_11

#define FM1702_NSS_HIGH()	GPIO_SetBits(FM1702_NSS_PORT, FM1702_NSS_PIN)
#define FM1702_NSS_LOW()	GPIO_ResetBits(FM1702_NSS_PORT, FM1702_NSS_PIN)

//注意有三极管反相
#define FM1702_RST_LOW()	GPIO_SetBits(FM1702_RST_PORT, FM1702_RST_PIN)
#define FM1702_RST_HIGH()	GPIO_ResetBits(FM1702_RST_PORT, FM1702_RST_PIN)

//初使化相关的操作
int fm1702_inf_init(void);

u8 fm1702_inf_rwbyte(u8 bSnd);

//接口读写
int FM1702_GET_REG (int reg);
int FM1702_SET_REG (int reg, int val);

int fm1702_inf_read(int reg, char* buf, int len);
int fm1702_inf_write(int reg, char* buf, int len);

int fm1702_timeout(unsigned long *stm, int msec);

#endif //__FM1702_INF_H__

/************************************END OF FILE******************************/
