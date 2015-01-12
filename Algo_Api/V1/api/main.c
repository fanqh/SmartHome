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
uint8 FlagRF24GLearn = 0; 


//315M
uint8 _315MHz_Flag;
uint8  _315MHz_TimeCount2;
unsigned int _315MHz_TimeCount;

//2.4G
unsigned char tx_test[17] = {0x7E,0x7E,0x34,0x43,0x10,0x10,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};



typedef union //char������תint�������� 
{  
	unsigned short  ue; 
	unsigned char 	 u[2]; 
}U16U8;
U16U8  M;//����8λת16λ




uint32 wifi_work_mode;




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
//int main(void)
{
//    uint32 reclen=0;
//    uint16 temp=0;
//    uint8 data;
    //315M����
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

	printf("uart is working\r\n"); 


    while(1)
    {
#if 1
		  if(_315MHz_Flag)	//315Mѧϰ
		  {
		  		RF315_DATA_t  RF315_Receive;

				if(RF315_Rec(&RF315_Receive))
				{
					_315MHz_Flag = 0;
					U1_sendS("LW:",3);
					U1_sendS((uint8*)&RF315_Receive, sizeof(RF315_DATA_t));
					U1_sendS((uint8*)tail,sizeof(tail));	
				}
				else
				{
					U1_sendS((uint8*)ResFail, sizeof(ResFail));
				}
		 }
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
							
							break;
	
							case 'W':   //315Mѧϰ
							{
								_315MHz_Flag = 1;
							}
							break;
	
							case 'F':	//433Mѧϰ
							{
								RFM69H_DATA_Type RF433_RxBuf;
	
								U1_sendS("LF", 2);
							 	RFM69H_Config();
								RFM69H_EntryRx();
								if(RFM69H_RxPacket(&RF433_RxBuf)>0)
								{
									U1_sendS("FF:", 3);	
									U1_sendS((uint8*)&RF433_RxBuf, sizeof(RFM69H_DATA_Type));	//�����Ż����͵�����
									U1_sendS((uint8*)tail, sizeof(tail));	
								}
								else
								{
									U1_sendS((uint8*)ResFail, sizeof(ResFail));
								}
							}
							break;
	
							case 'T':  //2.4Gѧϰ
							{
								U1_sendS((uint8*)RF433StudyCMD, sizeof(RF433StudyCMD));
								ifnnrf_rx_mode();
								RF24GTimeCount.TimeCount = 0;
								RF24GTimeCount.FlagStart = 1;
								timer2_enable();	
								FlagRF24GLearn = 1;
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
		//						U1_sendS("BW", 2);						
								memcpy((uint8*)&RF315_SendData, &rec_buf[3], sizeof(RF315_DATA_t));
								while(time < 6)//�ظ�����!
								{
									RF315_Send((RF315_DATA_t*) (&rec_buf[4]));
									time ++;
								}
//								m3_315_clr();		//�رն�ʱ��0
								U1_sendS((uint8*)ResSucess, sizeof(ResSucess));		
							}
							break;
							case 'F': //433M����
							{
		//						U1_sendS("FF", 2);	
								RFM69H_Config();
								RFM69H_EntryTx();
								RFM69H_TxPacket((RFM69H_DATA_Type*)&rec_buf[4]);
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
							}
							break;
							case 'F': //433M����
							{
		
							}
							break;
							case'T': //2.4G ����
							{
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

