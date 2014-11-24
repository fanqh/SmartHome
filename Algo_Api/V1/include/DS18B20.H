#ifndef __DS18B20__H
#define __DS18B20__H

#include"Include.h"






void    ds18b20_io_init(void);
void    Init_DS18B20(void);
int     GetTemperature(void);
void    delay_x_10us(int times);

void    RCC_Config(void);
void    GPIO_Configuration(void);
void    DS18B20_Delay(unsigned int t);
void    DS18B20_DirectPort(u16 GPIO_Pin,BitAction  BitVal);
void    DS18B20_WriteByteData(unsigned char Data);
char    DS18B20_ReadByteData(void);
void    DS18B20_Init(void);
void    DS18B20_Convert(void);
int     DS18B20_ReadTemperature(void);
void    DS18B20_Configuration(void);
int     DS18B20_Temperature(void);
void    DS18B20_Test(void);

#endif

