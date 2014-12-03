// ========================================================
/// @file       DK_RFM69H.c
/// @brief      RFM69H basic application
/// @version    V2.1
/// @date       2013/12/25
/// @company    HOPE MICROELECTRONICS Co., Ltd.
/// @website    http://www.hoperf.com
/// @author     Geman Deng
/// @mobile     +86-013244720618
/// @tel        0755-82973805 Ext:846
// ========================================================
#include "stm32f10x.h"
#include"Include.h"
#include "spi.h"
#include "DK_RFM.h"

/************************Description************************
                      ________________
                     /                \
                    /      HOPERF      \
                   /____________________\
                  |      DK_RFM69H       |
    (MCU)DIO1 --- | DIO4             GND | --- GND
    (MCU)DIO0 --- | DIO3              A7 | --- 
  (MCU)RFData --- | DIO2              A6 | ---  
   (MCU)nIRQ1 --- | DIO1              A5 | --- 
   (MCU)nIRQ0 --- | DIO0              A4 | --- 
    (MCU)MISO --- | MISO              A3 | --- 
    (MCU)MOSI --- | MOSI              A2 | --- 
     (MCU)SCK --- | SCK               A1 | --- 
     (MCU)nCS --- | NSS               A0 | --- 
  (MCU)PORChk --- | RESET            VCC | --- VCC             
                  |______________________|
                  
                  

//  RF module:           RFM69H
//  Carry Frequency:     315MHz/434MHz/868MHz/915MHz
//  Bit Rate:            1.2Kbps/2.4Kbps/4.8Kbps/9.6Kbps
//  Tx Power Output:     20dbm/17dbm/14dbm/11dbm
//  Frequency Deviation: +/-35KHz
//  Receive Bandwidth:   83KHz
//  Coding:              NRZ
//  Packet Format:       0x5555555555+0xAA2DD4+"HopeRF RFM COBRFM69HS" (total: 29 Bytes)
//  Tx Current:          about 130mA  (RFOP=+20dBm,typ.)
//  Rx Current:          about 16mA  (typ.)                 
**********************************************************/

/**********************************************************
**Parameter table define	  {0x076c, 0x084f, 0x09f8}, //433MHz
**********************************************************/
const u16 RFM69HFreqTbl[4][3] = 
{ 
  {0x074e, 0x08c0, 0x0900}, //315MHz
  {0x076c, 0x0880, 0x0900}, //434MHz
  {0x07d9, 0x0800, 0x0900}, //868MHz
  {0x07e4, 0x08c0, 0x0900}, //915MHz
};



const u16 RFM69HPowerTbl[4] = 
{
  0x117F,                   //20dbm  
  0x117C,                   //17dbm
  0x1179,                   //14dbm
  0x1176,                   //11dbm 
};

const u16 RFM69RateTbl[4][2] = 
{
  {0x0368, 0x042B},         //BR=1.2K
  {0x0334, 0x0415},         //BR=2.4K 
  {0x031A, 0x040B},         //BR=4.8K 
  {0x030D, 0x0405},         //BR=9.6K
};


#if 0  //刘斌
const u16 RFM69ConfigTbl[14] = 
{ 
          0x031A	   ,
          0x040B        ,//BR=4.8K
          0x117F        ,//20dbm;

          0x0268        ,//RegDataModul, ASK CONTINUOUS  	0x68
          0x0502        ,//RegFdevMsb, 241*61Hz = 35KHz  
          0x0641        ,//RegFdevLsb
          0x1942        ,//RegRxBw , RxBW, 125KHz 		 0x42

          0x2C00        ,//RegPreambleMsb  
          0x2D00        ,//RegPreambleLsb, 4Byte Preamble
          0x2E00        ,//enable Sync.Word, 3+1=4bytes 
  
          0x1888        ,//RegLNA, 200R  
          0x581B        ,//RegTestLna, Normal sensitivity
          0x6F30        ,//RegTestDAGC, Improved DAGC

//		  0x3700,
//		0x1b78,


          0x0104        ,//Enter standb 
};
#else

