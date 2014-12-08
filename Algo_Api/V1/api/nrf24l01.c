				  /**************************************************
Function: RX_Mode();

Description:
  This function initializes one nRF24L01 device to
  RX Mode, set RX address, writes RX payload width,
  select RF channel, datarate & LNA HCURR.
  After init, CE is toggled high, which means that
  this device is now ready to receive a datapacket.
/**************************************************/
void power_off()
{
  				CE=0;
				SPI_RW_Reg(WRITE_REG + CONFIG, 0x0D); 
				CE=1;
				_delay_us(20);
}
void ifnnrf_rx_mode(void)
{
    power_off();
	CE=0;
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // Use the same address on the RX device as the TX device

  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);        // Select RF channel 40
  	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH); // Select same RX payload width as TX Payload width
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled..

  	CE = 1; // Set CE pin high to enable RX device

  //  This device is now ready to receive one packet of 16 bytes payload from a TX device sending to address
  //  '3443101001', with auto acknowledgment, retransmit count of 10, RF channel 40 and datarate = 2Mbps.

}
/**************************************************/

/**************************************************
Function: TX_Mode();

Description:
  This function initializes one nRF24L01 device to
  TX mode, set TX address, set RX address for auto.ack,
  fill TX payload, select RF channel, datarate & TX pwr.
  PWR_UP is set, CRC(2 bytes) is enabled, & PRIM:TX.

  ToDo: One high pulse(>10us) on CE will now send this
  packet and expext an acknowledgment from the RX device.
/**************************************************/
void ifnnrf_tx_mode(BYTE *txbuf)
{
    power_off();
	CE=0;
	
  	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack
  	SPI_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); // Writes data to TX payload

  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);        // Select RF channel 40
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled..
	CE=1;

}

void SPI_CLR_Reg(BYTE R_T)
{
  	CSN = 0; 
	if(R_T==1)                  // CSN low, init SPI transaction
  	SPI_RW(FLUSH_TX);             // ..and write value to it..
	else
	SPI_RW(FLUSH_RX);             // ..and write value to it..
  	CSN = 1;                   // CSN high again
}

void ifnnrf_CLERN_ALL()
{
  SPI_CLR_Reg(0);
  SPI_CLR_Reg(1);
  SPI_RW_Reg(WRITE_REG+STATUS,0xff);
  IRQ=1;
}



extern xdata unsigned char US[800];
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

BYTE Ifnnrf_Send(BYTE *txbuf)
{		
	BYTE sta;
	IRQ=1;
	SPI_RW_Reg(WRITE_REG+STATUS,0xff);
	ifnnrf_tx_mode(txbuf);
	while(IRQ);
	sta=SPI_Read(STATUS);
	
	SendUart(sta);

	SPI_RW_Reg(WRITE_REG+STATUS,0xff);
	return sta;
}

BYTE Ifnnrf_Receive()
{
	BYTE sta,i;
	ifnnrf_rx_mode();

	IRQ=1;
	while(IRQ==0);
	sta=SPI_Read(STATUS);
	SendUart(sta);
	SPI_RW_Reg(WRITE_REG+STATUS,0xff);
 	if(sta&STA_MARK_RX)
	{
		SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer;
		for(i=0;i<TX_PLOAD_WIDTH;i++)
			SendUart(rx_buf[i]);
	}
	SPI_RW_Reg(WRITE_REG+STATUS,0xff);
	return sta;
}

BYTE nRF24L01_RxPacket(BYTE *rx_buf)
{
  BYTE revale=0;
	BYTE stata;
	//ifnnrf_rx_mode();
	//ifnnrf_rx_mode();
	//delay_ms(50);
	stata=SPI_Read(STATUS);	// read register STATUS's value
	SPI_RW_Reg(WRITE_REG+STATUS,0xff);
	if(stata&0x40)				// if receive data ready (RX_DR) interrupt
	{   
			SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer		
			SPI_RW_Reg(FLUSH_RX,0xff);//??RX FIFO??? 
			stata |= 0x40;
			revale =1;//we have receive data    
	}

	return revale;
	
}

BYTE NRF24L01_Check(void)
{
	BYTE buf[5]={0xa5,0xa5,0xa5,0xa5,0xa5};
	BYTE buf1[5]={0xff,0xff,0xff,0xff,0xff};
	BYTE i;  	 
	SPI_Write_Buf(WRITE_REG+TX_ADDR,buf,5);//??5??????.	
	SPI_Read_Buf(TX_ADDR,buf1,5);              //???????  	
	for(i=0;i<5;i++)
		if(buf1[i]!=0xa5)
			break;					   
	if(i!=5)
		return 1;                               //NRF24L01???	
	return 0;		                                //NRF24L01??
}

extern unsigned char ted;
void do_2_4G(void)
{
	//Ifnnrf_SendData_Init(); //发送数据初始化
	Ifnnrf_Send(tx_buf);
	ifnnrf_rx_mode();
	delay_ms(50);
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


