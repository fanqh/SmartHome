#ifndef __DK_RFM_h__
#define __DK_RFM_h__
#include "gpio_config.h"


//void RFM69H_Running(u8 mode,u8 WorkStatus,u8 ParaChangeFlag,u8 *TxFlag,u8 *RxFlag,u8 *RSSI);
void RFM69H_Config(void);
u8 RFM69H_RxPacket(void);
u8 RFM69H_TxPacket(u8* pSend);
void RFM69H_EntryTx(void);
void RFM69H_EntryRx(void);

#define  nIRQ0			   PBin(2)
#define  TxBuf_Len 		   10 
#define  RxBuf_Len 		   10  

extern unsigned char TxBuf[TxBuf_Len];  
extern unsigned char RxBuf[RxBuf_Len];

#endif







