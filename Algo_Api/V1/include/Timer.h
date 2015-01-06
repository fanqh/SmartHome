#ifndef _TIMER_H
#define _TIMER_H


#include"Include.h"


typedef struct
{
	uint8 FlagStart;
	volatile uint16 TimeCount;
}LearnTimeCountTypeDef;

extern LearnTimeCountTypeDef LearnTimeCount;

void    isr_13us(void);

void    timer2_init(void);

void    timer2_enable(void);

void    timer2_disable(void);

void    Enable_SysTick(void);

void    Disable_SysTick(void);


void TIM3_NVIC_Configuration(void);
void BSP_uDelay (const uint32_t usec);
void BSP_mDelay (const uint32_t msec);

#endif


