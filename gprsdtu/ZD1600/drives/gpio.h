/******************************* (C) Embest ***********************************
* File Name          : gpio.h
* Author             : tary 
* Date               : 2009-02-18
* Version            : 0.1
* Description        : 
******************************************************************************/

#ifndef __GPIO_H__
#define __GPIO_H__

#include "stm32f10x_lib.h"

int gpio_init(
	GPIO_TypeDef* port,
	u16 pin,
	GPIOSpeed_TypeDef speed,
	GPIOMode_TypeDef mode,
	u32 apb_clk
);

#endif //__GPIO_H__

/************************************END OF FILE******************************/
