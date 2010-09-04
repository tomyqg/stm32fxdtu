
#include "stm32f10x_lib.h"
#include "gpio.h"
#include "aux_lib.h"
#include "ucos_ii.h"

#define Delay(m) /*OSTimeDly((m)*1000) */sleep_us((m)*1000)

#define CDRS_POWER_PIN GPIO_Pin_10
#define CDRS_RESET_PIN GPIO_Pin_6
#define CDRS_ONOFF_PIN GPIO_Pin_7

void CDRS_Init(void)
{
    gpio_init(GPIOB, CDRS_POWER_PIN, GPIO_Speed_50MHz, 
        GPIO_Mode_Out_OD, RCC_APB2Periph_GPIOB);

    gpio_init(GPIOC,
	  CDRS_ONOFF_PIN | CDRS_RESET_PIN, GPIO_Speed_50MHz, GPIO_Mode_Out_PP,
	  RCC_APB2Periph_GPIOC
	);
    
}

void CDRS_SetPower(int s)
{
    if (s > 0) {
        GPIO_SetBits(GPIOB, CDRS_POWER_PIN);
    } else {
        GPIO_ResetBits(GPIOB, CDRS_POWER_PIN);
    }
}
void CDRS_Reset(void)
{
    GPIO_SetBits(GPIOC, CDRS_RESET_PIN);
    Delay(100); //100MS
    GPIO_ResetBits(GPIOC, CDRS_RESET_PIN);
}
void CDRS_Open(void)
{
    GPIO_SetBits(GPIOC, CDRS_ONOFF_PIN);
//    Delay(10); //100MS cdma
    Delay(1800);   
    GPIO_ResetBits(GPIOC, CDRS_ONOFF_PIN);
}
void CDRS_Close(void)
{
    GPIO_SetBits(GPIOC, CDRS_ONOFF_PIN);
    Delay(1800); //2000MS
    GPIO_ResetBits(GPIOC, CDRS_ONOFF_PIN);
}
