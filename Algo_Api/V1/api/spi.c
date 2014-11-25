#include "stm32f10x.h"
#include "stm32f10x_spi.h"

#include "spi.h"

#define  SPI2_PIN_SCK      GPIO_Pin_13
#define  SPI2_PIN_MOSI     GPIO_Pin_15
#define  SPI2_PIN_MISO     GPIO_Pin_14
#define  SPI2_PIN_NSS      GPIO_Pin_1
#define  PIN_nIRQ0           GPIO_Pin_2

#define  SPI2_GPIO		   GPIOB

//#define  IRQ			   PBin(2)


void SpiMsterGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//gpio clck enable
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//spi2 enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	  /* Configure SPIy pins: SCK, MOSI ---------------------------------*/
   GPIO_InitStructure.GPIO_Pin = SPI2_PIN_SCK | SPI2_PIN_MOSI;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
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


//para: SPI_DataSize_8b  SPI_DataSize_16b
void SpiInit(uint16_t SPI_DataSize)
{
	SPI_InitTypeDef	  SPI_InitStructure;
	  /* SPIy Config -------------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize;//参数
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);
}






#if 0

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
  
  for(bitcnt=8; bitcnt!=0; bitcnt--)
  {
    SCK=0;
    if(WrPara&0x80)
      MOSI=1;
    else
      MOSI=0;
    SCK=1;
    WrPara <<= 1;
  }
  SCK=0;
  MOSI=1;
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
  MOSI=1;                                                 //Read one byte data from FIFO, MOSI hold to High
  for(bitcnt=8; bitcnt!=0; bitcnt--)
  {
    SCK=0;
    RdPara <<= 1;
    SCK=1;
    if(MISO)
      RdPara |= 0x01;
    else
      RdPara |= 0x00;
  }
  SCK=0;
  return(RdPara);
}

#endif

/**********************************************************
**Name:     SPIRead
**Function: SPI Read CMD
**Input:    adr -> address for read
**Output:   None
**********************************************************/
uint8_t SPIRead(uint8_t adr)
{
  uint8_t tmp;
  uint8_t retry = 0;
   
//  SpiInit(SPI_DataSize_8b);

  while(SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_TXE ) == RESET)
  {
  	retry ++;
	if(retry > 200)
		return 0;
  }
  SPI_I2S_SendData(SPI2, adr);//SPICmd8bit(adr);                                         //Send address first
  
  retry = 0;
  while(SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_RXNE ) == RESET)
  {
  	retry ++;
	if(retry >200)
		return 0;
  }
  tmp = SPI_I2S_ReceiveData(SPI2);

  return(tmp);
}

/**********************************************************
**Name:     SPIWrite
**Function: SPI Write CMD
**Input:    WrPara -> address & data
**Output:   None
**********************************************************/
void SPIWrite(uint16_t WrPara)                
{                                                       
  uint8_t retry = 0;  
  
  SpiInit(SPI_DataSize_16b);  
  
  WrPara |= 0x8000;                                        //MSB must be "1" for write 


  while(SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_TXE ) == RESET)
  {
  	retry ++;
	if(retry > 200)
		return ;
  }
  SPI_I2S_SendData(SPI2, WrPara);
  
  SpiInit(SPI_DataSize_8b);   //改回8位位宽
}

/**********************************************************
**Name:     SPIBurstRead
**Function: SPI burst read mode
**Input:    adr-----address for read
**          ptr-----data buffer point for read
**          length--how many bytes for read
**Output:   None
**********************************************************/
void SPIBurstRead(uint8_t adr, uint8_t *ptr, uint8_t length)
{
  uint8_t i;
  uint8_t retry = 0;

  if(length<=1)                                            //length must more than one
    return;
  else
  {

	while(SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_TXE ) == RESET)
	{
	  	retry ++;
		if(retry > 200)
			return ;
	}
  	SPI_I2S_SendData(SPI2, adr);

    for(i=0;i<length;i++)
	{
    	ptr[i] = SPI_I2S_ReceiveData(SPI2);
	}
 
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
void BurstWrite(uint8_t adr, uint8_t *ptr, uint8_t length)
{ 
  uint8_t i;
  uint8_t retry = 0;

  if(length<=1)                                            //length must more than one
    return;
  else  
  {  
  	while(SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_TXE ) == RESET)
	{
	  	retry ++;
		if(retry > 200)
			return ;
	} 
	     
    SPI_I2S_SendData(SPI2, adr);

    for(i=0;i<length;i++)
	{
   		SPI_I2S_SendData(SPI2, adr);
    }
  }
}






