/******************************* (C) Embest ***********************************
* File Name          : tmr_drv.h
* Author             : tary
* Date               : 2009-06-08
* Version            : 0.1
* Description        : 硬件定时器驱动
******************************************************************************/

#ifndef __TMR_DRV_H__
#define __TMR_DRV_H__

#include "aux_lib.h"

int hwtm_isr(void);
int hwtm_init(void);
int hwtm_stop(void);
int hwtm_pause(void);
int hwtm_get_cnt(void);
int hwtm_start(unsigned rate);
func_none_arg_t hwtm_set_isr2(func_none_arg_t func);

#endif //__TMR_DRV_H__

/************************************END OF FILE******************************/
