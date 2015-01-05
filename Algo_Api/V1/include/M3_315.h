#ifndef M3_315_H
#define M3_315_H


#include"Include.h"

#define M315_DATA_LEN  3

extern volatile uint16	RF315_TimeCount;

typedef struct
{
	uint32 TimeBase;
	uint8  buff[M315_DATA_LEN];	
	uint8  reserve;
}RF315_DATA_t;

typedef enum
{
	RF315_IDLE,
	RF315_SENDING,
	RF315_RECEIVING
}RF315_STATA;

void    m3_315_io_config(void);


uint8 Get_rf315_flag(void);

int RF_decode(RF315_DATA_t *pdata);

void RF315_Rec(void);
int RF315_Send(RF315_DATA_t *pdata);

#endif


