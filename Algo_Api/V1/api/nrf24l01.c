#include "include.h"
#include "spi.h"
#include "nrf24l01.h"
#include "gpio_config.h"


#define  CE_RCC	    		RCC_APB2Periph_GPIOB
#define  CE_PORT			GPIOB
#define  CE_PIN	    		GPIO_Pin_0
#define  CE				    PBout(0)


#define  IRQ_RCC		    RCC_APB2Periph_GPIOB
#define  IRQ_PORT			GPIOB
#define  IRQ_PIN	    	GPIO_Pin_2


#define  IRQ_OUT    		PBout(2)
#define  IRQ_IN	    		PBin(2)

#define  CSN      			PBout(1)
#define   SCK      PBout(13)   //  X    0     0    RFMxx sck



uchar  const TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x43,0x10,0x10,0x01}; // Define a static TX address

uchar rx_buf[TX_PLOAD_WIDTH] = {0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
                                0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};

uchar tx_buf[TX_PLOAD_WIDTH];

uchar flag;


void init_nrf24l01_io(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(CE_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(CE_PORT, &GPIO_InitStructure);

	CE=0;			// chip enable
	CSN=1;			// Spi disable	
	SCK=0;			// Spi clock line init high
}


void IRQ_CongfigIN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(IRQ_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = IRQ_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(IRQ_PORT, &GPIO_InitStructure);
}


void IRQ_CongfigOUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(IRQ_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = IRQ_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(IRQ_PORT, &GPIO_InitStructure);
}

void power_off()
{
	CE=0;
	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + CONFIG, 0x0D); 
	CE=1;
}

/**************************************************
Function: RX_Mode();

Description:
  This function initializes one nRF24L01 device to
  RX Mode, set RX address, writes RX payload width,
  select RF channel, datarate & LNA HCURR.
  After init, CE is toggled high, which means that
  this device is now ready to receive a datapacket.
**************************************************/

void ifnnrf_rx_mode(void)
{
    power_off();
	CE=0;
  	SPI_Write_Buf(SPI_1, NRF24L01_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // Use the same address on the RX device as the TX device

  	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
  	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + RF_CH, 40);        // Select RF channel 40
  	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH); // Select same RX payload width as TX Payload width
  	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + CONFIG, 0x0f);     // Set PWR_UP bit, enable CRC(2 uint8s) & Prim:RX. RX_DR enabled..

  	CE = 1; // Set CE pin high to enable RX device

  //  This device is now ready to receive one packet of 16 uint8s payload from a TX device sending to address
  //  '3443101001', with auto acknowledgment, retransmit count of 10, RF channel 40 and datarate = 2Mbps.

}
/**************************************************/

/**************************************************
Function: TX_Mode();

Description:
  This function initializes one nRF24L01 device to
  TX mode, set TX address, set RX address for auto.ack,
  fill TX payload, select RF channel, datarate & TX pwr.
  PWR_UP is set, CRC(2 uint8s) is enabled, & PRIM:TX.

  ToDo: One high pulse(>10us) on CE will now send this
  packet and expext an acknowledgment from the RX device.
**************************************************/
void ifnnrf_tx_mode(uint8 *txbuf)
{
    power_off();
	CE=0;
	
  	SPI_Write_Buf(SPI_1, NRF24L01_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
  	SPI_Write_Buf(SPI_1, NRF24L01_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack
  	SPI_Write_Buf(SPI_1, WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); // Writes data to TX payload

  	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
  	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...
  	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + RF_CH, 40);        // Select RF channel 40
  	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG + CONFIG, 0x0e);     // Set PWR_UP bit, enable CRC(2 uint8s) & Prim:TX. MAX_RT & TX_DS enabled..
	CE=1;

}

void SPI_CLR_Reg(uint8 R_T)
{
  	CSN = 0; 
	if(R_T==1)                  // CSN low, init SPI transaction
		SPICmd8bit(SPI_1, FLUSH_TX); // ..and write value to it..            
	else
		SPICmd8bit(SPI_1, FLUSH_RX);             // ..and write value to it..
  	CSN = 1;                   // CSN high again
}

void ifnnrf_CLERN_ALL(void)
{
  SPI_CLR_Reg(0);
  SPI_CLR_Reg(1);
  SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG+STATUS, 0xff);

  IRQ_CongfigOUT();
  IRQ_OUT = 1;
}