const u16 RFM69ConfigTbl[14] = 
{ 
  0x0268,                   //RegDataModul,OOK, Continuous mode with bit synchronizer （官网demo ox48 打开同步时钟，可以与速率配合使用） 
  0x130F,                   //RegOcp, Disable OCP	//overload current protect
  0x0502,                   //RegFdevMsb, 241*61Hz = 35KHz  
  0x0641,                   //RegFdevLsb

  0x19F0,//0x19E9,//0x19E8  //RegRxBw , RxBW, 150KHz（打开接收带宽设置，OOK效果显著）
  0x1A8B,//0x1A8B,//0x1A8B  
  0x1E10,                   //AFC

  0x1B40,                   //RegOokPeak
  0x1C80,                   //RegOokAvg
  0x1D06,                   //OokFixedThresh
 
  0x1888,                   //RegLNA, 200R  (可以修改AGC等级，提高噪声门槛)
  0x581B,                   //RegTestLna, Normal sensitivity
  0x6F00,                   //RegTestDAGC, 
  0x0104,                   //Enter standby mode    
};

#endif

const u16 RFM69HRxTbl[6] = 
{   
  0x119F,                   //      
  0x2544,                   //DIO Mapping for Rx
  0x1310,                   //OCP enabled
  0x5A55,                   //Normal and Rx
  0x5C70,                   //Normal and Rx   
  0x0110                    //Enter Rx mode
};         
    
const u16 RFM69HTxTbl[5] = 
{
  0x2504,                   //DIO Mapping for Tx
  0x130F,                   //Disable OCP
  0x5A5D,                   //High power mode
  0x5C7C,                   //High power mode
  0x010C,                   //Enter Tx mode
};
/**********************************************************
**Variable define
**********************************************************/

#define RFM69H_DATA   PBin(2)


RFM69H_INFOR rfm69h_infor;


/**********************************************************
**Name:     RFM69H_Config
**Function: Initialize RFM69H & set it entry to standby mode
**Input:    none
**Output:   none
**********************************************************/
void RFM69H_Config(void)
{
  u8 i;
  
  for(i=0;i<3;i++)                      
	SPIWrite(SPI_2, RFM69HFreqTbl[1][i]);           //setting frequency parameter
  for(i=0;i<2;i++)
	SPIWrite(SPI_2, (RFM69RateTbl[1][i]));           //setting rf rate parameter
  for(i=0;i<14;i++)
    SPIWrite(SPI_2, RFM69ConfigTbl[i]);                          //setting base parameter

  rfm69h_infor.RFM69H_State = RFM69H_IDLE;
}


/**********************************************************
**Name:     RFM69H_EntryRx
**Function: Set RFM69H entry Rx_mode
**Input:    None
**Output:   "0" for Error Status
**********************************************************/
void RFM69H_EntryRx(void)
{
  u8 i; 

  RFM69H_Config();                                         //Module parameter setting
  for(i=0;i<6;i++)                                         //Define to Rx mode  
    SPIWrite(SPI_2, RFM69HRxTbl[i]);
}

/**********************************************************
**Name:     RFM69H_EntryTx
**Function: Set RFM69H entry Tx_mode
**Input:    None										  
**Output:   "0" for Error Status
**********************************************************/
void RFM69H_EntryTx(void)
{
  u8 i;
  RFM69H_Config();                                         //Module parameter setting  
  SPIWrite(SPI_2, RFM69HPowerTbl[0]);              //Setting output power parameter
  SPIWrite(SPI_2, RFM69HPowerTbl[0]);              //Setting output power parameter
  for(i=0;i<5;i++)                                         //Define to Tx mode  
    SPIWrite(SPI_2, RFM69HTxTbl[i]);
}

/**********************************************************
**Name:     RFM69H_TxWaitStable
**Function: Determine whether the state of stable Tx
**Input:    none
**Output:   none
**********************************************************/
u8 RFM69H_TxWaitStable(void)
{ 
  uint16_t timeout = 0;	
  uint8_t temp;
  
  temp = SPIRead(SPI_2, 0x27);
  while(1)
  {
  	if((temp&0xA0)==0xA0 && temp!=0xff)
	{
		rfm69h_infor.RFM69H_State = RFM69H_SEND;
		return 1;
	}
	else
	{
	  	timeout ++;
		if(timeout >= 500)
			return 0;
	//  	delay_ms(10);
		temp = 	SPIRead( SPI_2, 0x27);
	}
  }
}

