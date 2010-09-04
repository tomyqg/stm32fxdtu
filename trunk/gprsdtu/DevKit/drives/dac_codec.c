/******************************* (C) Embest ***********************************
* File Name          : dac_codec.c
* Author             : tary
* Date               : 2009-05-26
* Version            : 0.1
* Description        : DAC播放音乐
******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "ucos_ii.h"
#include "drv_init.h"
#include "dac_codec.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LOCAL_DBG			0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Wave details names table */
data_fmt_t fmt_buf[1] = {
	{
	WAVE_FORMAT_PCM,
	CHANNEL_MONO,
	SAMPLE_RATE_44100,
	(1 * BITS_PER_SAMPLE_16 / 8) * SAMPLE_RATE_44100,
	(1 * BITS_PER_SAMPLE_16 / 8),
	BITS_PER_SAMPLE_16
	},
}, *lcl_fmt = &fmt_buf[0];

char* wav_data_buf = NULL;
int wav_blk_cnt = 0;
int wav_blk_pos = 0;
int cur_volume = DCODEC_VOL_DEF;
int cur_channel = 0;
int cur_state = DCODEC_STATE_STOPPED;
inform_clbk_t inform_clbk = NULL;

/* Private define ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
int dcodec_gpio_config(void);
int dcodec_dac_config(void);

/*******************************************************************************
* Function Name  : dcodec_gpio_config
* Description    : Initializes the GPIO pins used by the codec application.
* Input          : None
* Output         : None
* Return         : DCODEC_OK
*******************************************************************************/
int dcodec_gpio_config(void) {

	/* DAC pin configuration (PA4) */
	gpio_init(GPIOA,
	  GPIO_Pin_4, GPIO_Speed_50MHz, GPIO_Mode_AIN,
	  RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO
	);
	return DCODEC_OK;
}

