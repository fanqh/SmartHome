#if 0
	while(1)
	{  
		RF315_DATA_t  RF315_Receive;
//		BSP_mDelay(1000);
//		printf("systme is work\r\n");		
		if(RF_decode(&RF315_Receive))
		{
			RF315_Send(&RF315_Receive);
			RF315_Send(&RF315_Receive);
			RF315_Send(&RF315_Receive);	
			BSP_mDelay(1000);
		}
	}
#endif

#if 0
		//2.hG
	SpiMsterGpioInit(SPI_1);
	init_nrf24l01_io();
	ifnnrf_rx_mode();

	while(NRF24L01_Check());
	printf("nrf24l01 is ok\r\n");

	while(1)
	{

	#if 1
		if(nRF24L01_RxPacket(rx_buf))
		{	
				
				debug_led_on();
				U1_sendS(rx_buf,10);
				memset(rx_buf, 0xff, 32);
	
		}
		BSP_mDelay(100);
	#else
		Ifnnrf_Send("i am sorry");
		BSP_mDelay(500);   
	#endif
	}
#endif

#if 0
	SpiMsterGpioInit(SPI_2);
 	RFM69H_Config();
	RFM69H_EntryRx();
	while(1)
	{	  
	   uint8_t uu;
	   	int len1;

//		SPIWrite(SPI_2, 0x0632);
//		uu = SPIRead(SPI_2, 0x06);
//		printf("%X\r\n", uu);
		len1 = RFM69H_RxPacket(&RxBuf);  ///需要10us定时器
		Disable_SysTick();
		if(len1 > 0)
		{	
			printf("receive data len = %d\r\n", len1);
#if 0
			RFM69H_EntryTx();
			while(1)
			{
				RFM69H_TxPacket(&RxBuf);
				BSP_mDelay (5000);

				printf("send data len = %d\r\n", len1);
			}
#endif
		}		 	
	}

#endif



#if 0
    while(1)
    {
        
        temp=GetTemperature();
        Boot_UsartSend((uint8 *)&temp,2);
        delay_ms(500);
        
        m3_315_clr();
        CLR_INFRARED;
        delay_ms(10);
        m3_315_set();
        SET_INFRARED;
        delay_ms(10);
          

    }
#endif




backup data:

						else if(rec_buf[1]=='W')
						{						
							c = 0;
							Enable_SysTick();		//启动定时器0
							while(c < 6)//重复次数!
							{
                                __disable_irq();
								T = 0;
                                 __enable_irq();
								 m3_315_set();
								i = 4;//第3与4位是数据长度,从第5位是红外、无线控制数据
								while(T < 28);//(13 * 808 = 10504同步脉宽!	
								 __disable_irq();
								T = 0;
                                 __enable_irq();
								 m3_315_clr();
								M.u[1] = rec_buf[3];
								M.u[0] = rec_buf[4];
								j = M.ue;//主机生成的长度要减1
								while(T < 808);//(13 * 808 = 10504同步脉宽!
							         
								while(i < j)
								{
                                     __disable_irq();
									T = 0;
                                    __enable_irq();
									 m3_315_set();
									i++;
									while(T < rec_buf[i]);
                                     __disable_irq();
									T = 0;
                                     __enable_irq();
									 m3_315_clr();
									i++;//i在此,精准一些
									while(T < rec_buf[i]);
								}
								c++;
							}