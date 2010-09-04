/******************************* (C) Embest ***********************************
* File Name          : tmr_drv.c
* Author             : tary
* Date               : 2009-06-08
* Version            : 0.1
* Description        : 硬件定时器驱动
******************************************************************************/

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include "drv_init.h"
#include "tmr_drv.h"

/* Variables ----------------------------------------------------------------*/
vu16 tim_arr_val = 0x00;
func_none_arg_t hwtm_2nd_isr = NULL;

/******************************************************************************
* Function Name : hwtm_nvic_config
* Description   : Configure the TIM2s NVIC channel
* Input         : None
* Output        : None
* Return        : 0
******************************************************************************/
int hwtm_nvic_config(void) {
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM2 IRQ Channel configuration */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	return 0;
}

/******************************************************************************
* Function Name : hwtm_ctrl_config
* Description   : Initialize the TIM Controller
* Input         : None
* Output        : None
* Return        : 0
******************************************************************************/
int hwtm_ctrl_config(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	/* Enable TIM2 APB1 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_OCStructInit(&TIM_OCInitStructure);

	/* TIM2 used for timing, the timing period depends on wav file sample rate */
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00;	/* TIM2CLK = 72 MHz */
	TIM_TimeBaseStructure.TIM_Period = tim_arr_val;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	/* Channel 1 Configuration in Timing mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 0x0;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	TIM_Cmd(TIM2, DISABLE);

	return 0;
}

/******************************************************************************
* Function Name : hwtm_init
* Description   : Initialize the TIM Peripheral
* Input         : None
* Output        : None
* Return        : 0
******************************************************************************/
int hwtm_init(void) {

	hwtm_ctrl_config();

	hwtm_nvic_config();

	return 0;
}

/*=============================================================================
* Function	: hwtm_start
* Description	: 启动周期定时器
* Input         : None
* Output        : None
* Return        : 0
=============================================================================*/
int hwtm_start(unsigned rate) {
	RCC_ClocksTypeDef clks;

	RCC_GetClocksFreq(&clks);
	tim_arr_val = clks.PCLK2_Frequency / rate;

	/* Set autoreload value */
	TIM_SetAutoreload(TIM2, tim_arr_val);

	/* Clear timer counter */
	TIM_SetCounter(TIM2, 0);

	/* Start TIM2 */
	TIM_Cmd(TIM2, ENABLE);

	/* Enable TIM2 interrupt used for sound wave amplitude update */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	// printf("\r\ntim_arr_val =\t\t\t%d", tim_arr_val);

	return 0;
}

/*=============================================================================
* Function	: hwtm_pause
* Description	: 暂停周期定时器
* Input         : None
* Output        : None
* Return        : 0
=============================================================================*/
int hwtm_pause(void) {
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	/* Disable TIM2 update interrupt */
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);

	return 0;
}

/*=============================================================================
* Function	: hwtm_stop
* Description	: 停止周期定时器
* Input         : None
* Output        : None
* Return        : 0
=============================================================================*/
int hwtm_stop(void) {
	/* Disable TIM2 update interrupt */
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);

	/* Disable TIM2 */
	TIM_Cmd(TIM2, DISABLE);

	return 0;
}

/*=============================================================================
* Function	: hwtm_get_cnt
* Description	: 获取定时计数
* Input         : None
* Output        : None
* Return        : 定时计数
=============================================================================*/
int hwtm_get_cnt(void) {
	int cnt;

	cnt = TIM_GetCounter(TIM2);
	return cnt;
}

/*=============================================================================
* Function	: hwtm_set_isr2
* Description	: 设置二级中断服务程序
* Input         : None
* Output        : None
* Return        : 0
=============================================================================*/
func_none_arg_t hwtm_set_isr2(func_none_arg_t func) {
	func_none_arg_t r;

	r = hwtm_2nd_isr;
	hwtm_2nd_isr = func;
	return r;
}

/*=============================================================================
* Function	: hwtm_isr
* Description	: 定时器中断服务程序
* Input         : None
* Output        : None
* Return        : 0
=============================================================================*/
int hwtm_isr(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == RESET) {
		return DCODEC_OK;
	}

	// 处理二级中断
	if (hwtm_2nd_isr != NULL) {
		hwtm_2nd_isr();
	}

	/* Clear TIM2 update interrupt */
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	return 0;
}

/************************************END OF FILE******************************/
