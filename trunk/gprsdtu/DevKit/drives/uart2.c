/******************************* (C) Embest ***********************************
* File Name          : uart.c
* Author             : tary
* Date               : 2009-05-23
* Version            : 0.2
* Description        : Low Level UART Routines
******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <ucos_ii.h>
#include "gpio.h"
#include "uart2.h"
#define     F_USE_UCOS      1  

#if F_USE_UCOS
#define QRECVSIZE			1024
static OS_EVENT *EventUSART2;
static void *qRecv[QRECVSIZE];
#endif
static int UART2_GPIO_Configuration(void) {

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

static int USART2_Configuration(int BaudRate) {
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable USART2 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* USART2 configured as follow:
	  	  - BaudRate = BaudRate baud	
	  	  - Word Length = 8 Bits
	  	  - One Stop Bit
	  	  - No parity
	  	  - Hardware flow control disabled (RTS and CTS signals)
	  	  - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	/* Configure the USART2 */ 
	USART_Init(USART2, &USART_InitStructure);

#if F_USE_UCOS
	/* Enable UART interrupt IRQ channel */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable recv irq*/
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
#endif
	/* Enable the USART2 */
	USART_Cmd(USART2, ENABLE);

	return 0;
}

int uart2_init (int BaudRate) {

    UART2_GPIO_Configuration();
	USART2_Configuration(BaudRate);
	
#if F_USE_UCOS
	EventUSART2 = OSQCreate(qRecv, QRECVSIZE);
	if (EventUSART2 == (OS_EVENT *)0) {
		return -1;
	}
#endif
	return 0;
}

#if F_USE_UCOS
void uart2_isr(void) {
	unsigned int ch;

	if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET) {
		ch = 0xff & USART_ReceiveData(USART2);
		OSQPost(EventUSART2, (void *)ch);
	}
}
#endif

/* Write character to Serial Port	   */
int uart2_send_ch(int ch) {
	USART_SendData(USART2, (u8) ch);

	/* Loop until the end of transmission */
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) {
		;
	}
	return (ch);
}

/* Read character from Serial Port	  */
int uart2_get_key(int ms) {
#if F_USE_UCOS
	void *ret;
	INT8U err;

	ret = OSQPend(EventUSART2, ms, &err);
	if (err != OS_ERR_NONE) {
		return EOF;
	}
	return (int)ret;
#else
	while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET) {
		;
	}
	return (USART_ReceiveData(USART2));
#endif
}

/* Read character but no wait if no key */
int uart2_peek_key(void) {
#if F_USE_UCOS
	void *ret;
	INT8U err;

	ret = OSQAccept(EventUSART2, &err);
	if (err != OS_ERR_NONE) {
		return EOF;
	}
	return (int)ret;
#else
	if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET) {
		return EOF;
	}
	return (USART_ReceiveData(USART2));
#endif
}

void uart2_read_abort(void)
{
    INT8U err;
    OSQPendAbort(EventUSART2, OS_PEND_OPT_NONE, &err);
}
void uart2_flush(void)
{
    OSQFlush(EventUSART2);
}
/************************************END OF FILE******************************/
