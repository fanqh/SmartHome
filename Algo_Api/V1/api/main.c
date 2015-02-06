#include"Include.h"
#include "DK_RFM.h"
#include "spi.h"
#include "timer.h"
#include "nrf24l01.h"		  
#include "string.h"
#include "command.h"

/**********************************************************************************/
/**********************************************************************************/
/*
1.	rfm69h 解析和发送需要一个误差小于10us的定时器，，现在用的和systemtick有冲突，需要修改
2. TIm2 定时器在315M 和24G中有开启和关闭，注意其他函数调用
*/
/**********************************************************************************/
/**********************************************************************************/
uint32  Wifi_Command_Mode = 0; //=1 wifi工作在命令模式 =0 工作在数据传输模式
uint32  Check_wifi = 1;		//检测wifi工作模式
uint32  Get_Wifi_MAC = 0; //检测wifi模块MAC地址标志，只在STA模式下检测
uint32  Wifi_MAC_Count = 0;
uint32  Wifi_AP_OPEN_MODE = 0; //wifi工作在AP的OPEN模式下，灯闪烁
uint32  RST_count1 = 0; //计数
uint32  RST_count2 = 0;



volatile uint32 ui = 0;//串口接收数据长度!
uint8   rec_buf[256];
#define wifi_mac_num 16
volatile uint8 Wifi_MAC[wifi_mac_num] = {0x00};

volatile uint8 RI=0;

//红外
uint8 FlagInfrared = 0;			// 0: idle 1: 学习 2：接手

//RF_RFM69H
RFM69H_DATA_Type TxBuf; 
uint8 FlagRF24GLearn = 0; // 0: idle 1: 学习 2：接手

//315M
uint8 RF315MHz_Flag = 0;
uint8 RF433MHz_Flag = 0;

//2.4G
uchar rx_buf[TX_PLOAD_WIDTH] = {0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
                                0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};
typedef union //char型数据转int型数据类 
{  
	unsigned short  ue; 
	unsigned char 	 u[2]; 
}U16U8;
U16U8  M;//两个8位转16位

//JTAG模式设置定义
#define JTAG_SWD_DISABLE   0X02
#define SWD_ENABLE         0X01
#define JTAG_SWD_ENABLE    0X00	
//----------------------------
void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
	AFIO->MAPR|=temp;       //设置jtag模式
}


