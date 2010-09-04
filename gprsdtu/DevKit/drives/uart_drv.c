/******************************* (C) Embest ***********************************
* File Name          : uart_drv.c
* Author             : tary
* Date               : 2009-06-25
* Version            : 0.1
* Description        : (¶à)´®¿ÚÇý¶¯
******************************************************************************/

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include "uart_hw.h"
#include "uart_std.h"
#include "uart_drv.h"

/* Variables ----------------------------------------------------------------*/


/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
static int uart_conf(uart_dev_t* udev) {
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable uart_flds[udev->port_nr].port clocks */
	if (udev->port_nr == 1) {
		RCC_APB2PeriphClockCmd(uart_flds[udev->port_nr].rcc, ENABLE);
	} else {
		RCC_APB1PeriphClockCmd(uart_flds[udev->port_nr].rcc, ENABLE);
	}

	/* uart_flds[udev->port_nr].port configured as follow:
	  	  - BaudRate = BaudRate baud	
	  	  - Word Length = 8 Bits
	  	  - One Stop Bit
	  	  - No parity
	  	  - Hardware flow control disabled (RTS and CTS signals)
	  	  - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = udev->baud_rate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	/* Configure the uart_flds[udev->port_nr].port */ 
	USART_Init(uart_flds[udev->port_nr].port, &USART_InitStructure);

#if F_USE_UCOS
	/* Enable UART interrupt IRQ channel */
	NVIC_InitStructure.NVIC_IRQChannel = uart_flds[udev->port_nr].rx_irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = uart_flds[udev->port_nr].rx_prio;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable recv irq*/
	USART_ITConfig(uart_flds[udev->port_nr].port, USART_IT_RXNE, ENABLE);
#endif
	/* Enable the uart_flds[udev->port_nr].port */
	USART_Cmd(uart_flds[udev->port_nr].port, ENABLE);

	return 0;
}

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
#if F_USE_UCOS
void* uart_isr(void* arg) {
	uart_dev_t* udev;
	unsigned ch;

	udev = (uart_dev_t*) arg;
	if (USART_GetFlagStatus(uart_flds[udev->port_nr].port, USART_FLAG_RXNE) != RESET) {
		ch = (u8) USART_ReceiveData(uart_flds[udev->port_nr].port);
		OSQPost(udev->rcv_evt, (void *)ch);
	}

	return NULL;
}
#endif

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int uart_pend_abort(uart_dev_t* udev) {
	INT8U err;
	// INT8U w_nr;

	// return # of tasks waiting on the udev->rcv_evt
	// w_nr = 
	OSQPendAbort(udev->rcv_evt, OS_PEND_OPT_NONE, &err);
	if (err != OS_ERR_NONE) {
		return -1;
	}	

	return 0;
}

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int uart_flush(uart_dev_t* udev) {
	INT8U r;

	r = OSQFlush(udev->rcv_evt);
	if (r != OS_ERR_NONE) {
		return -1;
	}
	return 0;
}

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int uart_init (uart_dev_t* udev) {

	if (udev == NULL) {
		return -1;
	}

	udev->rcv_isr = uart_isr;

	uart_conf(udev);

	if (udev->port_init != NULL) {
		udev->port_init();
	}

#if F_USE_UCOS
	udev->rcv_evt = OSQCreate(udev->rcv_que, UART_RCV_Q_SIZE);
	if (udev->rcv_evt == (OS_EVENT *)0) {
		return -1;
	}
#endif
	return 0;
}

/*=============================================================================
* Function	: 
* Description	: Write character to Serial Port
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int uart_send_ch(uart_dev_t* udev, int ch) {
	USART_SendData(uart_flds[udev->port_nr].port, (u8) ch);

	/* Loop until the end of transmission */
	while(USART_GetFlagStatus(uart_flds[udev->port_nr].port, USART_FLAG_TXE) == RESET) {
		;
	}
	return (ch);
}

/*=============================================================================
* Function	: 
* Description	: Read character from Serial Port
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int uart_get_key(uart_dev_t* udev) {
#if F_USE_UCOS
	void *r;
	INT8U err;

	r = OSQPend(udev->rcv_evt, 0, &err);
	if (err != OS_ERR_NONE) {
		return EOF;
	}
	return (int)r;
#else
	while(USART_GetFlagStatus(uart_flds[udev->port_nr].port, USART_FLAG_RXNE) == RESET) {
		;
	}
	return (USART_ReceiveData(uart_flds[udev->port_nr].port));
#endif
}

/*=============================================================================
* Function	: 
* Description	: Read character from Serial Port width msecs milliseconds
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int uart_wait_key(uart_dev_t* udev, int msecs) {
#if F_USE_UCOS
	void *r;
	INT8U err;

	r = OSQPend(udev->rcv_evt, msecs * OS_TICKS_PER_SEC / 1000, &err);
	if (err != OS_ERR_NONE) {
		return EOF;
	}
	return (int)r;
#else
	while(USART_GetFlagStatus(uart_flds[udev->port_nr].port, USART_FLAG_RXNE) == RESET) {
		;
	}
	return (USART_ReceiveData(uart_flds[udev->port_nr].port));
#endif
}

/*=============================================================================
* Function	: 
* Description	: Read character but no wait if no key
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int uart_peek_key(uart_dev_t* udev) {
#if F_USE_UCOS
	void *r;
	INT8U err;

	r = OSQAccept(udev->rcv_evt, &err);
	if (err != OS_ERR_NONE) {
		return EOF;
	}
	return (int)r;
#else
	if (USART_GetFlagStatus(uart_flds[udev->port_nr].port, USART_FLAG_RXNE) == RESET) {
		return EOF;
	}
	return (USART_ReceiveData(uart_flds[udev->port_nr].port));
#endif
}

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int uart_gets(uart_dev_t* udev, char *lin, int n)	{
	int cnt = 0;
	char c;

	do {
		if ((c = uart_get_key(udev)) == UART_CR) {
			c = UART_LF;				/* read character */
		}

		if (1
		  && (c != UART_BACKSPACE)			/* process backspace */
		  && (c != UART_DEL)
		) {
			cnt++;
			*lin++ = c;
			if (c != UART_LF) {			/* echo and store character */
				uart_send_ch(udev, c);
			}
		} else if (cnt != 0) {
			cnt--;
			lin--;
			uart_send_ch(udev, UART_BACKSPACE);		/* echo backspace */
			uart_send_ch(udev, ' ');
			uart_send_ch(udev, UART_BACKSPACE);
		}
	} while (cnt < n - 1 &&	c != UART_LF);			/* check limit and line feed */
	*lin = 0;						/* mark end of string */
	return cnt;
}

/************************************END OF FILE******************************/