/*******************************************************************************
* Function Name  : dcodec_dac_config
* Description    : Configure the DAC Peripheral
* Input          : None
* Output         : None
* Return         : DCODEC_OK
*******************************************************************************/
int dcodec_dac_config(void) {
	DAC_InitTypeDef DAC_InitStructure;

	/* Enable DAC APB1 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	DAC_StructInit(&DAC_InitStructure);
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Cmd(DAC_Channel_1, ENABLE);

	return DCODEC_OK;
}

/*******************************************************************************
* Function Name  : dcodec_init
* Description    : Initializes the DAC audio codec
* Input          : None
* Output         : None
* Return         : DCODEC_OK
*******************************************************************************/
int dcodec_init(void) {
	/* Configure the DAC pin (PA4) */
	dcodec_gpio_config();

	/* Configure the DAC peripheral */
	dcodec_dac_config();

	/* Configuration is OK */
	return DCODEC_OK;
}

/*******************************************************************************
* Function Name  : decode_start
* Description    : start play sound
* Input          : None
* Output         : None
* Return         : DCODEC_OK
*******************************************************************************/
int dcodec_start(void) {

	cur_state = DCODEC_STATE_PLAYING;

	hwtm_set_isr2(dcodec_tim_isr);

	hwtm_start(lcl_fmt->fc_sample_rate);

	return DCODEC_OK;
}

/*******************************************************************************
* Function Name  : dcodec_pause
* Description    : pause playing
* Input          : None
* Output         : None
* Return         : wave data index playing now
*******************************************************************************/
int dcodec_pause(void) {

	hwtm_pause();

	/* Set Paused status to inform other modules about the codec status */
	cur_state = DCODEC_STATE_PAUSED;

	/* Return the current data pointer position */
	return wav_blk_pos;
}

/*******************************************************************************
* Function Name  : dcodec_stop
* Description    : stop playing the audio file, reset the pointer
* Input          : None
* Output         : None
* Return         : wave data index playing now
*******************************************************************************/
int dcodec_stop(void) {

	hwtm_stop();

	/* Reinitialize the audio data pointer */
	wav_blk_pos = 0;

	/* Set Paused status to inform other modules about the codec status */
	cur_state = DCODEC_STATE_STOPPED;

	return wav_blk_pos;
}

/*******************************************************************************
* Function Name  : dcodec_pos2idx
* Description    : convert the sample position to byte offset
* Input          : -pos      sample position
* Output         : None
* Return         : byte offset
*******************************************************************************/
int dcodec_pos2idx(int pos) {
	int idx;

	idx = lcl_fmt->fc_block_align * pos;
	idx += ((lcl_fmt->fc_sample_bit + 7) / 8) * cur_channel;

	return idx;
}

/*******************************************************************************
* Function Name  : dcodec_idx2pos
* Description    : convert the byte offset to sample position
* Input          : -idx      the byte offset
* Output         : None
* Return         : sample position
*******************************************************************************/
int dcodec_idx2pos(int idx) {
	int pos;

	pos = idx / lcl_fmt->fc_block_align;

	return pos;
}

/*******************************************************************************
* Function Name  : dcodec_set_buf
* Description    : specify data be played by buf
* Input          : -buf      the data pointer
*                  -size     size of the buf
* Output         : None
* Return         : DCODEC_OK
*******************************************************************************/
int dcodec_set_buf(char* buf, int size) {
	wav_data_buf = buf;
	wav_blk_cnt = dcodec_idx2pos(size);
	wav_blk_pos = 0;
	return DCODEC_OK;
}

/*******************************************************************************
* Function Name  : dcodec_set_vol
* Description    : set the audio volume
* Input          :  -	vol: the volume value from DCODEC_VOL_MIN to DCODEC_VOL_MAX
* Output         : None
* Return         : audio volume
*******************************************************************************/
int dcodec_set_vol(int vol) {

	if (DCODEC_VOL_MIN > vol) {
		vol = DCODEC_VOL_MIN;
	} else if (vol > DCODEC_VOL_MAX) {
		vol = DCODEC_VOL_MAX;
	}

	cur_volume = vol;

	return cur_volume;
}

/*******************************************************************************
* Function Name  : dcodec_get_vol
* Description    : get current volume value
* Input          : None
* Output         : None
* Return         : current volume value
*******************************************************************************/
int dcodec_get_vol(void) {
	return cur_volume;
}

/*******************************************************************************
* Function Name  : dcodec_set_pos
* Description    : set the data position
* Input          : None
* Output         : None
* Return         : the data position playing now
*******************************************************************************/
int dcodec_set_pos(int pos) {

	if (pos < wav_blk_cnt) {
		wav_blk_pos = pos;
	}

	return pos;
}

/*******************************************************************************
* Function Name  : dcodec_get_pos
* Description    : get the data position
* Input          : None
* Output         : None
* Return         : the data position playing now
*******************************************************************************/
int dcodec_get_pos(void) {
	return wav_blk_pos;
}

/*******************************************************************************
* Function Name  : dcodec_set_channel
* Description    : set the audio volume
* Input          :  -	vol: the volume value from DCODEC_VOL_MIN to DCODEC_VOL_MAX
* Output         : None
* Return         : current channel playing
*******************************************************************************/
int dcodec_set_channel(int chnnl) {

	if (chnnl < lcl_fmt->fc_channel_nr) {
		cur_channel = chnnl;
	}

	return cur_channel;
}

/*******************************************************************************
* Function Name  : dcodec_get_channel
* Description    : get current channel playing
* Input          : None
* Output         : None
* Return         : current channel playing
*******************************************************************************/
int dcodec_get_channel(void) {
	return cur_channel;
}

/*******************************************************************************
* Function Name  : dcodec_state
* Description    : get playing state
* Input          : None
* Output         : None
* Return         : playing state
*******************************************************************************/
int dcodec_state(void) {
	return cur_state;
}

/*******************************************************************************
* Function Name  : decodec_update_state
* Description    : set the callback function
* Input          : None
* Output         : None
* Return         : DCODEC_OK
*******************************************************************************/
int dcodec_set_inform(inform_clbk_t clbk) {
	inform_clbk = clbk;
	return DCODEC_OK;
}

/*******************************************************************************
* Function Name  : dcodec_set_fmt
* Description    : set the data format while playing them
* Input          : None
* Output         : None
* Return         : - DCODEC_OK    success
*                  - DCODEC_ERR_UNSUPP
*                                 format unsupport
*******************************************************************************/
int dcodec_set_fmt(data_fmt_t* fmt) {
	uint16_t abps, fba;

	if (fmt->fc_code != WAVE_FORMAT_PCM) {
		return DCODEC_ERR_UNSUPP;
	}

	if (1
	// && (fmt->fc_sample_bit != BITS_PER_SAMPLE_8)
	// && (fmt->fc_sample_bit != BITS_PER_SAMPLE_16)
	&& (fmt->fc_sample_bit > BITS_PER_SAMPLE_24)
	) {
		return DCODEC_ERR_UNSUPP;
	}

	*lcl_fmt = *fmt;
	abps = ((fmt->fc_sample_bit + 7) / 8) * 8;
	fba = fmt->fc_channel_nr * abps / 8;

	lcl_fmt->fc_block_align = fba;
	lcl_fmt->fc_byte_rate = fba * fmt->fc_sample_rate;
#if 0
	printf("\r\nfmt->fc_code =\t\t\t%d", fmt->fc_code);
	printf("\r\nfmt->fc_channel_nr =\t\t%d", fmt->fc_channel_nr);
	printf("\r\nfmt->fc_sample_rate =\t\t%d", fmt->fc_sample_rate);
	printf("\r\nfmt->fc_byte_rate =\t\t%d", fmt->fc_byte_rate);
	printf("\r\nfmt->fc_block_align =\t\t%d", fmt->fc_block_align);
	printf("\r\nfmt->fc_sample_bit =\t\t%d", fmt->fc_sample_bit);
#endif
	return DCODEC_OK;
}

/*******************************************************************************
* Function Name  : decodec_update_state
* Description    : check if STOP or PAUSE command are generated and performs the 
*                :  relative action (STOP or PAUSE playing)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int decodec_update_state(void) {
	int r;
	
	r = DCODEC_OK;
	if (inform_clbk != NULL) {
		r = inform_clbk(wav_data_buf, wav_blk_pos, wav_blk_cnt);
		if (r < 0) {
			dcodec_stop();
		}
	}
	return r;
}

/*******************************************************************************
* Function Name  : dcodec_channel_data
* Description    : get the current output dac data
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int dcodec_channel_data(int pos) {
	int idx;
	int data;
	int size;

	idx = dcodec_pos2idx(pos);
	size = ((lcl_fmt->fc_sample_bit + 7) / 8);

	data = (int) get_byte_uint(&wav_data_buf[idx], size, 0);

	data <<= (4 - size) * 8;

	// 8 位PCM数据无符号, 其它位数据有符号
	if (size != 1) {
		// 转换成无符号数(用于DAC, 及音量控制)
		data ^= 1 << 31;
	}

	data = (long long)(unsigned)data * cur_volume / 0xFF;

	return data;
}

/*******************************************************************************
* Function Name  : dcodec_tim_isr
* Description    : set the audio data using DAC peripheral and checks the 
*                :   audio playing status (if a command (Pause/Stop) is pending 
*                :   the playing status is updated). If the interrupt 
*                :   is used to synchronize data sending, this function should be 
*                :   called in the hardware timer isr.
* Input          : None
* Output         : None
* Return         : DCODEC_OK
*******************************************************************************/
int dcodec_tim_isr(void) {
	long data = 0;

	if (wav_blk_pos >= wav_blk_cnt) {
		return -1;
	}

	/* Offset data for half of DAC area */
	data = dcodec_channel_data(wav_blk_pos);

	DAC_SetChannel1Data(DAC_Align_12b_L, (data >> 16) & 0xFFF0);

	/* Check and update the stream playing status */
	if (++wav_blk_pos >= wav_blk_cnt) {
		decodec_update_state();
	}

	return DCODEC_OK;
}

/************************************END OF FILE******************************/
