#ifndef __SPI_H_
#define __SPI_H_
#include "include.h"

void SpiMsterGpioInit(void);
void SpiInit(uint16_t SPI_DataSize);

uint8_t SPIRead(uint8_t adr);
void SPIWrite(uint16_t WrPara);
void SPIBurstRead(uint8_t adr, uint8_t *ptr, uint8_t length);
void BurstWrite(uint8_t adr, uint8_t *ptr, uint8_t length);


#endif

