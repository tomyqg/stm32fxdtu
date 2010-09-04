/******************************* (C) Embest ***********************************
* File Name          : adc_cap.h
* Author             : tary
* Date               : 2009-06-08
* Version            : 0.1
* Description        : 周期捕获ADC信号
******************************************************************************/

#ifndef __ADC_CAP_H__
#define __ADC_CAP_H__

/* Includes ------------------------------------------------------------------*/
#include "wav_fmt.h"

/* Exported macro ------------------------------------------------------------*/

#define ADCAP_STATE_STOPPED		0x0
#define ADCAP_STATE_PLAYING		0x1
#define ADCAP_STATE_PAUSED		0x2

// api return value
#define	ADCAP_OK			0
#define ADCAP_ERR			(-1)

/* Exported functions ------------------------------------------------------- */
int adcap_init(void);
int adcap_stop(void);
int adcap_start(void);
int adcap_pause(void);
int adcap_state(void);
int adcap_tim_isr(void);
int adcap_get_pos(void);
int adcap_set_pos(int pos);
int adcap_set_fmt(data_fmt_t* fmt);
int adcap_set_buf(char* buf, int size);
int adcap_set_inform(inform_clbk_t clbk);

#endif //__ADC_CAP_H__

/************************************END OF FILE******************************/
