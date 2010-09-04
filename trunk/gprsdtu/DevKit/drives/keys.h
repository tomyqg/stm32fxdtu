#ifndef __KEYS_H__
#define __KEYS_H__


#define KEYS_USE_EINT	0	/* 0:使用线程模式，1：使用中断模式 */

#define QKEYSIZE	4

#define KEY_SEL		1
#define KEY_UP		2
#define KEY_DOWN	3
#define KEY_LEFT	4
#define KEY_RIGHT	5
#define KEY_USER	6
#define KEY_TAMP	7
#define KEY_WAKE	8

#define KEY_UP_DATA	255

#define KEY_SEL_UP	(KEY_SEL + KEY_UP_DATA)
#define KEY_UP_UP	(KEY_UP	+ KEY_UP_DATA)
#define KEY_DOWN_UP	(KEY_DOWN + KEY_UP_DATA)
#define KEY_LEFT_UP	(KEY_LEFT + KEY_UP_DATA)
#define KEY_RIGHT_UP	(KEY_RIGHT + KEY_UP_DATA)
#define KEY_USER_UP	(KEY_USER + KEY_UP_DATA)
#define KEY_TAMP_UP	(KEY_TAMP + KEY_UP_DATA)
#define KEY_WAKE_UP	(KEY_WAKE + KEY_UP_DATA)

#if KEYS_USE_EINT == 0
void key_scan(void);
#endif

int key_init(void);
int key_read(void);
int key_accept(void);
int key_flush(void);

#endif				//__KEYS_H__
