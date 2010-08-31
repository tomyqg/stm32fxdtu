
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eval.h"
#include "includes.h"

OS_STK Task1Stk[100];
OS_STK Task2Stk[100];

OS_FLAG_GRP *task;

OS_FLAGS flag;

/*********************************************************************************************************
   FUNCTION PROTOTYPES
*********************************************************************************************************/
void Task1(void);
void Task2(void);

int  main(void)
{
    INT8U err;

	SystemInit();
	SysTick_Config(SystemFrequency / 1000);
	OSInit();

	/* Initialize Leds mounted on STM3210X-EVAL board */
	STM_EVAL_LEDInit(LED1);
	STM_EVAL_LEDInit(LED2);
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);

	task = OSFlagCreate(0,&err);

	if(err != OS_NO_ERR)
		while(1);

	OSTaskCreate(Task1,(void *)0,&Task1Stk[99],0);
	OSTaskCreate(Task2,(void *)0,&Task2Stk[99],1);
	OSStart();
}

void Task1(void)
{
	INT8U err;

	for(;;)
	{
		flag = OSFlagPend(task,0x01,OS_FLAG_WAIT_SET_ALL+OS_FLAG_CONSUME,0,&err);

		/* Toggle LED1 */
	    STM_EVAL_LEDToggle(LED1);
		OSTimeDly(1000);
	    STM_EVAL_LEDToggle(LED1);

		/* Toggle LED2 */
	    STM_EVAL_LEDToggle(LED2);
	    OSTimeDly(1000);
	    STM_EVAL_LEDToggle(LED2);
	}
}

void Task2(void)
{
	INT8U err;

	for(;;)
	{
		OSFlagPost(task,0x01,OS_FLAG_SET,&err);

	    /* Toggle LED3 */
	    STM_EVAL_LEDToggle(LED3);
		OSTimeDly(1000);
	    STM_EVAL_LEDToggle(LED3);

	    /* Toggle LED4 */
	    STM_EVAL_LEDToggle(LED4);
		OSTimeDly(1000);
	    STM_EVAL_LEDToggle(LED4);
	}
}


