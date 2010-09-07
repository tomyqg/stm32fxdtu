/******************************* (C) Embest ***********************************
* File Name          : systick.c
* Author             : tary 
* Date               : 2009-02-26
* Version            : 0.1
* Description        : generate the system clock and sleep time
******************************************************************************/

#include "stm32f10x_lib.h"
#include "systick.h"
//#include "vtimer.h"
#include <stdio.h>


/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
int systick_period = 0;
volatile unsigned long ticks = 0;

/* Private function prototypes ----------------------------------------------*/
/* Private functions --------------------------------------------------------*/
/*****************************************************************************/
int systick_init(int period_ms) {
	RCC_ClocksTypeDef RCCBUF[1], *RCC_Clocks = &RCCBUF[0];
	int systick_clk = 0;
	
	RCC_GetClocksFreq(RCC_Clocks);

	systick_clk = RCC_Clocks->HCLK_Frequency / 8;

	/* SysTick event each 10 ms with input clock equal to 9MHz (HCLK/8)  */
	SysTick_SetReload(systick_clk * period_ms / 1000);

	SysTick_ITConfig(ENABLE);             /* Enable SysTick interrupt    */

	systick_period = period_ms;

	return 0;
}

int systick_start(void) {
	/* Enable the SysTick Counter                                        */
	SysTick_CounterCmd(SysTick_Counter_Enable);
	return 0;
}

int systick_stop(void) {
	/* Disable the SysTick Counter                                       */
	SysTick_CounterCmd(SysTick_Counter_Disable);
	return 0;
}

// only used in single thread mode
int sys_timeout(unsigned long *start_ms, int m_secs) {
	//static unsigned long start_ms;

	if (start_ms == NULL) {
		return ticks * systick_period > m_secs;
	}

	if (m_secs == 0) {
		*start_ms = ticks * systick_period;
		return 0;
	}
	return (ticks * systick_period - *start_ms) > m_secs;
}

int sleep(int m_secs) {
	unsigned long goal;

	goal = m_secs + ticks * systick_period;
	while (goal > ticks * systick_period) {
		;
	}
	return 0;
}

int systick_isr(void) {
	void xPortSysTickHandler( void );
	
	++ticks;

	xPortSysTickHandler();
	// MSTimerService(ticks, systick_period);
	return 0;
}

/************************************END OF FILE******************************/
