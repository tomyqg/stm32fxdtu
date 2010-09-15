/******************************* (C) Embest ***********************************
* File Name          : led.c
* Author             : tary
* Date               : 2009-05-27
* Version            : 0.2u
* Description        : 
******************************************************************************/
#include <assert.h>
#include "aux_lib.h"
#include "led.h"
#include "gpio.h"

#define	STM3210C
#ifdef	STM3210C
static int LedPins[] = {GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5};
#else
static int LedPins[] = {GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9};
#endif
int led_init(void) {
	/* Configure the LED IOs as Output PP */
	int i;
	int pins = 0;
	for (i=0; i<countof(LedPins); i++)
		pins |= LedPins[i];
	
	gpio_init(LED_PORT,
	  pins, GPIO_Speed_50MHz, GPIO_Mode_Out_PP,
	  RCC_APB2Periph_LED
	);
	return 0;
}

int get_led_state(int idx) {
	assert(idx > 0 && idx <= countof(LedPins));

	return Bit_SET == GPIO_ReadInputDataBit(LED_PORT, LedPins[idx-1]);
}


int set_led_state(int idx, int state) {
	assert(idx > 0 && idx <= countof(LedPins));

	GPIO_WriteBit(LED_PORT, LedPins[idx-1], (BitAction)(state));
	return Bit_SET == GPIO_ReadInputDataBit(LED_PORT, LedPins[idx-1]);
}

// 第二版电平改为相反值
void led_off(int idx) {
	assert(idx > 0 && idx <= countof(LedPins));
	GPIO_WriteBit(LED_PORT, LedPins[idx-1], Bit_RESET);
}
void led_on(int idx) {
	assert(idx > 0 && idx <= countof(LedPins));

	GPIO_WriteBit(LED_PORT, LedPins[idx-1], Bit_SET);
}

/************************************END OF FILE******************************/

