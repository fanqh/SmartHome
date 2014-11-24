#ifndef _TIMER_H
#define _TIMER_H


#include"Include.h"


void    isr_13us(void);

void    timer2_init(void);

void    timer2_enable(void);

void    timer2_disable(void);

void    Enable_SysTick(void);

void    Disable_SysTick(void);

#endif


