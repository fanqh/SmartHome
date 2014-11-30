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
**Parameter table define
**********************************************************/
const u16 RFM69HFreqTbl[4][3] = 
{ 
  {0x074e, 0x08c0, 0x0900}, //315MHz
//  {0x076c, 0x0880, 0x0900}, //434MHz
{0x076c, 0x084f, 0x09f8}, //433MHz
  {0x07d9, 0x0800, 0x0900}, //868MHz
  {0x07e4, 0x08c0, 0x0900}, //915MHz
};

const u16 RFM69HRateTbl[4][2] = 
{
  {0x0368, 0x042B},         //BR=1.2K BW=83.333K
  {0x0334, 0x0415},         //BR=2.4K BW=83.333K  
  {0x031A, 0x040B},         //BR=4.8K BW=83.333K
  {0x030D, 0x0405},         //BR=9.6K BW=83.333K
};

const u16 RFM69HPowerTbl[4] = 
{
  0x117F,                   //20dbm  
  0x117C,                   //17dbm
  0x1179,                   //14dbm
  0x1176,                   //11dbm 
};

#if 0
const u16 RFM69HConfigTbl[17] = 
{ 
  0x0200,                   //RegDataModul, FSK Packet  
  0x0502,                   //RegFdevMsb, 241*61Hz = 35KHz  
  0x0641,                   //RegFdevLsb
  0x1952,                   //RegRxBw , RxBW, 83KHz
  
  0x2C00,                   //RegPreambleMsb  
  0x2D05,                   //RegPreambleLsb, 5Byte Preamble
  0x2E98,              //2e09     //enable Sync.Word, 2+1=3bytes
  0x2FAA,           //同步字节不知道是否影响        //0xAA, SyncWord = aa2dd4 
  0x302D,                  //0x2D
  0x31D4,                   //0xD4  
  0x3710,            //ox3710       //RegPacketConfig1, Disable CRC，NRZ encode
  0x3815,                   //RegPayloadLength, 21bytes for length & Fixed length
  0x3C95,                   //RegFiFoThresh   
  
  0x1888,                   //RegLNA, 200R  
  0x581B,                   //RegTestLna, Normal sensitivity
  //0x582D,                   //RegTestLna, increase sensitivity with LNA (Note: consumption also increase!)
  0x6F30,                   //RegTestDAGC, Improved DAGC
  //0x6F00,                   //RegTestDAGC, Normal DAGC
  0x0104,                   //Enter standby mode    
};

#else
const u16 RFM69HConfigTbl[17] = 
{ 
  0x0200,                   //RegDataModul, FSK Packet  
  0x0502,                   //RegFdevMsb, 241*61Hz = 35KHz  
  0x0641,                   //RegFdevLsb
  0x1952,                   //RegRxBw , RxBW, 83KHz
  
  0x2C00,                   //RegPreambleMsb  
  0x2D05,                   //RegPreambleLsb, 5Byte Preamble
  0x2E90,                   //enable Sync.Word, 2+1=3bytes
  0x2FAA,                   //0xAA, SyncWord = aa2dd4
  0x302D,                   //0x2D
  0x31D4,                   //0xD4  
  0x3700,                   //RegPacketConfig1, Disable CRC，NRZ encode
  0x3815,                   //RegPayloadLength, 21bytes for length & Fixed length
  0x3C95,                   //RegFiFoThresh   
  
  0x1888,                   //RegLNA, 200R  
  0x581B,                   //RegTestLna, Normal sensitivity
  //0x582D,                   //RegTestLna, increase sensitivity with LNA (Note: consumption also increase!)
  0x6F30,                   //RegTestDAGC, Improved DAGC
  //0x6F00,                   //RegTestDAGC, Normal DAGC
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

#define nIRQ0   PBin(2)


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
    //SPIWrite(RFM69HFreqTbl[gb_FreqBuf_Addr][i]);           //setting frequency parameter
	SPIWrite(RFM69HFreqTbl[1][i]);           //setting frequency parameter
  for(i=0;i<2;i++)
    //SPIWrite(RFM69HRateTbl[gb_RateBuf_Addr][i]);           //setting rf rate parameter
	SPIWrite(RFM69HRateTbl[0][i]);           //setting rf rate parameter
  for(i=0;i<17;i++)
    SPIWrite(RFM69HConfigTbl[i]);                          //setting base parameter
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
    SPIWrite(RFM69HRxTbl[i]);
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
//  SPIWrite(RFM69HPowerTbl[gb_PowerBuf_Addr]);              //Setting output power parameter
  SPIWrite(RFM69HPowerTbl[0]);              //Setting output power parameter
  for(i=0;i<5;i++)                                         //Define to Tx mode  
    SPIWrite(RFM69HTxTbl[i]);
}
#if 0
/**********************************************************
**Name:     RFM69H_TxWaitStable
**Function: Determine whether the state of stable Tx
**Input:    none
**Output:   none
**********************************************************/
void RFM69H_TxWaitStable(void)
{ 
  u8 temp=0;
  if(gb_WaitStableFlag==0x0f) 
  {
    if(gb_SysTimer_1S!=0)
    {
      temp=SPIRead(0x27);
      if((temp&0xA0)==0xA0 && temp!=0xff)
      {
        gb_WaitStableFlag=1; 
      }
    }
    else
    {
      gb_ErrorFlag=1;
      gb_WaitStableFlag=0;
    }
  }
}

