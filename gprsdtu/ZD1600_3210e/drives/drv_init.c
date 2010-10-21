/******************************* (C) Embest ***********************************
* File Name          : drv_init.c
* Author             : tary
* Date               : 2009-05-27
* Version            : 0.2u
* Description        : 
******************************************************************************/
#include <stdio.h>
#include <ucos_ii.h>
#include "drv_init.h"
#include "uart_drv.h"


//#define STM3210C //test
//#include "../app/test.h"	//test
#ifdef STM3210C
#include "3210c_rcc.h"
#endif


/*=============================================================================
* Function	: NVIC_Configuration
* Description	: �ж��������ַ������������   NVIC_init
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
#ifdef  VECT_TAB_RAM  
	/* Set the Vector Table base location at 0x20000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 

#else  /* VECT_TAB_FLASH  */												
	/* Set the Vector Table base location at __Vectors in vector.s */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);   
#endif

	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);



	NVIC_StructInit(&NVIC_InitStructure);
	/* Enable USART1 USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


//	return 0;
}

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
void RCC_Configuration(void)
{

	ErrorStatus HSEStartUpStatus;

	/* RCC system reset(for debug purpose) */
	RCC_DeInit();
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus == SUCCESS)
	{
		/* HCLK = SYSCLK */
		RCC_HCLKConfig(RCC_SYSCLK_Div1); 
		/* PCLK2 = HCLK */
		RCC_PCLK2Config(RCC_HCLK_Div1); 
		/* PCLK1 = HCLK/2 */
		RCC_PCLK1Config(RCC_HCLK_Div2);
		/* Flash 2 wait state */
		FLASH_SetLatency(FLASH_Latency_2);
		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

#ifdef STM3210C

	rcc_pll_set();		
#else

		/* PLLCLK = 8MHz * 9 = 72 MHz */
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
#endif
		/* Enable PLL */ 
		RCC_PLLCmd(ENABLE);
		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}
		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}
      /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
//  PWR_BackupAccessCmd(ENABLE);
//  /* Reset Backup Domain */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}
  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);  


}

void uart_init(void)
{
	COM_Conf_T conf;

	conf.com = COM2;
	conf.BaudRate = 9600;
	conf.WordLength = WL_8b;
	conf.StopBits = SB_1;
	conf.Parity = No;
	conf.HwFlowCtrl = None;
	ZD1600_COMInit(&conf);

	conf.com = COM1;
	conf.BaudRate = 115200;
	ZD1600_COMInit(&conf);
	
}
void systick_init(void)
{
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
}


/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
void drv_all_init(void)
{
	ENABLE_IRQ();

	led_init();
	led_on(2);

	uart_init();

	systick_init();



	

//	hwtm_init();
//	key_init();
//	rtc_init();
	// SRAM_Init();

	/* Initialize the LCD */
//	STM3210E_LCD_Init();

//	adc_init();

//	return 0;
}

/************************************END OF FILE******************************/

