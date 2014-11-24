#ifndef _M3_WIFI_H
#define _M3_WIFI_H

#include"Include.h"

#define     PIPE_MODE               0       //透明传输协议 ,wifi只是作为无线串口使用,默认模式
#define     AT_MODE                 1       //AT_命令集模式  



#define M3_WIFI_NRELOAD             GPIO_Pin_15
#define M3_WIFI_GPIO                GPIOA


#define SET_WIFI_RST        GPIO_SetBits(M3_WIFI_GPIO,M3_WIFI_NRELOAD)
#define CLR_WIFI_RST        GPIO_ResetBits(M3_WIFI_GPIO,M3_WIFI_NRELOAD)
#define READ_WIFI_RST       GPIO_ReadInputDataBit(M3_WIFI_GPIO,M3_WIFI_NRELOAD)

void    m3_wifi_config(void);

void    m3_wifi_rst_input(void);

void    m3_wifi_rst_output(void);

void    U1_in(void);

int     start_wifi_command(void);

int     start_wifi_data(void);

void    U1_send(uint8 data);

void    U1_sendS(uint8 *data,uint32 len);

void    m3_wifi_rec_data(uint8 *data);

void    m3_wifi_rec_buf(uint8 *data,uint32 len,uint32 timeout);

#endif

