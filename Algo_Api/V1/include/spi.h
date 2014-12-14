#ifndef __SPI_H_
#define __SPI_H_
#include "include.h"


#define SPI_1   1
#define SPI_2   0

void SpiMsterGpioInit(uint8_t spi);
void SpiInit(uint8_t spi, uint16_t SPI_DataSize);

uint8_t SPICmd8bit(uint8_t spi, uint8_t WrPara);

uint8_t SPIRead(uint8_t spi, uint8_t adr);
void SPIWrite(uint8_t spi, uint16_t WrPara);
//void SPI_RW_Reg(u8 spi, u8 reg, u8 value);
//u8 SPI_RW_Reg(u8 spi, u8 reg, u8 value)
void SPIBurstRead(uint8_t spi, uint8_t adr, uint8_t *ptr, uint8_t length);
//void BurstWrite(u8 spi, u8 adr, u8 *ptr, u8 length);
void BurstWrite(uint8 spi, uint8 adr, uint8 const *ptr, uint8 length);


u8 SPI_RW_Reg(u8 spi, u8 reg, u8 value);
u8 SPI_Read(u8 spi,u8 reg);
uchar SPI_Read_Buf(u8 spi, u8 reg, u8 *pBuf, u8 bytes);
uchar SPI_Write_Buf(u8 spi, u8 reg, u8 const *pBuf, u8 bytes);

																 
#endif

