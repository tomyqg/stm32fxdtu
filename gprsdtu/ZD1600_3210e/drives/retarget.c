/******************************* (C) Embest ***********************************
* File Name          : retarget.c
* Author             : tary
* Date               : 2009-02-26
* Version            : 0.2
* Description        : 'Retarget' layer for target-dependent low level functions
******************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <rt_misc.h>
#include "aux_lib.h"
//#include "uart_std.h"
#define LOCAL_DBG		0
#define	NO_SERIAL


#pragma import(__use_no_semihosting_swi)


struct __FILE {
	int handle; 
	int ch;	
	/* Add whatever you need here */
};

FILE __stdin = {0, EOF};
FILE __stdout = {1, EOF};

char stdin_buf[84];
int stdin_idx = 0;
int stdin_cnt = 0;
static int l_ch = EOF;

/*=============================================================================
* Function	: _ttywrch
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int ferror (FILE *stream) {
	/* Your implementation of ferror */
	return EOF;
}

/*=============================================================================
* Function	: _ttywrch
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
void _ttywrch (int ch) {
#if !defined(NO_SERIAL)
	uart_send_ch(udev_std, ch); 
#else
	return;
#endif
}

/*=============================================================================
* Function	: _sys_exit
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
void _sys_exit (int return_code) {
	for (;;) {
		;
	}
}

/*=============================================================================
* Function	: fgetc
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int fgetc(FILE *stream) {
#if 1
	return 0;//test
//	return uart_get_key(udev_std);
#else
	int ch = EOF;

	if (EOF != stream->ch) {
		ch = stream->ch;
		stream->ch = EOF;
		DBG_LINE(2, " ch", ch);
		return (l_ch = ch);
	}

	if (stdin_idx >= stdin_cnt) {
		stdin_cnt = uart_gets(udev_std, &stdin_buf[0], 80);
		DBG_LINE(6, " ch", ch,
		  " stdin_cnt", stdin_cnt,
		  " stdin_idx", stdin_idx
		  );
		DBG_BUF("stdin_buf", stdin_buf, stdin_cnt);
		stdin_idx = 0;
	}
	ch = stdin_buf[stdin_idx++];
	return (l_ch = ch);
#endif
}

/*=============================================================================
* Function	: fputc
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int fputc (int ch, FILE *stream) {
#if !defined(NO_SERIAL)
	return (uart_send_ch(udev_std, ch));
#else
	return 0;
#endif
}

/*=============================================================================
* Function	: __backspace
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int __backspace(FILE *stream) {
	if (EOF != stream->ch) {
		return EOF;
	}
	if (isalnum(l_ch)
	|| isspace(l_ch)
	) {
		DBG_LINE(2, " ch", l_ch);
		stream->ch = l_ch;
	}
	return 0;
}

/************************************END OF FILE******************************/
