#ifndef _INFRARED_H
#define _INFRARED_H

#include"Include.h"

#define INFRARED_SNED           GPIO_Pin_6
#define INFRARED_RECIEVE        GPIO_Pin_5

#define INFRARED_PORT           GPIOA

#define SET_INFRARED            INFRARED_PORT->BSRR=INFRARED_SNED
#define CLR_INFRARED            INFRARED_PORT->BRR=INFRARED_SNED
#define READ_INFRARED           GPIO_ReadInputDataBit(INFRARED_PORT, INFRARED_RECIEVE)
#if 0
#define SET_INFRARED            GPIO_SetBits(INFRARED_PORT, INFRARED_SNED)
#define CLR_INFRARED            GPIO_ResetBits(INFRARED_PORT, INFRARED_SNED)
#define READ_INFRARED           GPIO_ReadInputDataBit(INFRARED_PORT, INFRARED_RECIEVE)
#endif

void    infrared_io_init(void);

void    infrared_send_data(uint8 data);

uint8   infrared_read_data(void);



#endif


