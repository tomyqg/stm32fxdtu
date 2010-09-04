/******************************* (C) Embest ***********************************
* File Name          : wav_fmt.h
* Author             : tary
* Date               : 2009-06-08
* Version            : 0.1
* Description        : .WAV 文件格式
******************************************************************************/

#ifndef __WAV_FMT_H__
#define __WAV_FMT_H__

#include <stdint.h>

/* .WAV file format :

 Endian      Offset      Length      Contents
  big         0           4 bytes     'RIFF'             // 0x52494646
  little      4           4 bytes     <file length - 8>
  big         8           4 bytes     'WAVE'             // 0x57415645

Next, the fmt chunk describes the sample format:

  big         12          4 bytes     'fmt '          // 0x666D7420
  little      16          4 bytes     0x00000010      // Length of the fmt data (16 bytes)
  little      20          2 bytes     0x0001          // Format tag: 1 = PCM
  little      22          2 bytes     <channels>      // Channels: 1 = mono, 2 = stereo
  little      24          4 bytes     <sample rate>   // Samples per second: e.g., 22050
  little      28          4 bytes     <bytes/second>  // sample rate * block align
  little      32          2 bytes     <block align>   // channels * bits/sample / 8
  little      34          2 bytes     <bits/sample>   // 8 or 16

Finally, the data chunk contains the sample data:

  big         36          4 bytes     'data'        // 0x64617461
  little      40          4 bytes     <length of the data block>
  little      44          *           <sample data>

*/

/* Audio file header size */
#define HEADER_SIZE			100

/* Audio Parsing Constants */
#define  ChunkID			0x52494646	/* correspond to the letters 'RIFF' */
#define  FileFormat			0x57415645	/* correspond to the letters 'WAVE' */
#define  FormatID			0x666D7420	/* correspond to the letters 'fmt ' */
#define  DataID				0x64617461	/* correspond to the letters 'data' */
#define  FactID				0x66616374	/* correspond to the letters 'fact' */

#define WAVE_FORMAT_PCM			0x01
#define FORMAT_CHUNK_SIZE		0x10
#define CHANNEL_MONO			0x01
#define CHANNEL_STEREO			0x02

#define SAMPLE_RATE_8000		8000
#define SAMPLE_RATE_11025		16000
#define SAMPLE_RATE_16000		16000
#define SAMPLE_RATE_22050		22050
#define SAMPLE_RATE_44100		44100
#define SAMPLE_RATE_48000		48000
#define BITS_PER_SAMPLE_8		8
#define BITS_PER_SAMPLE_16		16
#define BITS_PER_SAMPLE_24		24

/* Exported types ------------------------------------------------------------*/
typedef struct {
	uint16_t fc_code;		// coding format, PCM etc
	uint16_t fc_channel_nr;		// channels count
	uint32_t fc_sample_rate;	// sample per second
	uint32_t fc_byte_rate;		// sample rate * block align
	uint16_t fc_block_align;	// channels count * bits per sample / 8
	uint16_t fc_sample_bit;		// bits per sample
	//uint16_t fc_optional;		// occur by fc_size
} data_fmt_t;

// CHUNK DEFINITION
typedef struct {
	char id[4];			// string, 4 bytes
	int size;			// size, integer
	char data[4];			// data, bytes count desinated by size
} riff_chunk_t;

typedef int (*inform_clbk_t)(char* buf, int idx, int cnt);


#endif //__WAV_FMT_H__

/************************************END OF FILE******************************/