/**********************************************************
**Name:     RFM69H_RxWaitStable
**Function: Determine whether the state of stable Rx
**Input:    none
**Output:   none
**********************************************************/
void RFM69H_RxWaitStable(void)
{ 
  u8 temp=0;
  if(gb_WaitStableFlag==0x0f) 
  {
    if(gb_SysTimer_1S!=0)
    {
      temp=SPIRead(0x27);
      if((temp&0xC0)==0xC0 && temp!=0xff)
      {
        gb_WaitStableFlag=1;        
      }
    }
    else
    {
      gb_ErrorFlag=1;
      gb_WaitStableFlag=0;
    }
  }
}
#else 

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
  
  temp = SPIRead(0x27);
  while(1)
  {
  	if((temp&0xA0)==0xA0 && temp!=0xff)
	{
		//Boot_UsartSend(&temp,1);
		return 1;
	}
	else
	{
	  	timeout ++;
		if(timeout >= 500)
			return 0;
	//  	delay_ms(10);
		temp = 	SPIRead(0x27);
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
  
  temp = SPIRead(0x27);
  while(1)
  {
  	if((temp&0xC0)==0xC0 && temp!=0xff)
	{
		//Boot_UsartSend(&temp,1);
		return 1;
	}
	else
	{
	  	timeout ++;
		if(timeout >= 500)
			return 0;
	//  	delay_ms(10);
		temp = 	SPIRead(0x27);
	}
  }
}


#endif
/**********************************************************
**Name:     RFM69H_ClearFIFO
**Function: Change to RxMode from StandbyMode, can clear FIFO buffer
**Input:    None
**Output:   None
**********************************************************/
void RFM69H_ClearFIFO(void)
{
  SPIWrite(0x0104);                                        //Standby
  SPIWrite(0x0110);                                        //entry RxMode
}

/**********************************************************
**Name:     RFM69H_Sleep
**Function: Set RFM69H to sleep mode 
**Input:    none
**Output:   none
**********************************************************/
void RFM69H_Sleep(void)
{
  SPIWrite(0x0100);                                        //Sleep
}

/**********************************************************
**Name:     RFM69H_Standby
**Function: Set RFM69H to Standby mode
**Input:    none
**Output:   none
**********************************************************/
void RFM69H_Standby(void)
{
  SPIWrite(0x0104);                                        //Standby
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
  if((SPIRead(0x24)&0x02)==0x02)
  {
    temp=SPIRead(0x24);
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
  	while(!nIRQ0)
	{
		timeout ++;
		if(timeout >= 1000)
			return 0;
//		delay_ms(10);
	//	Boot_UsartSend("67",2);
	}
	Boot_UsartSend("12",2);
	SPIBurstRead(0x00, pbuff, RxBuf_Len);  
    RFM69H_ClearFIFO();  
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
  	 BurstWrite(0x00, (u8 *)pSend, TxBuf_Len);              //Send one packet data	
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





#if TEST
/**********************************************************
**Parameter table define in test mode
**********************************************************/
const u16 RFM69HTestConfigTbl[12] = 
{ 
  0x0248,                   //RegDataModul, OOK
  0x0502,                   //RegFdevMsb, 241*61Hz = 35KHz  
  0x0641,                   //RegFdevLsb
  0x1952,                   //RegRxBw , RxBW, 83KHz
  0x1888,                   //RegLNA, 200R  
  0x2C00,                   //RegPreambleMsb  
  0x2D00,                   //RegPreambleLsb, 0Byte Preamble
  0x2E00,                   //Disable Sync
  0x3700,                   //RegPacketConfig1, Disable CRC，NRZ encode
  0x581B,                   //RegTestLna, Normal sensitivity
  0x6F30,                   //RegTestDAGC
  0x0104                    //Enter standby mode  
};

/**********************************************************
**Name:     RFM69H_TestConfig
**Function: Initialize RFM69H in test mode & set it entry to standby mode
**Input:    none
**Output:   none
**********************************************************/
void RFM69H_TestConfig(void)
{
  u8 i;
  for(i=0;i<3;i++)                      
    SPIWrite(RFM69HFreqTbl[gb_FreqBuf_Addr][i]);           //setting frequency parameter
  for(i=0;i<2;i++)
    SPIWrite(RFM69HRateTbl[gb_RateBuf_Addr][i]);           //setting rf rate parameter
  for(i=0;i<12;i++)
    SPIWrite(RFM69HTestConfigTbl[i]);                      //setting base parameter
}

/**********************************************************
**Name:     RFM69H_EntryTestRx
**Function: Set RFM69H entry Rx test mode
**Input:    None
**Output:   None
**********************************************************/
void RFM69H_EntryTestRx(void)
{
  u8 i;
  Input_RFData(); 
  RFM69H_Config();                                         //Module parameter setting
  for(i=0;i<6;i++)                                         //Define to Rx mode  
    SPIWrite(RFM69HRxTbl[i]);
  
  gb_SysTimer_1S=3;                                        //System time = 3S
  gb_StatusTx=0;                                           //Clear Tx status flag 
  gb_WaitStableFlag=0x0f;                                  //State stable flag initial
}

/**********************************************************
**Name:     RFM69H_EntryTestTx
**Function: Set RFM69H entry Tx test mode
**Input:    None
**Output:   None
**********************************************************/
void RFM69H_EntryTestTx(void)
{
  u8 i;
  
  Output_RFData();                                         //DIO2=0                                 
  RFData=0;
  RFM69H_TestConfig();
  SPIWrite(RFM69HPowerTbl[gb_PowerBuf_Addr]);              //Setting output power parameter
  for(i=0;i<5;i++)                                         //Define to Tx mode  
    SPIWrite(RFM69HTxTbl[i]);
    
  gb_SysTimer_1S=3;                                        //System time = 3S
  gb_StatusRx=0;                                           //Clear Rx status flag 
  gb_WaitStableFlag=0x0f;                                  //State stable flag initial
}



/**********************************************************
**Name:     RFM69H_TestRx
**Function: RFM69H Rx test mode
**Input:    None
**Output:   "0" for Error Status
**********************************************************/
void RFM69H_TestRx(void)
{
  RFM69H_RxWaitStable();
  if(gb_WaitStableFlag==1)
  {
    gb_WaitStableFlag=2;
    gb_StatusRx=1;    
  }
}

/**********************************************************
**Name:     RFM69H_TestTx
**Function: RFM69H Tx test mode
**Input:    None
**Output:   "0" for Error Status
**********************************************************/
void RFM69H_TestTx(void)
{ 
  RFM69H_TxWaitStable();
  if(gb_WaitStableFlag==1)
  {
    gb_WaitStableFlag=2;
    gb_StatusTx=1;  
                                        
    RFData=1;                                              //DIO2=1
  }
}


u8 gb_WorkmodeBackup=0;
/**********************************************************
**Name:     RFM69H_Running
**Function: RFM69H running function
**Input:    mode, work mode(FS-T,FS-R,T-T,T-R)
            WorkStatus, work status(normal,standby,sleep)
            ParaChangeFlag, Parameter changed when ParaChangeFlag=1
            *TxFlag, Tx success flag affect Tx count
            *RxFlag, Rx success flag affect Rx count
            *RSSI, RSSI value
**Output:   none
**********************************************************/
void RFM69H_Running(u8 mode,u8 WorkStatus,u8 ParaChangeFlag,u8 *TxFlag,u8 *RxFlag,u8 *RSSI)
{ 
  u8 temp;  
  if(WorkStatus==C_ModuleWorkMode_FSK)                  	 //Normal status
  {
    switch(mode)
    {
      case C_SysMode_FSKTx:                                //Normal send mode(Abbreviation:FS-T)
        if(ParaChangeFlag==1)                              //Parament changed
        {                   
          RFM69H_EntryTx();
        }         
        temp=RFM69H_TxPacket(); 
        if(temp==1){*TxFlag=1;}
        break;
      case C_SysMode_FSKRx:                                //Normal receive mode(Abbreviation:FS-R)      
        if(ParaChangeFlag==1)
        {           
          RFM69H_EntryRx();
        }
        temp=RFM69H_RxPacket(); 
        if(temp==1)
        {
          *RxFlag=1;
          BeepOn((u16*)MusicTab_RxSuccess);                //Buzzer on When received data success
        }
        
        *RSSI=RFM69H_ReadRSSI();                           //Read RSSI
        break;
      case C_SysMode_TestTx:                               //Test send mode(Abbreviation:T-T)   
        if(ParaChangeFlag==1)
        {           
          RFM69H_EntryTestTx();
        }
        RFM69H_TestTx();
        break;
      case C_SysMode_TestRx:                               //Test receive mode(Abbreviation:T-R)  
        if(ParaChangeFlag==1)
        {       
          RFM69H_EntryTestRx();
        }
        RFM69H_TestRx();
        
        *RSSI=RFM69H_ReadRSSI();                           //Read RSSI
        break;
    } 
  }
  else if(WorkStatus==C_ModuleWorkMode_Standby)            //Standby status
  {
    if(gb_WorkmodeBackup!=C_ModuleWorkMode_Standby)
    	RFM69H_Standby();
  }
  else if(WorkStatus==C_ModuleWorkMode_Sleep)              //Sleep status
  {
    if(gb_WorkmodeBackup!=C_ModuleWorkMode_Sleep)
    	RFM69H_Sleep();
  }
  gb_WorkmodeBackup=WorkStatus;
}

#endif

