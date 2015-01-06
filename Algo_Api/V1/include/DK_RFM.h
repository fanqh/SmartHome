#ifndef __DK_RFM_h__
#define __DK_RFM_h__
#include "gpio_config.h"



#define  nIRQ0			   PBin(2)
#define  TxBuf_Len 		   10 
#define  RxBuf_Len 		   10


#define RFM69H_DATA_LEN	 64

#define  RFM69H_DATA_IN     PBin(11)
#define  RFM69H_DATA_OUT    PBout(11)

typedef enum
{
	RFM69H_IDLE,
	RFM69H_RECEIVE,
	RFM69H_SENDING
}RFM69H_STATE;

typedef enum
{
	IDLE,
	ACTIVING,
	PULSE_HIG,
	PULSE_LOW
}DataState_t;

typedef struct 
{
	uint32 HoldTime  : 15;
	uint32 pulse     : 1;
}data_t;


typedef struct
{
	uint16 len;
	data_t  buff[RFM69H_DATA_LEN];
}RFM69H_DATA_Type;

//typedef	struct
//{
//	RFM69H_DATA_Type	Data;	   //脉冲数据长度
//
//}RFM69H_INFOR;

extern RFM69H_DATA_Type TxBuf;  
extern RFM69H_DATA_Type RxBuf;

extern volatile uint32	DataTimeCount;
//extern RFM69H_DATA_Type rfm69h_data;



//void RFM69H_Running(u8 mode,u8 WorkStatus,u8 ParaChangeFlag,u8 *TxFlag,u8 *RxFlag,u8 *RSSI);
void RFM69H_Config(void);
int RFM69H_RxPacket(RFM69H_DATA_Type* p);
u8 RFM69H_TxPacket(RFM69H_DATA_Type* pSend);
void RFM69H_EntryTx(void);
void RFM69H_EntryRx(void);
//u8 RFM69H_RxWaitStable(void);
//u8 RFM69H_TxWaitStable(void);
RFM69H_STATE  Get_RFM69H_Status(void);
//int RFM69H_Analysis(RFM69H_DATA_Type* pReceive);	
//void RFM69H_SendData(RFM69H_DATA_Type *p);

int RFM69H_Send(RFM69H_DATA_Type *pData);


#endif







