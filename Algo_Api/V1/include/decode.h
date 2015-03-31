#ifndef __DECODE_H_
#define __DECODE_H_


#include <stdbool.h>
#define LEN_MAX     256
typedef struct
{
	uint32 TimeBase;
	uint16  len;
	uint16  type; //0£º pt2262   1£º Å·Èð²© 
	uint16  buff[LEN_MAX];	
}RF_AC_DATA_TYPE;
//RF315_DATA_t;


//typedef struct
//{
//	uint8 id;
//	bool crc;	
//}ORVIBO_DATA_TYPE;




extern volatile uint16	TimeCount;

uint16 RFDecodeAC(RF_AC_DATA_TYPE *pdata, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
int RFCodeAC_Send(RF_AC_DATA_TYPE *pdata , GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8 GetFlagTimeCount(void);


#endif


