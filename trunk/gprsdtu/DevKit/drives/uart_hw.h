/******************************* (C) Embest ***********************************
* File Name          : uart_hw.h
* Author             : tary
* Date               : 2009-06-30
* Version            : 0.1
* Description        : 串口硬件定义
******************************************************************************/

#ifndef __UART_HW_H__
#define __UART_HW_H__

#include "stm32f10x_lib.h"

#define STD_UART_RCC			RCC_APB2Periph_USART1
#define STD_UART_NR			USART1
#define STD_UART_RCC_GPX		RCC_APB2Periph_GPIOA
#define STD_UART_PORT			GPIOA
#define STD_UART_RXPIN			GPIO_Pin_10
#define STD_UART_TXPIN			GPIO_Pin_9

typedef struct _uart_hw_t_tag_ {
	USART_TypeDef* port;
	u32 rcc;
	u16 flags;
	u8 rx_irq;
	u8 rx_prio;
	// u8 tx_irq;
	// u8 tx_prio;
} uart_hw_t;

extern const uart_hw_t uart_flds[];
extern const int UART_FLDS_CNT;

int uart1_gpio_init(void);
int uart2_gpio_init(void);


#endif //__UART_HW_H__

/************************************END OF FILE******************************/
