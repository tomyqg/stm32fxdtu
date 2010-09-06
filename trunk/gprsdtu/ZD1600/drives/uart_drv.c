/******************************* (C) ZDC ***********************************
* File Name          : uart_drv.c
* Author             : kidss
* Date               : 2010-09-04
* Version            : 1.0
* Description        : UART Driver
******************************************************************************/
#define NULL 0
#include "uart_drv.h"
#include "board_inf.h"
#include "gpio.h"


Uart_Buff_T	uart1_buff, uart2_buff;
u32	usart1_send_position, usart2_send_position;


/*usart hardware define*/
USART_TypeDef* COM_USART[COMn] = {	ZD1600_COM1, 
									ZD1600_COM2, 
									ZD1600_COM3 
								//	ZD1600_COM4, 
								//	ZD1600_COM5
									}; 

GPIO_TypeDef* COM_PORT[COMn] = {	ZD1600_COM1_GPIO, 
									ZD1600_COM2_GPIO, 
									ZD1600_COM3_GPIO 
								//	ZD1600_COM4_GPIO, 
								//	ZD1600_COM5_GPIO
									};

const u32 COM_USART_CLK[COMn] = {	ZD1600_COM1_CLK, 
									ZD1600_COM2_CLK, 
									ZD1600_COM3_CLK 
							//		ZD1600_COM4_CLK, 
							//		ZD1600_COM5_CLK
									};

const u32 COM_POR_CLK[COMn] = {	ZD1600_COM1_GPIO_CLK,
								ZD1600_COM2_GPIO_CLK,
								ZD1600_COM3_GPIO_CLK
						//		ZD1600_COM4_GPIO_CLK,
						//		ZD1600_COM5_GPIO_CLK
								};

const u16 COM_TX_PIN[COMn] = {	ZD1600_COM1_TxPin, 
								ZD1600_COM2_TxPin, 
								ZD1600_COM3_TxPin 
							//	ZD1600_COM4_TxPin, 
							//	ZD1600_COM5_TxPin
								};

const u16 COM_RX_PIN[COMn] = {	ZD1600_COM1_RxPin, 
								ZD1600_COM2_RxPin, 
								ZD1600_COM3_RxPin 
							//	ZD1600_COM4_RxPin, 
							//	ZD1600_COM5_RxPin
								};

/*usart configuration*/
const u16 COM_WordLength[2] = {	((u16)0x0000), 
								((u16)0x1000) 
								};
const u16 COM_StopBits[4] = {	((u16)0x0000), 
								((u16)0x1000), 
								((u16)0x2000), 
								((u16)0x3000) 
								};
const u16 COM_Parity[3] = {	((u16)0x0000), 
							((u16)0x0400), 
							((u16)0x0600) 
							};
const u16 COM_HwFlowControl[4] = {	((u16)0x0000),
									((u16)0x0100), 
									((u16)0x0200), 
									((u16)0x0300), 
									};









void ZD1600_COMInit(COM_Conf_T *uart_conf)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(COM_POR_CLK[uart_conf->com] | RCC_APB2Periph_AFIO, ENABLE);

	/* Enable UART clock */
	if (uart_conf->com == COM1){
		RCC_APB2PeriphClockCmd(COM_USART_CLK[uart_conf->com], ENABLE); 
		}else{
		RCC_APB1PeriphClockCmd(COM_USART_CLK[uart_conf->com], ENABLE);
		}
	
	/*UART Pin Remap*/
 	//test
 	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);	 	//stm3210c board usart2 use remap
	
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[uart_conf->com];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(COM_PORT[uart_conf->com], &GPIO_InitStructure);
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[uart_conf->com];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(COM_PORT[uart_conf->com], &GPIO_InitStructure);

	/* USART configuration */
	USART_InitStructure.USART_BaudRate = uart_conf->BaudRate;
	USART_InitStructure.USART_WordLength = COM_WordLength[uart_conf->WordLength];
	USART_InitStructure.USART_StopBits = COM_StopBits[uart_conf->StopBits];;
	USART_InitStructure.USART_Parity = COM_Parity[uart_conf->Parity];;
	USART_InitStructure.USART_HardwareFlowControl = COM_HwFlowControl[uart_conf->HwFlowCtrl];;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
