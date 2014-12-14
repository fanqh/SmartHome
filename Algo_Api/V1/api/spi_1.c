#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "gpio_config.h"

#include "spi.h"

#define  SPI2_PIN_SCK      GPIO_Pin_13
#define  SPI2_PIN_MOSI     GPIO_Pin_15
#define  SPI2_PIN_MISO     GPIO_Pin_14
#define  SPI2_PIN_NSS      GPIO_Pin_12
#define  PIN_PIN_CSN       GPIO_Pin_1

#define   PIN_nIRQ0         GPIO_Pin_2

#define   SPI2_GPIO		   GPIOB

#define   CSN      PBout(1)
#define   nCS      PBout(12)
#define   SCK      PBout(13)   //  X    0     0    RFMxx sck
#define   MOSI     PBout(15)   //  X    0     1    RFMxx MOSI
#define   MISO     PBin(14)    //  X    1     0    RFMxx MISO

//#define  IRQ			   PBin(2)


void SpiMsterGpioInit(uint8_t spi)
{
	GPIO_InitTypeDef GPIO_InitStructure;
					
	//gpio clck enable
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
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
   if(spi==0)
   {
	   GPIO_InitStructure.GPIO_Pin = SPI2_PIN_NSS;
	   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	   GPIO_Init(SPI2_GPIO, &GPIO_InitStructure);
   }
   else
   {

	      //ncs
	   GPIO_InitStructure.GPIO_Pin = PIN_PIN_CSN;
	   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	   GPIO_Init(SPI2_GPIO, &GPIO_InitStructure);
   }

}


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
**Function: SPI Write one u8
**Input:    WrPara	spi :SPI_1 SPI_2
**Output:   none
**note:     use for burst mode
**********************************************************/
uint8_t SPICmd8bit(uint8_t spi, uint8_t WrPara)	//SPI_I2S_SendData(SPI2, value);
{
  uint8_t bitcnt; 
  uint8_t temp  = 0;

  if(spi==0) 
  	nCS = 0;
  else
  	CSN = 0 ;
   	
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
	temp <<= 1;	
    SCK=1;
    WrPara <<= 1;
	delay();

	if(MISO)
      temp |= 0x01;
    else
      temp |= 0x00;

  }

  SCK=0;
  MOSI=1;
  delay();

  return temp;
}

/**********************************************************
**Name:     SPIRead8bit
**Function: SPI Read one u8
**Input:    None
**Output:   result u8
**Note:     use for burst mode
**********************************************************/
uint8_t SPIRead8bit(uint8_t spi)	  // status = SPI_I2S_ReceiveData(SPI2);
{
 uint8_t RdPara = 0;
 uint8_t bitcnt;

  if(spi==0) 
  	nCS = 0;
  else
  	CSN = 0;
  MOSI=1; 
  delay();                                                //Read one u8 data from FIFO, MOSI hold to High
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
u8 SPIRead(u8 spi, u8 adr)
{
  u8 tmp; 
  SPICmd8bit(spi,adr);                                         //Send address first
  tmp = SPIRead8bit(spi); 
  
  if(spi==0) 
  	nCS = 1;
  else
  	CSN = 1; 
  delay();

  return(tmp);
}


/**********************************************************
**Name:     SPIWrite
**Function: SPI Write CMD
**Input:    WrPara -> address & data
**Output:   None
**********************************************************/
void SPIWrite(u8 spi, u16 WrPara)                
{                                                       
  u8 bitcnt;    
  
  SCK=0;
  if(spi==0) 
  	nCS = 0;
  else
  	CSN = 0;
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
	
  if(spi==0) 
  	nCS=1;
  else
  	CSN =1;

  delay();
}




/**********************************************************
**Name:     SPIBurstRead
**Function: SPI burst read mode
**Input:    adr-----address for read
**          ptr-----data buffer point for read
**          length--how many u8s for read
**Output:   None
**********************************************************/
void SPIBurstRead(u8 spi, u8 adr, u8 *ptr, u8 length)
{
  u8 i;
  if(length<=1)                                            //length must more than one
    return;
  else
  {
    SCK=0;
    if(spi==0) 
  		nCS = 0;
    else
  		CSN = 0; 
	delay();
    SPICmd8bit(spi,adr); 
    for(i=0;i<length;i++)
   		ptr[i] = SPIRead8bit(spi);

	if(spi==0) 
  		nCS = 1;
    else
  		CSN = 1;  
	delay();
  }
}


/**********************************************************
**Name:     SPIBurstWrite
**Function: SPI burst write mode
**Input:    adr-----address for write
**          ptr-----data buffer point for write
**          length--how many u8s for write
**Output:   none
**********************************************************/
void BurstWrite(u8 spi, u8 adr, u8 *ptr, u8 length)
{ 
  u8 i;

  if(length<=1)                                            //length must more than one
    return;
  else  
  {   
    SCK=0;

	if(spi==0) 
  		nCS = 0;
    else
  		CSN = 0;   
	delay();     
    SPICmd8bit(spi, adr|0x80);
    for(i=0;i<length;i++)
   		SPICmd8bit(SPI_2, ptr[i]);

    if(spi==0) 
  		nCS = 1;
    else
  		CSN = 1;  
	delay();
  }
}

/*******************************************************/
// that funtion for nrf24l01
/*******************************************************/

u8 SPI_RW_Reg(u8 spi, u8 reg, u8 value)
{
	u8 status;

	status = SPICmd8bit(spi,reg);
	SPICmd8bit(spi,value);

	if(spi==0) 
	  	nCS = 1;
	else
	  	CSN = 1; 
	delay();
	return status;		
}

u8 SPI_Read(u8 spi,u8 reg)
{
	u8 reg_val;

//  	CSN = 0;                // CSN low, initialize SPI communication...
  	SPICmd8bit(spi, reg);            // Select register to read from..
  	reg_val = SPICmd8bit(spi,0);    // ..then read registervalue

  	if(spi==0) 
	  	nCS = 1;
	else
	  	CSN = 1;                // CSN high, terminate SPI communication

  	return(reg_val);        // return register value
}

uchar SPI_Read_Buf(u8 spi, u8 reg, u8 *pBuf, u8 u8s)
{
	uchar status,u8_ctr;

 // 	CSN = 0;                    		// Set CSN low, init SPI tranaction
  	status = SPICmd8bit(spi, reg);       		// Select register to write to and read status u8

  	for(u8_ctr=0;u8_ctr<u8s;u8_ctr++)
    	pBuf[u8_ctr] = SPICmd8bit(spi, 0);    // Perform SPI_RW to read u8 from nRF24L01

  	if(spi==0) 
	  	nCS = 1;
	else
	  	CSN = 1;                          // Set CSN high again

//	SendUart(status);
  	return(status);                    // return nRF24L01 status u8
}

uchar SPI_Write_Buf(u8 spi, u8 reg, u8 *pBuf, u8 u8s)
{
	uchar status,u8_ctr;

//  	CSN = 0;                   // Set CSN low, init SPI tranaction
  	status = SPICmd8bit(spi, reg);    // Select register to write to and read status u8
  	for(u8_ctr=0; u8_ctr<u8s; u8_ctr++) // then write all u8 in buffer(*pBuf)
    	SPICmd8bit(spi, *pBuf++);
  	if(spi==0) 
	  	nCS = 1;
	else
	  	CSN = 1;
//	SendUart(status);
	                 // Set CSN high again
  	return(status);          // return nRF24L01 status u8
}




