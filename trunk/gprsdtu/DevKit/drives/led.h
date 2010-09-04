/******************************* (C) Embest ***********************************
* File Name          : led.h
* Author             : tary
* Date               : 2009-05-27
* Version            : 0.2u
* Description        : 
******************************************************************************/

#ifndef	__LED_H__
#define	__LED_H__

#define LED_PORT			GPIOF
//#define LED_PIN				GPIO_Pin_6   /* PB.0 */
#define RCC_APB2Periph_LED		RCC_APB2Periph_GPIOF

int led_init(void);
int get_led_state(int idx);
int set_led_state(int idx, int state);

void led_on(int idx);
void led_off(int idx);

#endif	//__LED_H__

/************************************END OF FILE******************************/
