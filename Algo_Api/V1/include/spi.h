#ifndef __SPI_H_
#define __SPI_H_
#include "include.h"


#define SPI_1   1
#define SPI_2   0

void SpiMsterGpioInit(uint8_t spi);
void SpiInit(uint8_t spi, uint16_t SPI_DataSize);

uint8_t SPIRead(uint8_t spi, uint8_t adr);
void SPIWrite(uint8_t spi, uint16_t WrPara);
void SPI_RW_Reg(u8 spi, u8 reg, u8 value);
void SPIBurstRead(uint8_t spi, uint8_t adr, uint8_t *ptr, uint8_t length);
void BurstWrite(uint8_t spi, uint8_t adr, uint8_t *ptr, uint8_t length);


#endif

