#include <ucos_ii.h>
#include "led.h"

OS_STK gd_task_led_stk[GD_TASK_LED_STK_SIZE];

void gd_task_led(void *parg)
{
	while (1) 
	{
		if (parg) 
		{
			led_on(1);
			OSTimeDlyHMSM(0, 0, 0, 200);
			led_off(1);
			OSTimeDlyHMSM(0, 0, 0, 200);
			led_on(2);
			OSTimeDlyHMSM(0, 0, 0, 200);
			led_off(2);
			OSTimeDlyHMSM(0, 0, 0, 200);
			led_on(3);
			OSTimeDlyHMSM(0, 0, 0, 200);
			led_off(3);
			OSTimeDlyHMSM(0, 0, 0, 200);
			led_on(4);
			OSTimeDlyHMSM(0, 0, 0, 200);
			led_off(4);
			OSTimeDlyHMSM(0, 0, 0, 200);
		} 
		else 
		{
			led_on(1);
			OSTimeDlyHMSM(0, 0, 0, 500);
			led_off(1);
			OSTimeDlyHMSM(0, 0, 0, 500);
		}
	}
}
