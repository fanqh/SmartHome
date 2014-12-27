#ifndef M3_315_H
#define M3_315_H


#include"Include.h"

extern volatile uint16	RF315_TimeCount;

void    m3_315_io_config(void);

void    m3_315_set(void);

void    m3_315_clr(void);

uint8 Get_rf315_flag(void);

void RF_decode(void);

void RF315_Rec(void);

#endif