int tamain(void)
{


	JTAG_Set(SWD_ENABLE);		//加
	GPIOC->CRL&=0XFFF0FFFF;	//PC4推挽输出
 	GPIOC->CRL|=0X00030000;
	led_init();
    wifi_led(LED_ON);  
    debug_led_off();
    m3_315_io_config();

  //Command_Process();
    Init_DS18B20();
    timer2_init();                      //5ms 中断
	TIM3_NVIC_Configuration(); 		    //BSP_Delay 初始化
   
    app_enroll_tick_hdl(isr_13us, 0);   //13us在底层配置的，配置完成就关闭了
    Disable_SysTick();
	SpiMsterGpioInit(SPI_2);
	RF69H_DataCongfigIN();
	Infrared_UsartInit();
	printf("uart is working\r\n"); 


    while(1)
    {
//	   static uint8 t = 0;
		for(;;)
		{
			if(ScanKey())
				wifi_state = WIFI_IDLE;		//WIFI状态初始化
			if(Wifi_EnterEntmProcess())	
				break;	
		}
		

//		t++;
//		if(t>10 && t<20)
//		{
//			debug_led_on();
//		}
//		else if(t>=20)
//		{
//			t = 0;
//			debug_led_off();
//		}
//		delay_ms(300);
#if 0
		 if(FlagRF24GLearn == 1)
		 {
		 	if(RF24GTimeCount.TimeCount > RF24GLEARNTIMECOUNT)	//学习超时
			{
				FlagRF24GLearn = 0;	
				//timer2_disable();
				U1_sendS((uint8*)ResFail, sizeof(ResFail));	
			}
			if(Ifnnrf_Receive(rx_buf)==1)
			{
				FlagRF24GLearn = 0;	
				//timer2_disable();
				U1_sendS((uint8*)ResSucess, sizeof(ResSucess));	
			}

		 }
		 if(FlagInfrared ==1)
		 {
		 	infrared_data_t infrared_receive;
		 	if(InfraredTimeCount.TimeCount > INFRAREDLEARNTIMECOUNT)
			{
//				printf("timeout\r\n");
				FlagInfrared = 0;
				InfraredReset();
				//U1_sendS((uint8*)ResFail, sizeof(ResFail));	
			}
			else if(ParseInfrared(&infrared_receive))
			{
				FlagInfrared = 0;	
//				printf("rec is ok\r\n");
				U1_sendS((uint8*)InfraredSendCMD, sizeof(InfraredSendCMD));
				U1_sendS((uint8*)infrared_receive.buff, infrared_receive.len);
				InfraredReset();
				U1_sendS((uint8*)tail, sizeof(tail));
											
			}	
		 }
        if(get_usart_interrupt_flg())
		{
			U1_in();			//获取串口发送的SJ数据!
			if(rec_buf[2] == ':')//接收到正确的控制数据!
			{
//				//timer2_disable(); 
				switch(rec_buf[0])
				{
					 case 'L':
					 {
					 	switch(rec_buf[1])
						{
							case 'H': //红外学习
							{
								if((FlagInfrared==0) && InfraredEnterStudy())		 //需要解析，如果已经在学习模式
								{
									FlagInfrared = 1;
									InfraredTimeCount.TimeCount = 0;
									InfraredTimeCount.FlagStart = 1;
									U1_sendS((uint8*)InfraredStudyCMD, sizeof(InfraredStudyCMD));	
								}
								else 
								{
									U1_sendS((uint8*)ResFail, sizeof(ResFail));	
								}											
							}

							break;
	
							case 'W':   //315M学习
							{
						  		RF_AC_DATA_TYPE  RF315_Receive;

								RF315MHz_Flag = 1;
								RF315TimeCount.TimeCount = 0;
								RF315TimeCount.FlagStart = 1;
//								U1_sendS((uint8*)RF315StudyCMD, sizeof(RF315StudyCMD));
								while((RF315TimeCount.TimeCount <= 1000)&&(RF315MHz_Flag == 1))
								{
									//printf("lenarn\r\n");
									if(RFDecodeAC(&RF315_Receive, RF315_REC_PORT, RF315_REC_GPIO))
									{
//										printf("len = %d\r\n",RF315_Receive.len);
										RF315MHz_Flag = 0;
										U1_sendS((uint8*)RF315SendCMD, sizeof(RF315SendCMD));
										U1_sendS((uint8*)&RF315_Receive, RF315_Receive.len + 8);//	sizeof(RF_AC_DATA_TYPE)
										U1_sendS((uint8*)tail,sizeof(tail));	
									}
								}
								//timer2_disable();
								if(RF315TimeCount.TimeCount > 1000)
								{
									RF315MHz_Flag = 0;
									U1_sendS((uint8*)ResFail, sizeof(ResFail));
								}
							}
							break;
	
							case 'F':	//433M学习
							{
						  		 RF_AC_DATA_TYPE  RF433_Receive;

								RF433MHz_Flag = 1;
								RF433TimeCount.TimeCount = 0;
								RF433TimeCount.FlagStart = 1;
								timer2_enable();
//								U1_sendS((uint8*)RF433StudyCMD, sizeof(RF433StudyCMD));
								if(RFM69H_RxWaitStable())
								{
									RF69H_DataCongfigIN();
//									rfm69h_status = RFM69H_RECEIVE;
									while((RF433TimeCount.TimeCount <= 1000)&&(RF433MHz_Flag == 1))
									{
										if(RFDecodeAC(&RF433_Receive, RF69H_DATA_PORT, RF69H_DATA_PIN))
										{
//											printf("len = %d\r\n",RF433_Receive.len);
											RF433MHz_Flag = 0;
											U1_sendS((uint8*)RF433SendCMD, sizeof(RF433SendCMD));
											U1_sendS((uint8*)&RF433_Receive, RF433_Receive.len + 8);//	sizeof(RF_AC_DATA_TYPE)
 										    U1_sendS((uint8*)tail,sizeof(tail));
											break;	
										}
									}  
//									rfm69h_status = RFM69H_IDLE;
								}
//								else
//								{
//									RF433MHz_Flag = 0;
//									U1_sendS((uint8*)ResFail, sizeof(ResFail));	
//								}
//								//timer2_disable();
//								if(RF433TimeCount.TimeCount > 1000)
								{
									RF433MHz_Flag = 0;
									U1_sendS((uint8*)ResFail, sizeof(ResFail));
								}
							}
							break;
	
							case 'T':  //2.4G学习
							{
								if(FlagRF24GLearn!=2)
								{
									U1_sendS((uint8*)RF433StudyCMD, sizeof(RF433StudyCMD));
									ifnnrf_rx_mode();
									RF24GTimeCount.TimeCount = 0;
									RF24GTimeCount.FlagStart = 1;
									timer2_enable();	
									FlagRF24GLearn = 1;
								}
							}
							break;
	
						    case 'B':  //打开唤醒灯
	                            debug_led_on();        //唤醒led点亮
								U1_sendS((uint8*)ResSucess, sizeof(ResSucess));
							break;
	
							case 'D':  //关闭唤醒灯
	                            debug_led_off();        
								U1_sendS((uint8*)ResSucess, sizeof(ResSucess));
								break;
	
							default:
								break;
						}
					 }	
					 break;
	
				   case 'F':
				   {
				   		switch (rec_buf[1])
						{
					   		case'H' ://红外发射
							{
									
//								InfraredReset();
								U1_sendS((uint8*)InfraredSendCMD, sizeof(InfraredSendCMD));
								if(rec_buf[rec_buf[3]==0xfa])
								{
									Infrared_UsartSend((uint8*)&rec_buf[3], rec_buf[4]+4);
									U1_sendS((uint8*)ResSucess, sizeof(ResSucess));
								}
								else
								{
									U1_sendS((uint8*)ResFail, sizeof(ResFail));	
								}

							}
							break;
							case  'W'://315M发射
							{
								uint8 time = 0;
								RF_AC_DATA_TYPE RF315_SendData;

								U1_sendS((uint8*)RF315SendCMD, sizeof(RF315SendCMD));							
								memcpy((uint8*)&RF315_SendData, &rec_buf[3], sizeof(RF_AC_DATA_TYPE));
//								printf("TimeBase:%d, data[0]:%X, data[1]:%X, data[2]:%X\r\n", RF315_SendData.TimeBase,RF315_SendData.buff[0],RF315_SendData.buff[1],RF315_SendData.buff[2]);
								GPIO_WriteBit(RF315_SEND_PORT, RF315_SEND_GPIO, Bit_RESET);
								while(time < 6)//重复次数!
								{
									time ++;	
									RFCodeAC_Send(&RF315_SendData , RF315_SEND_PORT, RF315_SEND_GPIO);	  
									
								}
								U1_sendS((uint8*)ResSucess, sizeof(ResSucess));		
							}
							break;
							case 'F': //433M发射
							{	
								uint8 time = 0;
								RF_AC_DATA_TYPE RF433_SendData;

							    U1_sendS((uint8*)RF433SendCMD, sizeof(RF433SendCMD));
								RFM69H_Config();
								RFM69H_EntryTx();
								if(RFM69H_TxWaitStable())
								{
									RF69H_DataCongfigOUT();
									memcpy((uint8*)&RF433_SendData, &rec_buf[3], sizeof(RFM69H_DATA_Type));
								//	printf("TimeBase:%d, data[0]:%X, data[1]:%X, data[2]:%X\r\n", RF433_SendData.TimeBase,RF433_SendData.buff[0],RF433_SendData.buff[1],RF433_SendData.buff[2]);
									//printf("len = %d\r\n", RF433_SendData.len);
									while(time<6)
									{	
										time ++;
										RFCodeAC_Send(&RF433_SendData , RF69H_DATA_PORT, RF69H_DATA_PIN);	  
											
									}
									U1_sendS((uint8*)ResSucess, sizeof(ResSucess));	
								}
								else
								{
									U1_sendS((uint8*)ResFail, sizeof(ResFail));	//失败
								}
							}
							break;
							case'T': //2.4G 发射
							{
								if(Ifnnrf_Send(&rec_buf[4]))
									U1_sendS((uint8*)ResSucess, sizeof(ResSucess));	
								else
									U1_sendS((uint8*)ResFail, sizeof(ResFail));
								
							}
							break;
							default :
							break;
						}
				   }
				   break;
				   case 'T':
					   if(rec_buf[1]=='K')	 //心跳
					   {
					   		U1_sendS((uint8*)ResSucess, sizeof(ResSucess));	
					   }
				   break;
	
				   case 'D':
					   if(rec_buf[1]=='T') //温度采集
					   {
	//				   		uint16 temp;
	
	//					    U1_sendS("回复命令", 5);
							while((rec_buf[2] = GetTemperature()) == 0x55);	   // 这里？ 如果是负值？  如何判断采集失败
							rec_buf[3] = '<';
							rec_buf[4] = '<';
							U1_sendS(rec_buf, 5);	
					   }
					   else if(rec_buf[1]=='S')	//检测wifi命令
					   {
					   		Check_wifi = 1;
							Wifi_Command_Mode = 0;
							U1_sendS("DS<<",4);
					   }
					   else	if(rec_buf[1]=='M')	 //MAC 地址发送
					   {};
	//				   else
	//				   {};
				   break;
	
				   case	'V': //固件版本获取
				   		if(rec_buf[1]=='E')
						{}
				   break;
	
				   case 'S':
				   		if(rec_buf[1]=='X')//固件升级
						{
						  if(rec_buf[1]=='X')
	                      {
	                            Boot_UsartSend("enter_upg_mode",sizeof("enter_upg_mode")-1);
	                            //timer2_disable();
	                            Command_Process();
								timer2_enable();
	                      }
						}
				   break;
				   default :
				   		break;
		 		   }
				}
			}
			else 
			{
			}
		memset(rec_buf,0x00,sizeof(rec_buf));	 
#endif
	}										   

}



#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

