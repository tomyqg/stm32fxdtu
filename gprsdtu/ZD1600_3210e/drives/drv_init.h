/******************************* (C) Embest ***********************************
* File Name          : drv_init.h
* Author             : tary
* Date               : 2009-05-27
* Version            : 0.2u
* Description        : 
******************************************************************************/

#ifndef __DRV_INIT_H__
#define __DRV_INIT_H__

#include <stdint.h>
#include "stm32f10x_lib.h"
#include "aux_lib.h"
#include "led.h"   
#include "adc.h"
#include "gpio.h"
//#include "uart_drv.h"
//#include "uart_std.h"
//#include "uart2.h"
#include "keys.h"
//#include "lcd_low.h"
//#include "wav_fmt.h"
#include "tmr_drv.h"
//#include "adc_cap.h"
//#include "dac_codec.h"
//#include "fsmc_sram.h"
//#include "fsmc_nand.h"
#include "rtc.h"
//#include "cdrs_op.h"
//#include "fm1702.h"

int drv_all_init(void);
int RCC_Configuration(void);
int NVIC_Configuration(void);


#endif //__DRV_INIT_H__

/************************************END OF FILE******************************/