/**********************************************************
**Name:     RFM69H_RxWaitStable
**Function: Determine whether the state of stable Rx
**Input:    none
**Output:   none
**********************************************************/
u8 RFM69H_RxWaitStable(void)
{ 
  uint16_t timeout = 0;	
  uint8_t temp;
  
  temp = SPIRead(SPI_2, 0x27);
  while(1)
  {
  	if((temp&0xC0)==0xC0 && temp!=0xff)
	{
		rfm69h_infor.RFM69H_State = RFM69H_SEND;
		return 1;
	}
	else
	{
	  	timeout ++;
		if(timeout >= 50)
			return 0;
	  	delay_ms(10);
		temp = 	SPIRead(SPI_2, 0x27);
	}
  }
}

/**********************************************************
**Name:     RFM69H_ClearFIFO
**Function: Change to RxMode from StandbyMode, can clear FIFO buffer
**Input:    None
**Output:   None
**********************************************************/
void RFM69H_ClearFIFO(void)
{
  SPIWrite(SPI_2, 0x0104);                                        //Standby
  SPIWrite(SPI_2, 0x0110);                                        //entry RxMode
}

/**********************************************************
**Name:     RFM69H_Sleep
**Function: Set RFM69H to sleep mode 
**Input:    none
**Output:   none
**********************************************************/
void RFM69H_Sleep(void)
{
  SPIWrite(SPI_2, 0x0100);                                        //Sleep
}

/**********************************************************
**Name:     RFM69H_Standby
**Function: Set RFM69H to Standby mode
**Input:    none
**Output:   none
**********************************************************/
void RFM69H_Standby(void)
{
  SPIWrite(SPI_2, 0x0104);                                        //Standby
}

/**********************************************************
**Name:     RFM69H_ReadRSSI
**Function: Read the RSSI value
**Input:    none
**Output:   temp, RSSI value
**********************************************************/
u8 RFM69H_ReadRSSI(void)
{
  u16 temp=0xff;
  if((SPIRead(SPI_2, 0x24)&0x02)==0x02)
  {
    temp=SPIRead(SPI_2, 0x24);
    temp=0xff-temp;
    temp>>=1;
    temp&=0xff;
  }
  return (u8)temp;
}


/**********************************************************
**Name:     RFM69H_RxPacket
**Function: Check for receive one packet
**Input:    none
**Output:   "!0"-----Receive one packet
**          "0"------Nothing for receive
**********************************************************/
u8 RFM69H_RxPacket(uint8 *pbuff)
{
  uint16_t timeout =0; 
 
  if(RFM69H_RxWaitStable())
  {

//  	delay_ms(100);
//  	while(!nIRQ0)
//	{
//		timeout ++;
//		if(timeout >= 1000)
//			return 0;
//		delay_ms(10);
//	//	Boot_UsartSend("67",2);
//	}
////	Boot_UsartSend("12",2);
//	SPIBurstRead(SPI_2, 0x00, pbuff, RxBuf_Len);  
//    RFM69H_ClearFIFO();  
	return 1;
  }
  else										  
  	return 0;
}

/**********************************************************
**Name:     RFM69H_TxPacket
**Function: Check RFM69H send over & send next packet
**Input:    none
**Output:   TxFlag=1, Send success
**********************************************************/
u8 RFM69H_TxPacket(u8* pSend)
{
  uint16_t timeout = 0;

  if(RFM69H_TxWaitStable())
  {
  	 BurstWrite(SPI_2, 0x00, (u8 *)pSend, TxBuf_Len);              //Send one packet data	
	 while(!nIRQ0)
	 {
	  	timeout ++;
		if(timeout>=50)
		{
			RFM69H_Config();
			return 0;
		}
//		delay_ms(10);
	 }
	 return 1;
  }
  else
  	return 0;
}

u8 RFM69H_Analysis(void)
{
	static time = 0;
	uint16 i = 0;

	while(1)
	{
		Delay_
	}

	while(i<RFM69H_DATA_LEN)
	{
//		__disable_irq();
//		rfm69h_infor.RF69H_TimeCount = 1;  
//        __enable_irq();

					
	}

}



