#ifndef __DK_RFM_h__
#define __DK_RFM_h__
#include "gpio_config.h"


//void RFM69H_Running(u8 mode,u8 WorkStatus,u8 ParaChangeFlag,u8 *TxFlag,u8 *RxFlag,u8 *RSSI);
void RFM69H_Config(void);
u8 RFM69H_RxPacket(uint8* pbuff);
u8 RFM69H_TxPacket(u8* pSend);
void RFM69H_EntryTx(void);
void RFM69H_EntryRx(void);
u8 RFM69H_RxWaitStable(void);
u8 RFM69H_TxWaitStable(void);

#define  nIRQ0			   PBin(2)
#define  TxBuf_Len 		   10 
#define  RxBuf_Len 		   10



#define RFM69H_DATA_LEN	 512

typedef enum
{
	RFM69H_IDLE,
	RFM69H_RECEIVE,
	RFM69H_SEND
}RFM69H_STATE;

typedef union 
{
	uint16 data  : 15;
	uint16 pulse :	1;
}Data_t;

typedef struct
{
	uint16 len;
	Data_t  buff[RFM69H_DATA_LEN];
}RFM69H_DATA_Type;

typedef	struct
{
	RFM69H_STATE 		RFM69H_State;
	RFM69H_DATA_Type	RFM69H_Data;
	uint16	RF69H_TimeCount;

}RFM69H_INFOR;

extern RFM69H_INFOR rfm69h_infor; 

extern unsigned char TxBuf[TxBuf_Len];  
extern unsigned char RxBuf[RxBuf_Len];

#endif







