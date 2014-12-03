#ifndef __DELAY_H_
#define __DELAY_H_


void SysTick_Init(unsigned int ticks);
void Delay_Init(void);
void Delay_MS(u16 nms);
void Delay_US(u32 nus);
void Delay_S(vu16 s);

#endif



