#include <ucos_ii.h>
#include "stm32f10x_lib.h"


void App_TaskCreateHook(OS_TCB * ptcb)
{
}

void App_TaskDelHook(OS_TCB * ptcb)
{
	__WFI();
}
void App_TaskIdleHook(void)
{
}

void App_TaskStatHook(void)
{
	OSCtxSwCtr = 0;
}

void App_TaskSwHook(void)
{
#if OS_TASK_PROFILE_EN > 0
	INT32U cycles;

	cycles = OSTimeGet();	/* This task is done                           */
	OSTCBCur->OSTCBCyclesTot += cycles - OSTCBCur->OSTCBCyclesStart;
	OSTCBHighRdy->OSTCBCyclesStart = cycles;
#endif
}

void App_TCBInitHook(OS_TCB * ptcb)
{
}
void App_TimeTickHook(void)
{
}
