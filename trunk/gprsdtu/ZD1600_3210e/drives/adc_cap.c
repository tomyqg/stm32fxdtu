/******************************* (C) Embest ***********************************
* File Name          : adc_cap.c
* Author             : tary
* Date               : 2009-06-08
* Version            : 0.1
* Description        : 周期捕获ADC信号
******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "ucos_ii.h"
#include "drv_init.h"
#include "adc_cap.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LOCAL_DBG			0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Wave details names table */
data_fmt_t adcap_fmt_buf[1] = {
	{
	WAVE_FORMAT_PCM,
	CHANNEL_MONO,
	SAMPLE_RATE_8000,
	(1 * BITS_PER_SAMPLE_8 / 8) * SAMPLE_RATE_8000,
	(1 * BITS_PER_SAMPLE_8 / 8),
	BITS_PER_SAMPLE_8
	},
}, *adcap_lcl_fmt = &adcap_fmt_buf[0];

char* adcap_data_buf = NULL;
int adcap_blk_cnt = 0;
int adcap_blk_pos = 0;
int cap_state = ADCAP_STATE_STOPPED;
inform_clbk_t adcap_inform_clbk = NULL;

/* Private define ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : adcap_init
* Description    : Initializes the DAC audio codec
* Input          : None
* Output         : None
* Return         : ADCAP_OK
*******************************************************************************/
int adcap_init(void) {

	/* Configure the ADC peripheral */
	// be in drv_all_init()
	// adc_init();

	/* Configuration is OK */
	return ADCAP_OK;
}

/*******************************************************************************
* Function Name  : decode_start
* Description    : start play sound
* Input          : None
* Output         : None
* Return         : ADCAP_OK
*******************************************************************************/
int adcap_start(void) {

	cap_state = ADCAP_STATE_PLAYING;

	hwtm_set_isr2(adcap_tim_isr);

	hwtm_start(adcap_lcl_fmt->fc_sample_rate);

	return ADCAP_OK;
}

/*******************************************************************************
* Function Name  : adcap_pause
* Description    : pause playing
* Input          : None
* Output         : None
* Return         : wave data index playing now
*******************************************************************************/
int adcap_pause(void) {

	hwtm_pause();

	/* Set Paused status to inform other modules about the codec status */
	cap_state = ADCAP_STATE_PAUSED;

	/* Return the current data pointer position */
	return adcap_blk_pos;
}

/*******************************************************************************
* Function Name  : adcap_stop
* Description    : stop playing the audio file, reset the pointer
* Input          : None
* Output         : None
* Return         : wave data index playing now
*******************************************************************************/
int adcap_stop(void) {

	hwtm_stop();

	/* Reinitialize the audio data pointer */
	adcap_blk_pos = 0;

	/* Set Paused status to inform other modules about the codec status */
	cap_state = ADCAP_STATE_STOPPED;

	return adcap_blk_pos;
}

/*******************************************************************************
* Function Name  : adcap_set_buf
* Description    : specify data be played by buf
* Input          : -buf      the data pointer
*                  -size     size of the buf
* Output         : None
* Return         : ADCAP_OK
*******************************************************************************/
int adcap_set_buf(char* buf, int size) {
	adcap_data_buf = buf;
	adcap_blk_cnt = size / adcap_lcl_fmt->fc_block_align;
	adcap_blk_pos = 0;
	return ADCAP_OK;
}

/*******************************************************************************
* Function Name  : adcap_set_pos
* Description    : set the data position
* Input          : None
* Output         : None
* Return         : the data position playing now
*******************************************************************************/
int adcap_set_pos(int pos) {

	if (pos < adcap_blk_cnt) {
		adcap_blk_pos = pos;
	}

	return pos;
}

/*******************************************************************************
* Function Name  : adcap_get_pos
* Description    : get the data position
* Input          : None
* Output         : None
* Return         : the data position playing now
*******************************************************************************/
int adcap_get_pos(void) {
	return adcap_blk_pos;
}

/*******************************************************************************
* Function Name  : adcap_state
* Description    : get playing state
* Input          : None
* Output         : None
* Return         : playing state
*******************************************************************************/
int adcap_state(void) {
	return cap_state;
}

