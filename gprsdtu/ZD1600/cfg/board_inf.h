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

/****************************************************************************** 
stm32f105\107xx
******************************************************************************/
#ifdef	STM32F107XX
/*FLASH*/												 
//flash page size	 stm32f107vc
#define FLASH_PAGE_SIZE    ((u16)0x800)
//#define FLASH_PAGE_COUNTER		2
//#define FLASH_STORAGE_SIZE			(FLASH_PAGE_COUNTER * FLASH_PAGE_SIZE)
#define FLASH_STORAGE_SIZE			2048
#define FLASH_STORAGE_HEAD		((u32)0x08019000)
#define FLASH_STORAGE_END			(FLASH_STORAGE_HEAD + FLASH_STORAGE_SIZE)




/*USART*/
//USART1
#define ZD1600_COM1                   USART1
#define ZD1600_COM1_GPIO              GPIOA
#define ZD1600_COM1_CLK               RCC_APB2Periph_USART1
#define ZD1600_COM1_GPIO_CLK          RCC_APB2Periph_GPIOA
#define ZD1600_COM1_RxPin             GPIO_Pin_10
#define ZD1600_COM1_TxPin             GPIO_Pin_9
/*USART1 Remap*/

//USART2
/* stm3210c board usart2 use remap
#define ZD1600_COM2                   USART2
#define ZD1600_COM2_GPIO              GPIOA
#define ZD1600_COM2_CLK               RCC_APB1Periph_USART2
#define ZD1600_COM2_GPIO_CLK          RCC_APB2Periph_GPIOA
#define ZD1600_COM2_RxPin             GPIO_Pin_3
#define ZD1600_COM2_TxPin             GPIO_Pin_2
*/
//USART2 Remap
#define ZD1600_COM2                   USART2
#define ZD1600_COM2_GPIO              GPIOD
#define ZD1600_COM2_CLK               RCC_APB1Periph_USART2
#define ZD1600_COM2_GPIO_CLK          RCC_APB2Periph_GPIOD
#define ZD1600_COM2_RxPin             GPIO_Pin_6
#define ZD1600_COM2_TxPin             GPIO_Pin_5

//USART3
#define ZD1600_COM3                   USART3
#define ZD1600_COM3_GPIO              GPIOB
#define ZD1600_COM3_CLK               RCC_APB1Periph_USART3
#define ZD1600_COM3_GPIO_CLK          RCC_APB2Periph_GPIOB
#define ZD1600_COM3_RxPin             GPIO_Pin_11
#define ZD1600_COM3_TxPin             GPIO_Pin_10
//USART3 Remap

#endif





/****************************************************************************** 
STM32F103XX
******************************************************************************/
#ifdef	STM32F103XX
/*FLASH*/
//flash page size	 stm32f103ze
#define FLASH_PAGE_SIZE    ((u16)0x800)
//#define FLASH_PAGE_COUNTER		2
//#define FLASH_STORAGE_SIZE			(FLASH_PAGE_COUNTER * FLASH_PAGE_SIZE)
#define FLASH_STORAGE_SIZE			2048
#define FLASH_STORAGE_HEAD		((u32)0x08019000)
#define FLASH_STORAGE_END			(FLASH_STORAGE_HEAD + FLASH_STORAGE_SIZE)










/*USART*/
//USART1
#define ZD1600_COM1                   USART1
#define ZD1600_COM1_GPIO              GPIOA
#define ZD1600_COM1_CLK               RCC_APB2Periph_USART1
#define ZD1600_COM1_GPIO_CLK          RCC_APB2Periph_GPIOA
#define ZD1600_COM1_RxPin             GPIO_Pin_10
#define ZD1600_COM1_TxPin             GPIO_Pin_9
//USART1 Remap

//USART2
#define ZD1600_COM2                   USART2
#define ZD1600_COM2_GPIO              GPIOA
#define ZD1600_COM2_CLK               RCC_APB1Periph_USART2
#define ZD1600_COM2_GPIO_CLK          RCC_APB2Periph_GPIOA
#define ZD1600_COM2_RxPin             GPIO_Pin_3
#define ZD1600_COM2_TxPin             GPIO_Pin_2
//USART2 Remap
/*#define ZD1600_COM2                   USART2
#define ZD1600_COM2_GPIO              GPIOD
#define ZD1600_COM2_CLK               RCC_APB1Periph_USART2
#define ZD1600_COM2_GPIO_CLK          RCC_APB2Periph_GPIOD
#define ZD1600_COM2_RxPin             GPIO_Pin_6
#define ZD1600_COM2_TxPin             GPIO_Pin_5
*/
//USART3
#define ZD1600_COM3                   USART3
#define ZD1600_COM3_GPIO              GPIOB
#define ZD1600_COM3_CLK               RCC_APB1Periph_USART3
#define ZD1600_COM3_GPIO_CLK          RCC_APB2Periph_GPIOB
#define ZD1600_COM3_RxPin             GPIO_Pin_11
#define ZD1600_COM3_TxPin             GPIO_Pin_10
//USART3 Remap

#endif









#endif

