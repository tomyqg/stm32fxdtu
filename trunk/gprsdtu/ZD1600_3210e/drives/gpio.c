/******************************* (C) Embest ***********************************
* File Name          : gpio.h
* Author             : tary 
* Date               : 2009-02-18
* Version            : 0.1
* Description        : 
******************************************************************************/
#include "gpio.h"


/*=============================================================================
* Function	: gpio_init
* Description	: 初使化某个GPIO引脚，用于节省空间
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int gpio_init(
	GPIO_TypeDef* port,
	u16 pin,
	GPIOSpeed_TypeDef speed,
	GPIOMode_TypeDef mode,
	u32 apb_clk
) {
	GPIO_InitTypeDef gpio_init_buf[1],
	 * gpio_init = &gpio_init_buf[0];

	if (apb_clk != 0) {
		RCC_APB2PeriphClockCmd(apb_clk, ENABLE);
	}

	gpio_init->GPIO_Pin = pin;
	gpio_init->GPIO_Speed = speed;
	gpio_init->GPIO_Mode = mode;
	GPIO_Init(port, gpio_init);

	return 0;
}

/************************************END OF FILE******************************/
