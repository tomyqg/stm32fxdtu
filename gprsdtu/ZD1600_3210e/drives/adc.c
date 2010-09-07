/******************************* (C) Embest ***********************************
* File Name          : adc.c
* Author             : tary
* Date               : 2009-02-11
* Version            : 0.1
* Description        : 
******************************************************************************/

#include <stdio.h>
#include "gpio.h"
#include "adc.h"
#include "aux_lib.h"

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
static int adc_gpio_config(void) {
	gpio_init(ADC_IO_PORT,
	  ADC_IO_PIN, GPIO_Speed_50MHz, GPIO_Mode_AIN,
	  ADC_IO_CLK
	);
	return 0;
}

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
static int adc_part_config(void) {
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(ADC_OP_CLK, ENABLE);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC_NR, &ADC_InitStructure);
	/* ADC_NR regular channels configuration */ 
	ADC_RegularChannelConfig(ADC_NR, ADC_CHANNEL, 1, ADC_SampleTime_1Cycles5); 	

	/* Enable ADC_NR */
	ADC_Cmd(ADC_NR, ENABLE);

	/* Enable ADC_NR reset calibaration register */   
	ADC_ResetCalibration(ADC_NR);
	/* Check the end of ADC_NR reset calibration register */
	while (ADC_GetResetCalibrationStatus(ADC_NR));

	/* Start ADC_NR calibaration */
	ADC_StartCalibration(ADC_NR);
	/* Check the end of ADC_NR calibration */
	while (ADC_GetCalibrationStatus(ADC_NR));

	return 0;
}
/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int adc_init(void) {

	adc_gpio_config();
	adc_part_config();
	return 0;
}

/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
unsigned short adc_get_value(void)
{
	unsigned short val;

	val = ADC_GetConversionValue(ADC_NR);

	/* Start ADC_NR Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC_NR, ENABLE);
	while (ADC_GetSoftwareStartConvStatus(ADC_NR));

	return val;
}


/*=============================================================================
* Function	: 
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int adc_dbg(int id) {
	int dat;

	dat = adc_get_value();
	printf("adc data = %d\r\n", dat);
//	MSTimerStart(1000, adc_dbg);
	return 0;
}

/************************************END OF FILE******************************/
