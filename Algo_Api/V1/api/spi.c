#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "gpio_config.h"

#include "spi.h"

#define  SPI2_PIN_SCK      GPIO_Pin_13
#define  SPI2_PIN_MOSI     GPIO_Pin_15
#define  SPI2_PIN_MISO     GPIO_Pin_14
#define  SPI2_PIN_NSS      GPIO_Pin_12
#define  PIN_nIRQ0         GPIO_Pin_2

#define  SPI2_GPIO		   GPIOB

#define   nCS      PBout(12)
#define   SCK      PBout(13)   //  X    0     0    RFMxx sck
#define   MOSI     PBout(15)  //  X    0     1    RFMxx MOSI
#define   MISO     PBin(14)  //  X    1     0    RFMxx MISO

//#define  IRQ			   PBin(2)


void SpiMsterGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;


	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);						
	//gpio clck enable
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//spi2 enable
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	  /* Configure SPIy pins: SCK, MOSI ---------------------------------*/
   GPIO_InitStructure.GPIO_Pin = SPI2_PIN_SCK | SPI2_PIN_MOSI;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(SPI2_GPIO, &GPIO_InitStructure);
   //MISO	IRQ
   GPIO_InitStructure.GPIO_Pin = SPI2_PIN_MISO | PIN_nIRQ0;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(SPI2_GPIO, &GPIO_InitStructure);
   //nss
   GPIO_InitStructure.GPIO_Pin = SPI2_PIN_NSS;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(SPI2_GPIO, &GPIO_InitStructure);

}

/*
//para: SPI_DataSize_8b  SPI_DataSize_16b
void SpiInit(uint16_t SPI_DataSize)
{
	SPI_InitTypeDef	  SPI_InitStructure;
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize;//²ÎÊý
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);
}

*/

static void delay(void)
{
    int i =3 ;
    //while(times--)
    {
       
       // i=3;
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        
        __NOP();
        __NOP();
        while(i--)
        {
           __NOP();
           __NOP();
           __NOP();
           __NOP();
           __NOP();
           
        }
    }

}

#if 1

/**********************************************************
**Name:     SPICmd8bit
**Function: SPI Write one byte
**Input:    WrPara
**Output:   none
**note:     use for burst mode
**********************************************************/
void SPICmd8bit(uint8_t WrPara)	//SPI_I2S_SendData(SPI2, value);
{
  uint8_t bitcnt;  
  nCS=0;
  SCK=0;
  delay();
  for(bitcnt=8; bitcnt!=0; bitcnt--)
  {
    SCK=0;
    if(WrPara&0x80)
      MOSI=1;
    else
      MOSI=0;
	delay(); 	
    SCK=1;
    WrPara <<= 1;
	delay();
  }

  SCK=0;
  MOSI=1;
  delay();
}

/**********************************************************
**Name:     SPIRead8bit
**Function: SPI Read one byte
**Input:    None
**Output:   result byte
**Note:     use for burst mode
**********************************************************/
uint8_t SPIRead8bit(void)	  // status = SPI_I2S_ReceiveData(SPI2);
{
 uint8_t RdPara = 0;
 uint8_t bitcnt;
 
  nCS=0;
  MOSI=1; 
  delay();                                                //Read one byte data from FIFO, MOSI hold to High
  for(bitcnt=8; bitcnt!=0; bitcnt--)
  {
    SCK=0;
	delay();
    RdPara <<= 1;
    SCK=1;
	delay();
    if(MISO)
      RdPara |= 0x01;
    else
      RdPara |= 0x00;
  }
  SCK=0;
  delay();
  return(RdPara);
}

#endif


/**********************************************************
**Name:     SPIRead
**Function: SPI Read CMD
**Input:    adr -> address for read
**Output:   None
**********************************************************/
u8 SPIRead(u8 adr)
{
  u8 tmp; 
  SPICmd8bit(adr);                                         //Send address first
  tmp = SPIRead8bit();  
  nCS=1;
  delay();
  return(tmp);
}


/**********************************************************
**Name:     SPIWrite
**Function: SPI Write CMD
**Input:    WrPara -> address & data
**Output:   None
**********************************************************/
void SPIWrite(u16 WrPara)                
{                                                       
  u8 bitcnt;    
  
  SCK=0;
  nCS=0;
  delay();
  WrPara |= 0x8000;                                        //MSB must be "1" for write 
  
  for(bitcnt=16; bitcnt!=0; bitcnt--)
  {
    SCK=0;
    if(WrPara&0x8000)
      MOSI=1;
    else
      MOSI=0;
	delay();
    SCK=1;
    WrPara <<= 1;
	delay();
  }
  SCK=0;
  MOSI=1;
  nCS=1;
  delay();
}


/**********************************************************
**Name:     SPIBurstRead
**Function: SPI burst read mode
**Input:    adr-----address for read
**          ptr-----data buffer point for read
**          length--how many bytes for read
**Output:   None
**********************************************************/
void SPIBurstRead(u8 adr, u8 *ptr, u8 length)
{
  u8 i;
  if(length<=1)                                            //length must more than one
    return;
  else
  {
    SCK=0; 
    nCS=0;
	delay();
    SPICmd8bit(adr); 
    for(i=0;i<length;i++)
   		ptr[i] = SPIRead8bit();
    nCS=1;  
	delay();
  }
}


/**********************************************************
**Name:     SPIBurstWrite
**Function: SPI burst write mode
**Input:    adr-----address for write
**          ptr-----data buffer point for write
**          length--how many bytes for write
**Output:   none
**********************************************************/
void BurstWrite(u8 adr, u8 *ptr, u8 length)
{ 
  u8 i;

  if(length<=1)                                            //length must more than one
    return;
  else  
  {   
    SCK=0;
    nCS=0;   
	delay();     
    SPICmd8bit(adr|0x80);
    for(i=0;i<length;i++)
    SPICmd8bit(ptr[i]);
    nCS=1;  
	delay();
  }
}




