#ifndef M3_315_H
#define M3_315_H


#include"Include.h"


#define M3_315_DATA_PIN         GPIO_Pin_8
#define M3_315_DATA_PORT        GPIOA

#define M3_315_CTRL             GPIO_Pin_3
#define M3_315_GPIO             GPIOB


#define RF315_REC_GPIO          GPIO_Pin_3
#define RF315_REC_PORT          GPIOB

#define RF315_SEND_GPIO             GPIO_Pin_6
#define RF315_SEND_PORT             GPIOA


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


