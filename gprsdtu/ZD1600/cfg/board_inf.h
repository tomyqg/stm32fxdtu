/******************************* (C) ZDC ***********************************
* File Name          : uart_drv.h
* Author             : kidss
* Date               : 2010-09-04
* Version            : 1.0
* Description        : board information
******************************************************************************/
#ifndef	__BOARD_INF_H__
#define	__BOARD_INF_H__
#include "stm32f10x_type.h"


#define	STM32F107XX
/*	board hardware information */


/*stm32f105\107xx*/
#ifdef	STM32F107XX
/*USART*/
/*USART1*/
#define ZD1600_COM1                   USART1
#define ZD1600_COM1_GPIO              GPIOA
#define ZD1600_COM1_CLK               RCC_APB2Periph_USART1
#define ZD1600_COM1_GPIO_CLK          RCC_APB2Periph_GPIOA
#define ZD1600_COM1_RxPin             GPIO_Pin_10
#define ZD1600_COM1_TxPin             GPIO_Pin_9
/*USART1 Remap*/

/*USART2*/
/* stm3210c board usart2 use remap
#define ZD1600_COM2                   USART2
#define ZD1600_COM2_GPIO              GPIOA
#define ZD1600_COM2_CLK               RCC_APB1Periph_USART2
#define ZD1600_COM2_GPIO_CLK          RCC_APB2Periph_GPIOA
#define ZD1600_COM2_RxPin             GPIO_Pin_3
#define ZD1600_COM2_TxPin             GPIO_Pin_2
*/
/*USART2 Remap*/
#define ZD1600_COM2                   USART2
#define ZD1600_COM2_GPIO              GPIOD
#define ZD1600_COM2_CLK               RCC_APB1Periph_USART2
#define ZD1600_COM2_GPIO_CLK          RCC_APB2Periph_GPIOD
#define ZD1600_COM2_RxPin             GPIO_Pin_6
#define ZD1600_COM2_TxPin             GPIO_Pin_5

/*USART3*/
#define ZD1600_COM3                   USART3
#define ZD1600_COM3_GPIO              GPIOB
#define ZD1600_COM3_CLK               RCC_APB1Periph_USART3
#define ZD1600_COM3_GPIO_CLK          RCC_APB2Periph_GPIOB
#define ZD1600_COM3_RxPin             GPIO_Pin_11
#define ZD1600_COM3_TxPin             GPIO_Pin_10
/*USART3 Remap*/

#endif

#ifdef	STM32F103XX
/*USART*/
/*USART1*/
#define ZD1600_COM1                   USART1
#define ZD1600_COM1_GPIO              GPIOA
#define ZD1600_COM1_CLK               RCC_APB2Periph_USART1
#define ZD1600_COM1_GPIO_CLK          RCC_APB2Periph_GPIOA
#define ZD1600_COM1_RxPin             GPIO_Pin_10
#define ZD1600_COM1_TxPin             GPIO_Pin_9
/*USART1 Remap*/

/*USART2*/
#define ZD1600_COM2                   USART2
#define ZD1600_COM2_GPIO              GPIOA
#define ZD1600_COM2_CLK               RCC_APB1Periph_USART2
#define ZD1600_COM2_GPIO_CLK          RCC_APB2Periph_GPIOA
#define ZD1600_COM2_RxPin             GPIO_Pin_3
#define ZD1600_COM2_TxPin             GPIO_Pin_2
/*USART2 Remap*/
/*#define ZD1600_COM2                   USART2
#define ZD1600_COM2_GPIO              GPIOD
#define ZD1600_COM2_CLK               RCC_APB1Periph_USART2
#define ZD1600_COM2_GPIO_CLK          RCC_APB2Periph_GPIOD
#define ZD1600_COM2_RxPin             GPIO_Pin_6
#define ZD1600_COM2_TxPin             GPIO_Pin_5
*/
/*USART3*/
#define ZD1600_COM3                   USART3
#define ZD1600_COM3_GPIO              GPIOB
#define ZD1600_COM3_CLK               RCC_APB1Periph_USART3
#define ZD1600_COM3_GPIO_CLK          RCC_APB2Periph_GPIOB
#define ZD1600_COM3_RxPin             GPIO_Pin_11
#define ZD1600_COM3_TxPin             GPIO_Pin_10
/*USART3 Remap*/

#endif









#endif

