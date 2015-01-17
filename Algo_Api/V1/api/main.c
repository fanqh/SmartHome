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
1.	rfm69h �����ͷ�����Ҫһ�����С��10us�Ķ�ʱ�����������õĺ�systemtick�г�ͻ����Ҫ�޸�
2. TIm2 ��ʱ����315M ��24G���п����͹رգ�ע��������������
*/
/**********************************************************************************/
/**********************************************************************************/

//const uint8 ResSucess[] = "RE:0<<";
//const uint8 ResFail[]  =  "RE:1<<";
//const uint8 tail[]  = "<<" ;

uint32  Flag = 0;	  //�Ƿ��38KH��������
uint32  Wifi_Command_Mode = 0; //=1 wifi����������ģʽ =0 ���������ݴ���ģʽ
uint32  Check_wifi = 1;		//���wifi����ģʽ
uint32  Get_Wifi_MAC = 0; //���wifiģ��MAC��ַ��־��ֻ��STAģʽ�¼��
uint32  Wifi_MAC_Count = 0;
uint32  Wifi_AP_OPEN_MODE = 0; //wifi������AP��OPENģʽ�£�����˸
uint32  RST_count1 = 0; //����
uint32  RST_count2 = 0;
volatile uint32 T = 0;	//����


uint32  i = 0;//������ 
uint32  j = 0;//������
uint32  c = 0;//������

volatile uint32 ui = 0;//���ڽ������ݳ���!
uint8   rec_buf[256];
#define wifi_mac_num 16
volatile uint8 Wifi_MAC[wifi_mac_num] = {0x00};

volatile uint8 RI=0;


//RF_RFM69H
RFM69H_DATA_Type TxBuf; 
uint8 FlagRF24GLearn = 0; // 0: idle 1: ѧϰ 2������


//315M
uint8 _315MHz_Flag;

//2.4G
uchar rx_buf[TX_PLOAD_WIDTH] = {0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
                                0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};
typedef union //char������תint�������� 
{  
	unsigned short  ue; 
	unsigned char 	 u[2]; 
}U16U8;
U16U8  M;//����8λת16λ

//JTAGģʽ���ö���
#define JTAG_SWD_DISABLE   0X02
#define SWD_ENABLE         0X01
#define JTAG_SWD_ENABLE    0X00	
//----------------------------
void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //��������ʱ��	   
	AFIO->MAPR&=0XF8FFFFFF; //���MAPR��[26:24]
	AFIO->MAPR|=temp;       //����jtagģʽ
} 

