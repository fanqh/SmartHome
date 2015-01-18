#ifndef __DECODE_H_
#define __DECODE_H_




#define LEN_MAX     256
typedef struct
{
	uint32 TimeBase;
	uint16  len;
	uint8  buff[LEN_MAX];	
	uint16  reserve;
}RF_AC_DATA_TYPE;
//RF315_DATA_t;




extern volatile uint16	TimeCount;

uint16 RFDecodeAC(RF_AC_DATA_TYPE *pdata, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
int RFCodeAC_Send(RF_AC_DATA_TYPE *pdata , GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8 GetFlagTimeCount(void);


#endif


