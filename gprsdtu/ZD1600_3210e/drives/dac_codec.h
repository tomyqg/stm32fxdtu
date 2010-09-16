/******************************* (C) Embest ***********************************
* File Name          : dac_codec.h
* Author             : tary
* Date               : 2009-05-26
* Version            : 0.1
* Description        : DAC≤•∑≈“Ù¿÷
******************************************************************************/

#ifndef __DAC_CODEC_H__
#define __DAC_CODEC_H__

/* Includes ------------------------------------------------------------------*/
#include "wav_fmt.h"

/* Exported macro ------------------------------------------------------------*/

#define DCODEC_VOL_MIN			0x0
#define DCODEC_VOL_MAX			0xFF
#define DCODEC_VOL_DEF			0x33	//default

#define DCODEC_STATE_STOPPED		0x0
#define DCODEC_STATE_PLAYING		0x1
#define DCODEC_STATE_PAUSED		0x2

// api return value
#define	DCODEC_OK			0
#define DCODEC_ERR_FORMAT		(-1)
#define DCODEC_ERR_UNSUPP		(-2)

/* Exported functions ------------------------------------------------------- */
int dcodec_init(void);
int dcodec_stop(void);
int dcodec_start(void);
int dcodec_pause(void);
int dcodec_state(void);
int dcodec_tim_isr(void);
int dcodec_get_vol(void);
int dcodec_get_pos(void);
int dcodec_set_vol(int vol);
int dcodec_set_pos(int pos);
int dcodec_get_channel(void);
int dcodec_set_channel(int chnnl);
int dcodec_set_fmt(data_fmt_t* fmt);
int dcodec_set_buf(char* buf, int size);
int dcodec_set_inform(inform_clbk_t clbk);

#endif //__DAC_CODEC_H__

/************************************END OF FILE******************************/
