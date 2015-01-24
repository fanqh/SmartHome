#ifndef _INFRARED_H
#define _INFRARED_H

#include"Include.h"

#define INFRARED_LEN   256
typedef struct
{
	uint16 len;
	uint8 buff[INFRARED_LEN];	
}infrared_data_t;

uint8 InfraredEnterStudy(void);
uint8 InfraredExitStudy(void);
uint8 InfraredReset(void);
uint8 ParseInfrared(infrared_data_t*  ptr);



#endif


