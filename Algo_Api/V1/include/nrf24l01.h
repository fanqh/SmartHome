 #ifndef __24G_FUN_H_
#define __24G_FUN_H_


void Ifnnrf_SendData_Init(void);
BYTE Ifnnrf_Send(BYTE *txbuf);
BYTE nRF24L01_RxPacket(BYTE *rx_buf);
BYTE NRF24L01_Check(void);
void do_2_4G(void);


void init_nrf24l01_io(void);
void ifnnrf_rx_mode(void);
void ifnnrf_tx_mode(BYTE *txbuf);
void ifnnrf_CLERN_ALL();

#endif