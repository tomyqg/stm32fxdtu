
#include <ucos_ii.h>
#include "stm32f10x_lib.h"
#include "keys.h"

#define REVERSEKEY  1
/* */

#define SELECT_Pin	        GPIO_Pin_7
#define UP_Pin              GPIO_Pin_15
#define LEFT_Pin            GPIO_Pin_14
#define RIGHT_Pin           GPIO_Pin_13
#define DOWN_Pin            GPIO_Pin_3	//PD3
#define USER_Pin            GPIO_Pin_8
#define TAMP_Pin		GPIO_Pin_13	//PC13
#define WAKE_Pin		GPIO_Pin_0	//PA0

#define SELECT_PinSource    GPIO_PinSource7
#define UP_PinSource        GPIO_PinSource15
#define LEFT_PinSource      GPIO_PinSource14
#define RIGHT_PinSource     GPIO_PinSource13
#define DOWN_PinSource      GPIO_PinSource3
#define USER_PinSource      GPIO_PinSource8
#define TAMP_PinSource		GPIO_PinSource13
#define WAKE_PinSource		GPIO_PinSource0

#if KEYS_USE_EINT

#define SELECT_IRQChannel   EXTI9_5_IRQChannel
#define UP_IRQChannel       EXTI15_10_IRQChannel
#define LEFT_IRQChannel     EXTI15_10_IRQChannel
#define RIGHT_IRQChannel    EXTI15_10_IRQChannel
#define DOWN_IRQChannel     EXTI3_IRQChannel	//PD3
#define USER_IRQChannel     EXTI9_5_IRQChannel
#define TAMP_IRQChannel		EXTI15_10_IRQChannel
#define WAKE_IRQChannel		EXTI0_IRQChannel

#define SELECT_Line         EXTI_Line7
#define UP_Line             EXTI_Line15
#define LEFT_Line           EXTI_Line14
#define RIGHT_Line          EXTI_Line13
#define DOWN_Line           EXTI_Line3	//PD3
#define USER_Line           EXTI_Line8
#define TAMP_Line		EXTI_Line13
#define WAKE_Line		EXTI_Line0

#endif

#define RCC_APB2Periph_GPIO_KEY_BUTTON  (RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOD | \
					RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC )

static OS_EVENT *EventKey;

static void *qKey[QKEYSIZE];

static void KEYS_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable Key Button GPIO Port, GPIO_LED and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_KEY_BUTTON
			       | RCC_APB2Periph_AFIO, ENABLE);


	/* Configure Key Button GPIO Pin as input floating */
	GPIO_InitStructure.GPIO_Pin =
	    SELECT_Pin | UP_Pin | LEFT_Pin | RIGHT_Pin | USER_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = TAMP_Pin;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = WAKE_Pin;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


#if KEYS_USE_EINT
	/* RIGHT Button */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, RIGHT_PinSource);

	/* LEFT Button */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, LEFT_PinSource);

	/* DOWN Button */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, DOWN_PinSource);

	/* UP Button */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, UP_PinSource);

	/* SEL Button */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, SELECT_PinSource);

	/* KEY Button */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, USER_PinSource);

	/* TAMP Button */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, TAMP_PinSource);

	/* WAKE Button */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, WAKE_PinSource);
#endif
}

#if KEYS_USE_EINT
static void KEYS_Interrupt_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the EXTI Interrupt */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQChannel;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = WAKE_IRQChannel;
	NVIC_Init(&NVIC_InitStructure);

	/* Configure EXTI Line to generate an interrupt on falling edge */
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;

	EXTI_InitStructure.EXTI_Line = SELECT_Line;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = UP_Line;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = LEFT_Line;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = RIGHT_Line;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = DOWN_Line;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = USER_Line;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = TAMP_Line;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = WAKE_Line;
	EXTI_Init(&EXTI_InitStructure);


	//清中断

	EXTI_ClearITPendingBit(SELECT_Line);
	EXTI_ClearITPendingBit(UP_Line);
	EXTI_ClearITPendingBit(LEFT_Line);
	EXTI_ClearITPendingBit(RIGHT_Line);
	EXTI_ClearITPendingBit(DOWN_Line);
	EXTI_ClearITPendingBit(USER_Line);
	EXTI_ClearITPendingBit(TAMP_Line);
	EXTI_ClearITPendingBit(WAKE_Line);
}
void key_isr(void)
{
	int key = 0;

	/* "TAMP" key is pressed */
	if (!GPIO_ReadInputDataBit(GPIOC, TAMP_Pin)) {
		key = KEY_TAMP;
		goto out;
	}

	/* "WAKE" key is pressed */
	if (!GPIO_ReadInputDataBit(GPIOA, WAKE_Pin)) {
		key = KEY_WAKE;
		goto out;
	}


	/* "KEY" key is pressed */
	if (!GPIO_ReadInputDataBit(GPIOG, USER_Pin)) {
		key = KEY_USER;
		goto out;
	}

	/* "right" key is pressed */
	if (!GPIO_ReadInputDataBit(GPIOG, RIGHT_Pin)) {
#if REVERSEKEY
		key = KEY_LEFT;
#else
		key = KEY_RIGHT;
#endif
		goto out;
	}
	/* "left" key is pressed */
	if (!GPIO_ReadInputDataBit(GPIOG, LEFT_Pin)) {
#if REVERSEKEY
		key = KEY_RIGHT;
#else
		key = KEY_LEFT;
#endif
		goto out;
	}
	/* "up" key is pressed */
	if (!GPIO_ReadInputDataBit(GPIOG, UP_Pin)) {
#if REVERSEKEY
		key = KEY_DOWN;
#else
		key = KEY_UP;
#endif
		goto out;
	}
	/* "down" key is pressed */
	if (!GPIO_ReadInputDataBit(GPIOD, DOWN_Pin)) {
#if REVERSEKEY
		key = KEY_UP;
#else
		key = KEY_DOWN;
#endif
		goto out;
	}
	/* "sel" key is pressed */
	if (!GPIO_ReadInputDataBit(GPIOG, SELECT_Pin)) {
		key = KEY_SEL;
		goto out;
	}



      out:
	if (key)		/* No key is pressed */
		OSQPost(EventKey, (void *) key);

	EXTI_ClearITPendingBit(SELECT_Line);
	EXTI_ClearITPendingBit(UP_Line);
	EXTI_ClearITPendingBit(LEFT_Line);
	EXTI_ClearITPendingBit(RIGHT_Line);
	EXTI_ClearITPendingBit(DOWN_Line);
	EXTI_ClearITPendingBit(USER_Line);
	EXTI_ClearITPendingBit(TAMP_Line);
	EXTI_ClearITPendingBit(WAKE_Line);

}
#else
void key_isr(void)
{
}