//	USART_InitStructure.USART_Clock = USART_Clock_Disable;
//	USART_InitStructure.USART_CPOL = USART_CPOL_Low;
//	USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
//	USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
	USART_Init(COM_USART[uart_conf->com], &USART_InitStructure);
	    
	/* Enable USART */
	USART_Cmd(COM_USART[uart_conf->com], ENABLE);

}

/*uart接收中断设置（中断接收方式）*/
void uart_rx_itconf(COM_TypeDef comx, FunctionalState newstate)
{
	USART_ITConfig(COM_USART[comx], USART_IT_RXNE, newstate);		
}
/*uart接收buffer设置*/
u32* uart_rx_bufset(COM_TypeDef comx, u8 *buf, u32 maxlen)
{
	if(comx == COM1){
		uart1_buff.rxbuf = buf;
		uart1_buff.rxbuflen_max = maxlen;
		/*返回buf数据长度地址*/
		return &uart1_buff.rxlen;
		}
	if(comx == COM2){
		uart2_buff.rxbuf = buf;
		uart2_buff.rxbuflen_max = maxlen;
		/*返回buf数据长度地址*/
		return &uart2_buff.rxlen;
		}
//	else if(comx == COM3){
		
//		}
	return NULL;
}
/*uart1收发中断*/
void uart1_rx_isr(void)
{
	if(uart1_buff.rxlen < uart2_buff.rxbuflen_max) {
		uart1_buff.rxbuf[uart1_buff.rxlen++] = USART_ReceiveData(ZD1600_COM1);
		}else{
		uart1_buff.rxlen = 0;
		}
}
void uart1_tx_isr(void)
{
	if(usart1_send_position < uart1_buff.txlen){	
		USART_SendData(ZD1600_COM1, uart1_buff.txbuf[usart1_send_position++]);
		}else{
		uart1_buff.txlen = 0;
		//dissable uart1 tx interrupt
		USART_ITConfig(ZD1600_COM1, USART_IT_TXE, DISABLE);
		}

}
/*uart2收发中断*/
void uart2_rx_isr(void)
{
	uart2_buff.rxbuf[uart2_buff.rxlen++] = USART_ReceiveData(ZD1600_COM2);
}
void uart2_tx_isr(void)
{
	if(usart2_send_position < uart2_buff.txlen){	
		USART_SendData(ZD1600_COM2, uart2_buff.txbuf[usart2_send_position++]);
		}else{
		uart2_buff.txlen = 0;
		//dissable uart2 tx interrupt
		USART_ITConfig(ZD1600_COM2, USART_IT_TXE, DISABLE);
		}
}

/*uart1发送数据 中断方式*/
void uart1_senddata(u8 *data, u32 len)
{
	uart1_buff.txbuf = data;
	uart1_buff.txlen = len;
	usart1_send_position = 0;
	//emable uart1 tx interrupt
	USART_ITConfig(ZD1600_COM1, USART_IT_TXE, ENABLE);
	//begin send
	if(usart1_send_position == 0){
		USART_SendData(ZD1600_COM1, uart1_buff.txbuf[usart1_send_position++]);
		}


}
/*uart2发送数据 中断方式*/
void uart2_senddata(u8 *data, u32 len)
{
	uart2_buff.txbuf = data;
	uart2_buff.txlen = len;
	usart2_send_position = 0;
	//emable uart2 tx interrupt
	USART_ITConfig(ZD1600_COM2, USART_IT_TXE, ENABLE);
	//begin send
	if(usart2_send_position == 0){
		USART_SendData(ZD1600_COM2, uart2_buff.txbuf[usart2_send_position++]);
		}
}
/**/










