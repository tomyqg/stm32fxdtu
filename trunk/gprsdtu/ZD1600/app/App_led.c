#include <ucos_ii.h>
#include "led.h"

OS_STK App_TaskLedStk[APP_TASK_LED_STK_SIZE];
void App_led(void *parg)
{

	while (1) {
		if (parg) {
			led_on(1);
			OSTimeDlyHMSM(0, 0, 0, 700);
			led_off(1);
			OSTimeDlyHMSM(0, 0, 0, 400);
			led_on(1);
			OSTimeDlyHMSM(0, 0, 0, 200);
			led_off(1);
			OSTimeDlyHMSM(0, 0, 0, 300);
			led_on(1);
			OSTimeDlyHMSM(0, 0, 0, 200);
			led_off(1);
			OSTimeDlyHMSM(0, 0, 3, 0);
		} else {
			led_on(1);
			OSTimeDlyHMSM(0, 0, 0, 500);
			led_off(1);
			OSTimeDlyHMSM(0, 0, 0, 500);

		}
	}
}