void key_scan(void)
{
	char keys[] =
	    { KEY_USER, KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN, KEY_SEL, KEY_TAMP, KEY_WAKE };
	static unsigned int kls = 0;
	unsigned int kcs, res;
	int key, i;

	kcs = 0;
//    DISABLE_IRQ();
	/* "TAMP" key is pressed */
	if (!GPIO_ReadInputDataBit(GPIOC, TAMP_Pin)) {
		kcs |= 1 << KEY_TAMP;
	}

	/* "WAKE" key is pressed */
	if (!GPIO_ReadInputDataBit(GPIOA, WAKE_Pin)) {
		kcs |= 1 << KEY_WAKE;
	}


	if (!GPIO_ReadInputDataBit(GPIOG, USER_Pin)) {
		kcs |= 1 << KEY_USER;
	}

	if (!GPIO_ReadInputDataBit(GPIOG, RIGHT_Pin))
#if REVERSEKEY
		kcs |= 1 << KEY_LEFT;
#else
		kcs |= 1 << KEY_RIGHT;
#endif

	if (!GPIO_ReadInputDataBit(GPIOG, LEFT_Pin))
#if REVERSEKEY
		kcs |= 1 << KEY_RIGHT;
#else
		kcs |= 1 << KEY_LEFT;
#endif
	if (!GPIO_ReadInputDataBit(GPIOG, UP_Pin))
#if REVERSEKEY
		kcs |= 1 << KEY_DOWN;
#else
		kcs |= 1 << KEY_UP;
#endif

	if (!GPIO_ReadInputDataBit(GPIOD, DOWN_Pin))
#if REVERSEKEY
		kcs |= 1 << KEY_UP;
#else
		kcs |= 1 << KEY_DOWN;
#endif

	if (!GPIO_ReadInputDataBit(GPIOG, SELECT_Pin)) {
		kcs |= 1 << KEY_SEL;
	}
//    ENABLE_IRQ();

	res = kls ^ kcs;
	i = 0;
	while (res) {
		//有按键状态发生变化
		if ((res >> keys[i]) & 1) {
			key = keys[i];	//默认按下 
			if (!(kcs >> keys[i] & 1)) {
				//keys[i]放开
				key += KEY_UP_DATA;
			}
			res &= ~(1 << keys[i]);

			OSQPost(EventKey, (void *) key);
		}
		++i;
	}
	kls = kcs;
}
#endif


/*=============================================================================
* Function	:       按键初始化
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int key_init(void)
{
	KEYS_GPIO_Configuration();
#if KEYS_USE_EINT
	KEYS_Interrupt_Init();
#endif
	//创建uC/OS-II key 消息队列
	EventKey = OSQCreate(qKey, QKEYSIZE);
	if (EventKey == (OS_EVENT *) 0)
		return -1;
	return 0;
}

/*=============================================================================
* Function	:       key_read
* Description	:   按键读取函数，如果没有按键则会阻塞程序，不能在中断中调用
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int key_read(void)
{
	int key;
	void *ret;
	INT8U err;
	ret = OSQPend(EventKey, 0, &err);
	key = (int) ret;
	if (0 == key)
		key = -1;
	return key;
}

/*=============================================================================
* Function	:      key_accept
* Description	:  按键无阻塞读取函数
* Input Para	: 
* Output Para	:  
* Return Value  :  -1； 无按键，或发生错误, >0 按键值
=============================================================================*/
int key_accept(void)
{
	int key;
	void *ret;
	INT8U err;
	ret = OSQAccept(EventKey, &err);
	key = (int) ret;
	if (0 == key)
		key = -1;
	return key;

}

/*=============================================================================
* Function	:       清空按键缓冲
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value  : 
=============================================================================*/
int key_flush(void)
{
//	OSQFlush(EventKey);
	return 0;
}
