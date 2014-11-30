#include"Include.h"
#include "DK_RFM.h"
#include "spi.h"

uint32  F = 0;	  //�Ƿ��38KH��������
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
uint8   rec_buf[800];
#define wifi_mac_num 16
volatile uint8 Wifi_MAC[wifi_mac_num] = {0x00};

volatile uint8 RI=0;


//RF_RFM69H
#define TxBuf_Len 10 
#define RxBuf_Len 10 
unsigned char TxBuf[TxBuf_Len] = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x6d};  
unsigned char RxBuf[RxBuf_Len];
uint8 _315MHz_Flag;



typedef union //char������תint�������� 
{  
	unsigned short  ue; 
	unsigned char 	 u[2]; 
}U16U8;
U16U8  M;//����8λת16λ




uint32 wifi_work_mode;

int tamain(void)
//int main(void)
{
//    uint32 reclen=0;
//    uint16 temp=0;
//    uint8 data;
    

	GPIOC->CRL&=0XFFF0FFFF;	//PC4�������
 	GPIOC->CRL|=0X00030000;
    led_init();
    m3_315_io_config();
    infrared_io_init();
    
    m3_315_clr();                       //�ر�315
    wifi_led(LED_ON);  
    debug_led_off();

  //Command_Process();
    Init_DS18B20();

    timer2_init();                      //5ms �ж�
    
    app_enroll_tick_hdl(isr_13us, 0);   //13us�ڵײ����õģ�������ɾ͹ر���
    Disable_SysTick();




		SpiMsterGpioInit();
//		SpiInit(SPI_DataSize_8b);

	 	RFM69H_Config();
		RFM69H_EntryRx();
    while(1)
	{
//	uint8 status;
//
//    SPIWrite(0x0423);
//   status = SPIRead(0x04);
//    Boot_UsartSend(&status,1);
		 if(RFM69H_RxPacket(RxBuf))			 //
		 	U1_sendS(RxBuf,1);
	}




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
    while(1);
#endif
    while(1)
    {
#if 1
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
				timer2_disable(); 
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
			U1_in();//��ȡ���ڷ��͵�SJ����!
			if(rec_buf[2] == ':')//���յ���ȷ�Ŀ�������!
			{
				timer2_disable(); 
				switch(rec_buf[0])
				{
					case 'F'://���⡢�������ݷ���!
						wifi_led(LED_OFF);  
						if(rec_buf[1]=='H')//����
						{							
							i = 4;//��3��4λ�����ݳ���,�ӵ�4λ�Ǻ��⡢���߿�������
							M.u[1] = rec_buf[3];
							M.u[0] = rec_buf[4];
							j = M.ue;						
							Enable_SysTick();		//������ʱ��0
							while(i < j)//j�����ݳ���-1!
							{
                                __disable_irq();
								T = 0;
								F=1;
                                __enable_irq(); 
								i++;
								if(rec_buf[i] == 0)//&&rec_buf[i+1]==0)
								{
									i += 2;
									M.u[1] = rec_buf[i];
									i++;	
									M.u[0] = rec_buf[i];
								}
								else
								{
									M.u[1] = 0;	
									M.u[0] = rec_buf[i];
								}
								while(T < M.ue);
                                 __disable_irq();
								T = 0;
								F=0;
                                 __enable_irq();
            					SET_INFRARED;
								i++;
								if(rec_buf[i] == 0)//&&uip_appdata[i+1]==0)
								{
									i += 2;
									M.u[1] = rec_buf[i];
									i++;	
									M.u[0] = rec_buf[i];
								}
								else
								{
									M.u[1] = 0;	
									M.u[0] = rec_buf[i];
								}
								while(T < M.ue);								
							}
							Disable_SysTick();		//�ر�13us��ʱ��
							U1_sendS("FH<<", 4); 
							wifi_led(LED_ON);  
						}
						else if(rec_buf[1]=='W')
						{						
							c = 0;
							Enable_SysTick();		//������ʱ��0
							while(c < 6)//�ظ�����!
							{
                                __disable_irq();
								T = 0;
                                 __enable_irq();
								 m3_315_set();
								i = 4;//��3��4λ�����ݳ���,�ӵ�5λ�Ǻ��⡢���߿�������
								while(T < 28);//(13 * 808 = 10504ͬ������!	
								 __disable_irq();
								T = 0;
                                 __enable_irq();
								 m3_315_clr();
								M.u[1] = rec_buf[3];
								M.u[0] = rec_buf[4];
								j = M.ue;//�������ɵĳ���Ҫ��1
								while(T < 808);//(13 * 808 = 10504ͬ������!

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
									i++;//i�ڴ�,��׼һЩ
									while(T < rec_buf[i]);
								}
								c++;
							}
							Disable_SysTick();
							 m3_315_clr();		//�رն�ʱ��0
							U1_sendS("FW<<", 4);
						}
						else if(rec_buf[1]=='S')
						{
							U1_sendS("FS<<", 4);
						}

						break;

					case 'C'://����ɼ�!
				   	    U1_sendS("CA<<", 4);//���ص������밴ң����("<<"��U1_sendS�����)
						i = 5;//��3��4λ�����ݳ���,�ӵ�4λ�Ǻ��⡢���߿�������
						j = 0;
						Enable_SysTick();		//������ʱ��0
						while(i < 756) //���ȸ�����й�-->>rec_buf[2] = i;//����λ�����ݳ���
						{ 
                            __disable_irq();
							T = 1;  
                            __enable_irq();
     	  			        while(READ_INFRARED == 0);
   	  				        if(T > 5)
							{
								M.ue = T;
                                __disable_irq();
	                            T = 1;
                                __enable_irq();
								if(M.u[1] > 0)
								{
									rec_buf[i] = 0;	//�����յ����ݷ��ͻ�ȥ��ɾ��//����Ч��
									i++;
									rec_buf[i] = 0;	//�����յ����ݷ��ͻ�ȥ��ɾ��//����Ч��
									i++;
									rec_buf[i] = M.u[1];	//�����յ����ݷ��ͻ�ȥ��ɾ��//����Ч��
									i++;
								}
								rec_buf[i] = M.u[0];
								i++;	
								while(READ_INFRARED == 1)
								{								
									if(T > 6000)//�������˳�								
									{
										rec_buf[i] = 0;
										i++;

										M.ue = i;
										rec_buf[3] = M.u[1];//��3��4λ�����ݳ���(��������ͷ,��������β!)
										rec_buf[4] = M.u[0];//��3��4λ�����ݳ���(��������ͷ,��������β!)
									 		   
										rec_buf[i] = '<';
										i++;
										rec_buf[i] = '<';
										i++;

										rec_buf[0] = 'C';
										rec_buf[1] = 'H';
										rec_buf[2] = ':';																	
										U1_sendS(rec_buf, i);//����ɼ��ɹ�

										i = 756;
										break;
									}
								}
								if(i < 756)
								{				
									M.ue = T;
				
									if(M.u[1] > 0)
									{
										rec_buf[i] = 0;
										i++;
										rec_buf[i] = 0;
										i++;
										rec_buf[i] = M.u[1];
										i++;
									}
									rec_buf[i] = M.u[0];
									i++;
									j = 0;
								}
							}
							else
							{
								while(READ_INFRARED == 1)
								{
									if(T > 50000)
									{
                                        __disable_irq();
										T = 0;
                                        __enable_irq();
										j++;
										if(j > 30)
										{
											i = 756;
											U1_sendS("CC<<", 4);//��ʱ�˳�!��Լ20���޲����˳�!
											break;
										}
									}
								}
							}
						}
						Disable_SysTick();		//�رն�ʱ��0
						break;
					case 'D':

						//	_315MHz_Flag = 1;

					
					 		RFM69H_Config();
							RFM69H_EntryTx();		// ÿ���һ��ʱ�䣬����һ�����ݣ������� Acknowledge �ź�
							RFM69H_TxPacket(TxBuf);
							//CLOSE_RX_OK;	//Ϩ��ָ����LED
							RFM69H_EntryRx();
							delay_ms(200);
							U1_sendS("BF<<",4);	

							switch(rec_buf[1])
							{
								case 'T'://�¶�
										memset(rec_buf,0x00,sizeof(rec_buf));
										rec_buf[0] = 'D';
										rec_buf[1] = 'T';
										while((rec_buf[2] = GetTemperature()) == 0x55);
										rec_buf[3] = '<';
										rec_buf[4] = '<';
										U1_sendS(rec_buf, 5);	
										break;
								case 'S'://���wifi����ģʽ
										Check_wifi = 1;
										Wifi_Command_Mode = 0;
										U1_sendS("DS<<",4);
										break;
								default:break;				
							}
							break;
					case 'L': //����״ָ̬ʾ��
							if(rec_buf[1] == 'B')
							{
                                debug_led_on();        //����led����
								U1_sendS("LB<<",4);
							}	
							else if(rec_buf[1] == 'D')
							{
                                debug_led_off();        
								U1_sendS("LD<<",4);
							}
							break;
                     case 'S':
                        if(rec_buf[1]=='X')
                        {
                            Boot_UsartSend("enter_upg_mode",sizeof("enter_upg_mode")-1);
                            timer2_disable();
                            Command_Process();
                        }
						break;

					case 'B':
							RFM69H_Config();
							RFM69H_EntryTx();		// ÿ���һ��ʱ�䣬����һ�����ݣ������� Acknowledge �ź�
							RFM69H_TxPacket(TxBuf);
							//CLOSE_RX_OK;	//Ϩ��ָ����LED
							RFM69H_EntryRx();
							delay_ms(200);
							U1_sendS("BF<<",4);	
                        break;
					default:break;	
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