/*******************************************************************************
* Function Name  : decodec_update_state
* Description    : set the callback function
* Input          : None
* Output         : None
* Return         : ADCAP_OK
*******************************************************************************/
int adcap_set_inform(inform_clbk_t clbk) {
	adcap_inform_clbk = clbk;
	return ADCAP_OK;
}

/*******************************************************************************
* Function Name  : adcap_set_fmt
* Description    : set the data format while playing them
* Input          : None
* Output         : None
* Return         : - ADCAP_OK    success
*                  - ADCAP_ERR   format unsupport
*******************************************************************************/
int adcap_set_fmt(data_fmt_t* fmt) {
	uint16_t abps, fba;

	if (fmt->fc_code != WAVE_FORMAT_PCM) {
		return ADCAP_ERR;
	}

	if (fmt->fc_channel_nr != CHANNEL_MONO) {
		return ADCAP_ERR;
	}

	if (1
	// && (fmt->fc_sample_bit != BITS_PER_SAMPLE_8)
	// && (fmt->fc_sample_bit != BITS_PER_SAMPLE_16)
	&& (fmt->fc_sample_bit > BITS_PER_SAMPLE_16)
	) {
		return ADCAP_ERR;
	}

	*adcap_lcl_fmt = *fmt;
	abps = ((fmt->fc_sample_bit + 7) / 8) * 8;
	fba = fmt->fc_channel_nr * abps / 8;

	adcap_lcl_fmt->fc_block_align = fba;
	adcap_lcl_fmt->fc_byte_rate = fba * fmt->fc_sample_rate;
#if 0
	printf("\r\nfmt->fc_code =\t\t\t%d", fmt->fc_code);
	printf("\r\nfmt->fc_channel_nr =\t\t%d", fmt->fc_channel_nr);
	printf("\r\nfmt->fc_sample_rate =\t\t%d", fmt->fc_sample_rate);
	printf("\r\nfmt->fc_byte_rate =\t\t%d", fmt->fc_byte_rate);
	printf("\r\nfmt->fc_block_align =\t\t%d", fmt->fc_block_align);
	printf("\r\nfmt->fc_sample_bit =\t\t%d", fmt->fc_sample_bit);
#endif
	return ADCAP_OK;
}

/*******************************************************************************
* Function Name  : adcap_update_state
* Description    : check if STOP or PAUSE command are generated and performs the 
*                :  relative action (STOP or PAUSE playing)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int adcap_update_state(void) {
	int r;
	
	r = ADCAP_OK;
	if (adcap_inform_clbk != NULL) {
		r = adcap_inform_clbk(adcap_data_buf, adcap_blk_pos, adcap_blk_cnt);
		if (r < 0) {
			adcap_stop();
		}
	}
	return r;
}

/*******************************************************************************
* Function Name  : adcap_fill_data
* Description    : None
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int adcap_fill_data(int pos, unsigned data) {

	set_byte_uint(
	  &adcap_data_buf[pos * adcap_lcl_fmt->fc_block_align], 
	  adcap_lcl_fmt->fc_block_align,
	  data,
	  0
	);
	return ADCAP_OK;
}

/*******************************************************************************
* Function Name  : adcap_tim_isr
* Description    : set the audio data using DAC peripheral and checks the 
*                :   audio playing status (if a command (Pause/Stop) is pending 
*                :   the playing status is updated). If the interrupt 
*                :   is used to synchronize data sending, this function should be 
*                :   called in the hardware timer isr.
* Input          : None
* Output         : None
* Return         : ADCAP_OK
*******************************************************************************/
int adcap_tim_isr(void) {
	unsigned data = 0;

	if (adcap_blk_pos >= adcap_blk_cnt) {
		return -1;
	}

	data = adc_get_value();

	// printf("\r\n%d", data);

	/* Offset data for half of DAC area */
	adcap_fill_data(adcap_blk_pos, data >> 4);

	/* Check and update the stream playing status */
	if (++adcap_blk_pos >= adcap_blk_cnt) {
		// printf("\r\n %d %d", adcap_blk_pos, adcap_blk_cnt);
		adcap_update_state();
	}

	return ADCAP_OK;
}

/************************************END OF FILE******************************/