int tamain(void)
{
	_315MHz_Flag = 0;


	JTAG_Set(SWD_ENABLE);		//��
	GPIOC->CRL&=0XFFF0FFFF;	//PC4�������
 	GPIOC->CRL|=0X00030000;


	led_init();
    wifi_led(LED_ON);  
    debug_led_off();

    m3_315_io_config();
    infrared_io_init();  
//    m3_315_clr();                       //�ر�315


  //Command_Process();
    Init_DS18B20();
    timer2_init();                      //5ms �ж�
	TIM3_NVIC_Configuration(); 		    //BSP_Delay ��ʼ��
    
    app_enroll_tick_hdl(isr_13us, 0);   //13us�ڵײ����õģ�������ɾ͹ر���
    Disable_SysTick();
	SpiMsterGpioInit(SPI_2);
	 RF69H_DataCongfigIN();

	printf("uart is working\r\n"); 


    while(1)
    {
		 if(FlagRF24GLearn == 1)
		 {
		 	if(RF24GTimeCount.TimeCount > RF24GLEARNTIMECOUNT)	//ѧϰ��ʱ
			{
				FlagRF24GLearn = 0;	
				timer2_disable();
				U1_sendS((uint8*)ResFail, sizeof(ResFail));	
			}
			if(Ifnnrf_Receive(rx_buf)==1)
			{
				FlagRF24GLearn = 0;	
				timer2_disable();
				U1_sendS((uint8*)ResSucess, sizeof(ResSucess));	
			}

		 }
		 else if(FlagRF24GLearn == 2)	   //����
		 {
		 	if(RF24GTimeCount.TimeCount > RF24GLEARNTIMECOUNT)	//ѧϰ��ʱ
			{
				FlagRF24GLearn = 0;	
				timer2_disable();
				U1_sendS((uint8*)ResFail, sizeof(ResFail));	
			}
			if(Ifnnrf_Receive(rx_buf)==1)
			{
				FlagRF24GLearn = 0;	
				timer2_disable();
				U1_sendS((uint8*)RF24GRecCMD, sizeof(RF24GRecCMD));
				U1_sendS((uint8*)rx_buf, TX_PLOAD_WIDTH);
				U1_sendS((uint8*)ResSucess, sizeof(ResSucess));	
			}
		 }
#if 0
		if(Check_wifi)
		{
			timer2_disable(); 
			if(!Wifi_Command_Mode)
			{
				start_wifi_command();
			}
			if(Wifi_Command_Mode)
			{
				delay_ms(10);
				U1_sendS("AT+WMODE\r\n",10);	
				Check_wifi = 0;	
			}
			timer2_enable(); 
		}	
		if(!Wifi_AP_OPEN_MODE)
		{
            wifi_led(READ_WIFI_RST);
			if(Get_Wifi_MAC)
			{
				timer2_disable(); 	   //���ﶨʱ���ʹ򿪺���û�гɶԳ���
				if(Wifi_MAC[0]==0x00)
				{
					if(!Wifi_Command_Mode)
					{
						start_wifi_command();
					}
					if(Wifi_Command_Mode)
					{
						delay_ms(10);
						U1_sendS("AT+NMAC\r\n",9);	
					}
				}
				else
				{
					Get_Wifi_MAC = 0;
					Wifi_Command_Mode = 0;
					U1_sendS("DM:",3);
					U1_sendS(Wifi_MAC,sizeof(Wifi_MAC));
					U1_sendS("<<",2);
					Wifi_MAC_Count = 0;
					timer2_enable(); 
				}
			}
		}
        m3_wifi_rst_input();
		if(READ_WIFI_RST==0)
		{
			timer2_disable(); 
			//debug_led_off();   
			while(READ_WIFI_RST==0)
			{
				RST_count1++;
				if(RST_count1 == 65535)
				{
					RST_count1 = 0;
					RST_count2++;
				}
			}
			if(RST_count2 >= 5)
			{
				Wifi_Command_Mode = 0;
				Get_Wifi_MAC = 0;
				Check_wifi = 1;
				RST_count1 = 0;
				RST_count2 = 0;
			}	
			timer2_enable(); 
		}	
#endif 
        if(get_usart_interrupt_flg())
		{
			U1_in();			//��ȡ���ڷ��͵�SJ����!
			if(rec_buf[2] == ':')//���յ���ȷ�Ŀ�������!
			{
				timer2_disable(); 
				switch(rec_buf[0])
				{
					 case 'L':
					 {
					 	switch(rec_buf[1])
						{
							case 'H': //����ѧϰ
							{
								
							}

							break;
	
							case 'W':   //315Mѧϰ
							{
						  		RF315_DATA_t  RF315_Receive;
								 RFM69H_DATA_Type RF433_RxBuf;

								_315MHz_Flag = 1;
								RF315TimeCount.TimeCount = 0;
								RF315TimeCount.FlagStart = 1;
								timer2_enable();
//								U1_sendS((uint8*)RF315StudyCMD, sizeof(RF315StudyCMD));
								if(RFM69H_RxPacket(&RF433_RxBuf))
								{
									while((RF315TimeCount.TimeCount <= 1000)&&(_315MHz_Flag == 1))
									{
									//	if(RFM69H_RxPacket(&RF433_RxBuf))
										{
										//printf("lenarn\r\n");
										if(RF_decode(&RF315_Receive, GPIOB, GPIO_Pin_11))
										{
											printf("len = %d\r\n",RF315_Receive.len);
											_315MHz_Flag = 0;
	//										U1_sendS((uint8*)RF315SendCMD, sizeof(RF315SendCMD));
	//										U1_sendS((uint8*)&RF315_Receive, RF315_Receive.len + 8);//	sizeof(RF315_DATA_t)
	//										U1_sendS((uint8*)tail,sizeof(tail));	
										}
										}
									}
								}
								timer2_disable();
								if(RF315TimeCount.TimeCount > 1000)
								{
									_315MHz_Flag = 0;
									U1_sendS((uint8*)ResFail, sizeof(ResFail));
								}
							}
							break;
	
							case 'F':	//433Mѧϰ
							{
								RFM69H_DATA_Type RF433_RxBuf;
									
						    	//U1_sendS((uint8*)RF433StudyCMD, sizeof(RF433StudyCMD));
							 	RFM69H_Config();
								RFM69H_EntryRx();
								if(RFM69H_RxPacket(&RF433_RxBuf)>0)
								{
								//	printf("len = %d\r\n", RF433_RxBuf.len);
									U1_sendS((uint8*)RF433SendCMD, sizeof(RF433SendCMD));
									U1_sendS((uint8*)&RF433_RxBuf, sizeof(RFM69H_DATA_Type));	//�����Ż����͵�����
								//	U1_sendS((uint8*)tail, sizeof(tail));	
								}
								else
								{
									U1_sendS((uint8*)ResFail, sizeof(ResFail));
								}
								timer2_enable();
							}
							break;
	
							case 'T':  //2.4Gѧϰ
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
	
						    case 'B':  //�򿪻��ѵ�
	//							U1_sendS("TF<<",4);
	                            debug_led_on();        //����led����
	//							U1_sendS("LB<<",4);
							break;
	
							case 'D':  //�رջ��ѵ�
								U1_sendS("TF<<",4);	
	                            debug_led_off();        
								U1_sendS("LD<<",4);
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
					   		case'H' ://���ⷢ��
							{
							}
							break;
							case  'W'://315M����
							{
								uint8 time = 0;
								RF315_DATA_t RF315_SendData;

								U1_sendS((uint8*)RF315SendCMD, sizeof(RF315SendCMD));							
								memcpy((uint8*)&RF315_SendData, &rec_buf[3], sizeof(RF315_DATA_t));
//								printf("TimeBase:%d, data[0]:%d, data[1]:%d, data[2]:%d\r\n", RF315_SendData.TimeBase,RF315_SendData.buff[0],RF315_SendData.buff[1],RF315_SendData.buff[2]);
								while(time < 6)//�ظ�����!
								{
									RF315_Send(&RF315_SendData);		  
									time ++;
								}
								U1_sendS((uint8*)ResSucess, sizeof(ResSucess));		
							}
							break;
							case 'F': //433M����
							{
								RFM69H_DATA_Type RF433_SendBuf;

							    U1_sendS((uint8*)RF433SendCMD, sizeof(RF433SendCMD));
								RFM69H_Config();
								RFM69H_EntryTx();
								memcpy((uint8*)&RF433_SendBuf, &rec_buf[3], sizeof(RFM69H_DATA_Type));

								printf("len = %d\r\n", RF433_SendBuf.len);
								RFM69H_TxPacket(&RF433_SendBuf);
								U1_sendS((uint8*)ResSucess, sizeof(ResSucess));	
							}
							break;
							case'T': //2.4G ����
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
				   case 'B':
				   {
				   		switch (rec_buf[1])
						{
							case'H' ://�������
							{
							}
							break;
							case  'W'://315M����
							{
						    	RF315_DATA_t  RF315_Receive;
				
								_315MHz_Flag = 1;
								RF315TimeCount.TimeCount = 0;
								RF315TimeCount.FlagStart = 1;
								timer2_enable();
								U1_sendS((uint8*)RF315RecCMD, sizeof(RF315RecCMD));
								while((RF315TimeCount.TimeCount <= 1000)&&(_315MHz_Flag == 1))
								{
								//	if(RF315_Rec(&RF315_Receive))
									{
										_315MHz_Flag = 0;
										U1_sendS((uint8*)RF315RecCMD, sizeof(RF315RecCMD));
										U1_sendS((uint8*)&RF315_Receive, sizeof(RF315_DATA_t));
										U1_sendS((uint8*)tail,sizeof(tail));	
									}
								}
								timer2_disable();
								if(RF315TimeCount.TimeCount > 1000)
								{
									_315MHz_Flag = 0;
									U1_sendS((uint8*)ResFail, sizeof(ResFail));
								}
							}
							break;
							case 'F': //433M����
							{
						    	RFM69H_DATA_Type RF433_RxBuf;
	
								U1_sendS((uint8*)RF433RecCMD, sizeof(RF433RecCMD));
								RF315TimeCount.TimeCount = 0;
								RF315TimeCount.FlagStart = 1;
								timer2_enable();
							 	RFM69H_Config();
								RFM69H_EntryRx();
								if(RFM69H_RxPacket(&RF433_RxBuf)>0)
								{
								//	U1_sendS((uint8*)RF433RecCMD, sizeof(RF433RecCMD));
									U1_sendS((uint8*)&RF433_RxBuf, sizeof(RFM69H_DATA_Type));	//�����Ż����͵�����
								//	U1_sendS((uint8*)tail, sizeof(tail));	
								}
								else
								{
									U1_sendS((uint8*)ResFail, sizeof(ResFail));
								}
								timer2_disable();
		
							}
							break;
							case'T': //2.4G ����
							{
								if(FlagRF24GLearn!=1)
								{
									U1_sendS((uint8*)RF433RecCMD, sizeof(RF433RecCMD));
									ifnnrf_rx_mode();
									RF24GTimeCount.TimeCount = 0;
									RF24GTimeCount.FlagStart = 1;
									timer2_enable();	
									FlagRF24GLearn = 2;
								}
							}
							case 'D': //��
							{
							}
							break;
		
							default :
							break;
						}
				   }
				   break;
	
				   case 'T':
					   if(rec_buf[1]=='K')	 //����
					   {
					   		U1_sendS((uint8*)ResSucess, sizeof(ResSucess));	
					   }
				   break;
	
				   case 'D':
					   if(rec_buf[1]=='T') //�¶Ȳɼ�
					   {
	//				   		uint16 temp;
	
	//					    U1_sendS("�ظ�����", 5);
							while((rec_buf[2] = GetTemperature()) == 0x55);	   // ��� ����Ǹ�ֵ��  ����жϲɼ�ʧ��
							rec_buf[3] = '<';
							rec_buf[4] = '<';
							U1_sendS(rec_buf, 5);	
					   }
					   else if(rec_buf[1]=='S')	//���wifi����
					   {
					   		Check_wifi = 1;
							Wifi_Command_Mode = 0;
							U1_sendS("DS<<",4);
					   }
					   else	if(rec_buf[1]=='M')	 //MAC ��ַ����
					   {};
	//				   else
	//				   {};
				   break;
	
				   case	'V': //�̼��汾��ȡ
				   		if(rec_buf[1]=='E')
						{}
				   break;
	
				   case 'S':
				   		if(rec_buf[1]=='X')//�̼�����
						{
						  if(rec_buf[1]=='X')
	                      {
	                            Boot_UsartSend("enter_upg_mode",sizeof("enter_upg_mode")-1);
	                            timer2_disable();
	                            Command_Process();
								timer2_enable();
	                      }
						}
				  break;
				   default :
				   break;
		 	}
			timer2_enable(); 
		    }
			else if(strstr(rec_buf,"+o") != NULL) //�յ�wifiģ�鷵�ص����� +ok
			{
				if(strstr(rec_buf,"AP") != NULL) 	//wifi������APģʽ
				{
					delay_ms(10);
					U1_sendS("AT+WAKEY\r\n",10);
				}
				else if(strstr(rec_buf,"OPEN") != NULL) //APģʽ�µ�open����  
				{
					Check_wifi = 0;
					Wifi_AP_OPEN_MODE = 1;
					timer2_enable(); 
					if(!start_wifi_data())
					{
						Check_wifi = 0;
						Wifi_Command_Mode = 0;
					}
				}
				else if(strstr(rec_buf,"MAC") != NULL)
				{
					memset(Wifi_MAC,0x00,sizeof(Wifi_MAC));
					memcpy(Wifi_MAC,strstr(rec_buf,"+ok="),sizeof(Wifi_MAC));
					if(!start_wifi_data())
					{
						Get_Wifi_MAC = 0;
						Wifi_Command_Mode = 0;
						U1_sendS("DM:",3);
						U1_sendS(Wifi_MAC,sizeof(Wifi_MAC));
						U1_sendS("<<",2);
					}
					Wifi_MAC_Count = 0;
					timer2_enable(); 
				}
				else
				{
					if(!start_wifi_data())
					{
						Check_wifi = 0;
						Wifi_Command_Mode = 0;
						Wifi_AP_OPEN_MODE = 0;
					}
				}
			}
		}
		rec_buf[2] = 0x00;//һ����������ִ�����, ���
		memset(rec_buf,0x00,sizeof(rec_buf));
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

