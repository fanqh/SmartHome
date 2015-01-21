#ifndef    __UART_H_
#define   __UART_H_




#define UART_LEN   256             

typedef struct
{
    volatile uint32 size;
    uint8 *write;
    uint8 *read;
}UART_INIT_STRUCT;

void    Infrared_UsartInit(void);
void    Infrared_UsartSend(unsigned char *outptr,unsigned int len);
int16   Infrared_UsartGet(uint8 *buff,uint32 len,uint32 timeout);
uint32  GetUartBuffSize(void);


#endif
