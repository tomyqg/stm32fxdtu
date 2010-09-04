/******************************* (C) Embest ***********************************
* File Name          : adc.h
* Author             : tary
* Date               : 2009-02-11
* Version            : 0.1
* Description        : 
******************************************************************************/

#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f10x_lib.h"
#include "systick.h"
#include "vtimer.h"

#define ADC_IO_PIN	GPIO_Pin_0
#define ADC_IO_PORT	GPIOB
#define ADC_IO_CLK	RCC_APB2Periph_GPIOB
#define ADC_NR		ADC1
#define ADC_CHANNEL	ADC_Channel_8
#define ADC_OP_CLK	RCC_APB2Periph_ADC1

int adc_init(void);
unsigned short adc_get_value(void);
int adc_dbg(int id);

#endif	//__ADC_H__

/************************************END OF FILE******************************/
