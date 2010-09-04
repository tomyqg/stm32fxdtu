/******************************* (C) Embest ***********************************
* File Name          : uart_hw.c
* Author             : tary
* Date               : 2009-06-30
* Version            : 0.1
* Description        : 串口硬件定义
******************************************************************************/

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include "gpio.h"
#include "aux_lib.h"
#include "uart_std.h"
#include "uart_hw.h"

/* Variables ----------------------------------------------------------------*/

const uart_hw_t uart_flds[] = {
	{
	NULL,
	0,
	0,
	0,
	0
	},

#ifdef USART1
	{
	USART1,
	RCC_APB2Periph_USART1,
	0,
	USART1_IRQChannel,
	0
	},
#endif

#ifdef USART2
	{
	USART2,
	RCC_APB1Periph_USART2,
	0,
	USART2_IRQChannel,
	0
	},
#endif

#ifdef USART3
	{
	USART3,
	RCC_APB1Periph_USART3,
	0,
	USART3_IRQChannel,
	0
	},
#endif

#ifdef UART4
	{
	UART4,
	RCC_APB1Periph_UART4,
	0,
	UART4_IRQChannel,
	0
	},
#endif

#ifdef UART5
	{
	UART5,
	RCC_APB1Periph_UART5,
	0,
	UART5_IRQChannel,
	0
	},
#endif

};


const int UART_FLDS_CNT = countof(uart_flds);

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int uart1_gpio_init(void) {

	/* Enable GPIOx clock */
	/* Configure STD_UART_TXPIN as alternate function push-pull */
	gpio_init(STD_UART_PORT,
	  STD_UART_TXPIN, GPIO_Speed_50MHz, GPIO_Mode_AF_PP,
	  STD_UART_RCC_GPX
	);

	/* Configure STD_UART_RXPIN as input floating */
	gpio_init(STD_UART_PORT,
	  STD_UART_RXPIN, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING,
	  0
	);
	return 0;
}

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int uart2_gpio_init(void) {

	/* Enable GPIOx clock */
	/* Configure STD_UART_TXPIN as alternate function push-pull */
	gpio_init(GPIOA,
	  GPIO_Pin_2, GPIO_Speed_50MHz, GPIO_Mode_AF_PP,
	  RCC_APB2Periph_GPIOA
	);

	/* Configure STD_UART_RXPIN as input floating */
	gpio_init(GPIOA,
	  GPIO_Pin_3, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING,
	  0
	);
	return 0;

}

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int uart1_isr(void) {
	udev_std->rcv_isr(udev_std);
	return 0;
}

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
#if 0
int uart2_isr(void) {
	udev_cdrs->rcv_isr(udev_cdrs);
	return 0;
}
#endif

/************************************END OF FILE******************************/
