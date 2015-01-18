#ifndef M3_315_H
#define M3_315_H


#include"Include.h"

//#define M315_DATA_LEN  3


extern volatile uint16	RF315_TimeCount;
extern volatile uint32 RF315_LearnTimeCount;



typedef enum
{
	RF315_IDLE,
	RF315_SENDING,
	RF315_RECEIVING
}RF315_STATA;

void    m3_315_io_config(void);


uint8 Get_rf315_flag(void);

//uint16 RF_decode(RF315_DATA_t *pdata);
//uint16 RF_decode(RF315_DATA_t *pdata, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) ;

//uint8 RF315_Rec(RF315_DATA_t *pdata);
//int RF315_Send(RF315_DATA_t *pdata);

#endif