//extern unsigned char US[800];
/*****************************************************
					2.4G-功能
* 文       件: 24G_FUN.c
* 作       者: Allen
* 说       明: 2.4G-发送，接收，配置
* 编 写 时 间：2014-06-06
* 版	   本：V1.0
* 修 改 时 间：无
* ----------------------------------------------------
* 注：无

******************************************************/


void Ifnnrf_SendData_Init(void)
{
	int i = 0;
	//while(buf[i]!='\0')
	//tx_buf[0] = 0x7E;tx_buf[1] = 0x7E;
	for(i = 0;i < 7; i++)
		tx_buf[i] = i;
	tx_buf[7]=0x01;
	tx_buf[8]=0x01;
	for(i = 9;i < 32; i++)
	  tx_buf[i] = i;
}

uint8 Ifnnrf_Send(uint8 *txbuf)
{		
	uint8 sta;

	IRQ_CongfigOUT();
	IRQ_OUT = 1 ;
	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG+STATUS,0xff);
	ifnnrf_tx_mode(txbuf);

	IRQ_CongfigIN();
	while(IRQ_IN);
	sta = SPIRead(SPI_1, STATUS);
	
	Boot_UsartSend(&sta, 1);

	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG+STATUS, 0xff);
	return sta;
}

uint8 Ifnnrf_Receive()
{
	uint8 sta;
	ifnnrf_rx_mode();

	IRQ_CongfigOUT();
	IRQ_OUT = 1;

	IRQ_CongfigIN();
	while(IRQ_IN==0);
	sta = SPIRead(SPI_1, STATUS);
	Boot_UsartSend(&sta, 1);
	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG+STATUS,0xff);
 	if(sta&STA_MARK_RX)
	{
		SPIBurstRead(SPI_1, RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer;
//		for(i=0;i<TX_PLOAD_WIDTH;i++)
//			SendUart(rx_buf[i]);
		Boot_UsartSend(rx_buf, TX_PLOAD_WIDTH);

	}
	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG+STATUS,0xff);
	return sta;
}

uint8 nRF24L01_RxPacket(uint8 *rx_buf)
{
    uint8 revale=0;
	uint8 stata;
	//ifnnrf_rx_mode();
	//ifnnrf_rx_mode();
	//delay_ms(50);
	stata = SPIRead(SPI_1, STATUS);	// read register STATUS's value
	SPI_RW_Reg(SPI_1, NRF24L01_WRITE_REG+STATUS,0xff);
	if(stata&0x40)				// if receive data ready (RX_DR) interrupt
	{   
			SPIBurstRead(SPI_1, RD_RX_PLOAD, rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer		
			SPI_RW_Reg(SPI_1, FLUSH_RX,0xff);//??RX FIFO??? 
			stata |= 0x40;
			revale =1;//we have receive data    
	}
	return revale;
	
}

uint8 NRF24L01_Check(void)
{
	uint8 buf[5]={0xa5,0xa5,0xa5,0xa5,0xa5};
	uint8 buf1[5]={0xff,0xff,0xff,0xff,0xff};
	uint8 i;  	 
	SPI_Write_Buf(SPI_1, NRF24L01_WRITE_REG+TX_ADDR,buf,5);	//??5??????.	
	SPIBurstRead(SPI_1, TX_ADDR,buf1,5);              		//???????  	
	for(i=0;i<5;i++)
		if(buf1[i]!=0xa5)
			break;	
			
	//printf("%X,%X,%X,%X,%X\r\n",buf1[0],buf1[1],buf1[2],buf1[3],buf1[4]);				   
	if(i!=5)
		return 1;                               	//NRF24L01???	
	return 0;		                                //NRF24L01??
}

extern unsigned char ted;
void do_2_4G(void)
{
	//Ifnnrf_SendData_Init(); //发送数据初始化
	Ifnnrf_Send(tx_buf);
	ifnnrf_rx_mode();
	BSP_mDelay(50);
//			while(nRF24L01_RxPacket(rx_buf)!=1)
//			{
//				for(ted=0;ted<32;ted++)
//				{
//					SendUart(rx_buf[ted]);
//					break;
//				}
//				
//			}
}


