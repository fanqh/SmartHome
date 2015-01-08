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
	rfm69h 解析和发送需要一个误差小于10us的定时器，，现在用的和systemtick有冲突，需要修改
*/
/**********************************************************************************/
/**********************************************************************************/

//const uint8 ResSucess[] = "RE:0<<";
//const uint8 ResFail[]  =  "RE:1<<";
//const uint8 tail[]  = "<<" ;

uint32  Flag = 0;	  //是否打开38KH方波调制
uint32  Wifi_Command_Mode = 0; //=1 wifi工作在命令模式 =0 工作在数据传输模式
uint32  Check_wifi = 1;		//检测wifi工作模式
uint32  Get_Wifi_MAC = 0; //检测wifi模块MAC地址标志，只在STA模式下检测
uint32  Wifi_MAC_Count = 0;
uint32  Wifi_AP_OPEN_MODE = 0; //wifi工作在AP的OPEN模式下，灯闪烁
uint32  RST_count1 = 0; //计数
uint32  RST_count2 = 0;
volatile uint32 T = 0;	//计数


uint32  i = 0;//计数用 
uint32  j = 0;//计数用
uint32  c = 0;//计数用

volatile uint32 ui = 0;//串口接收数据长度!
uint8   rec_buf[256];
#define wifi_mac_num 16
volatile uint8 Wifi_MAC[wifi_mac_num] = {0x00};

volatile uint8 RI=0;


//RF_RFM69H
RFM69H_DATA_Type TxBuf;// = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x6d};  


//315M
uint8 _315MHz_Flag;
uint8  _315MHz_TimeCount2;
unsigned int _315MHz_TimeCount;

//2.4G
unsigned char tx_test[17] = {0x7E,0x7E,0x34,0x43,0x10,0x10,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};



typedef union //char型数据转int型数据类 
{  
	unsigned short  ue; 
	unsigned char 	 u[2]; 
}U16U8;
U16U8  M;//两个8位转16位




uint32 wifi_work_mode;




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
//int main(void)
{
//    uint32 reclen=0;
//    uint16 temp=0;
//    uint8 data;
    //315M设置
	_315MHz_Flag = 0;
	_315MHz_TimeCount = 0;
	_315MHz_TimeCount2 = 1;

	JTAG_Set(SWD_ENABLE);		//加
	GPIOC->CRL&=0XFFF0FFFF;	//PC4推挽输出
 	GPIOC->CRL|=0X00030000;


	led_init();
    wifi_led(LED_ON);  
    debug_led_off();

    m3_315_io_config();
    infrared_io_init();  
//    m3_315_clr();                       //关闭315


  //Command_Process();
    Init_DS18B20();
    timer2_init();                      //5ms 中断
	TIM3_NVIC_Configuration(); 		    //BSP_Delay 初始化
    
    app_enroll_tick_hdl(isr_13us, 0);   //13us在底层配置的，配置完成就关闭了
    Disable_SysTick();
	SpiMsterGpioInit(SPI_2);

	printf("uart is working\r\n"); 


    while(1)
    {
#if 1
		  if(_315MHz_Flag)	//315M学习
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
				timer2_disable(); 	   //这里定时器和打开函数没有成对出现
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
			U1_in();			//获取串口发送的SJ数据!
			if(rec_buf[2] == ':')//接收到正确的控制数据!
			{
				timer2_disable(); 
				switch(rec_buf[0])
				{
				 case 'L':
				 {
				 	switch(rec_buf[1])
					{
						case 'H': //红外学习
						
						break;

						case 'W':   //315M学习
						{
							_315MHz_Flag = 1;
						}
						break;

						case 'F':	//433M学习
						{
							RFM69H_DATA_Type RF433_RxBuf;

							U1_sendS("LF", 2);
						 	RFM69H_Config();
							RFM69H_EntryRx();
							if(RFM69H_RxPacket(&RF433_RxBuf)>0)
							{
								U1_sendS("FF:", 3);	
								U1_sendS((uint8*)&RF433_RxBuf, sizeof(RFM69H_DATA_Type));	//可以优化发送的数据
								U1_sendS((uint8*)tail, sizeof(tail));	
							}
							else
							{
								U1_sendS((uint8*)ResFail, sizeof(ResFail));
							}


						}
							break;
						case 'T':  //2.4G学习
							break;

					    case 'B':  //打开唤醒灯
//							U1_sendS("TF<<",4);
                            debug_led_on();        //唤醒led点亮
//							U1_sendS("LB<<",4);
						break;

						case 'D':  //关闭唤醒灯
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
			   		case'H' ://红外发射
					{
					}
					break;
					case  'W'://315M发射
					{
						uint8 time = 0;
						RF315_DATA_t RF315_SendData;
//						U1_sendS("BW", 2);						
						memcpy((uint8*)&RF315_SendData, &rec_buf[3], sizeof(RF315_DATA_t));
						while(time < 6)//重复次数!
						{
							RF315_Send((RF315_DATA_t*) (&rec_buf[4]));
							time ++;
						}
//						m3_315_clr();		//关闭定时器0
						U1_sendS((uint8*)ResSucess, sizeof(ResSucess));		
					}
					break;
					case 'F': //433M发射
					{
//						U1_sendS("FF", 2);	
						RFM69H_Config();
						RFM69H_EntryTx();
						RFM69H_TxPacket((RFM69H_DATA_Type*)&rec_buf[4]);
						U1_sendS((uint8*)ResSucess, sizeof(ResSucess));	
					}
					break;
					case'T': //2.4G 发射
					{
						Ifnnrf_Send(&rec_buf[4]);
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
					case'H' ://红外接受
					{
					}
					break;
					case  'W'://315M接收
					{


					}
					break;
					case 'F': //433M接收
					{

					}
					break;
					case'T': //2.4G 接收
					{
					}
					case 'D': //绑定
					{}
					break;

					default :
					break;
					}
			   }
			   break;

			   case 'T':
				   if(rec_buf[1]=='K')	 //心跳
				   {
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
                            timer2_disable();
                            Command_Process();
							timer2_enable();
                      }

					}
			  break;
			   default :
			   break;
		 }

























//					case 'F'://红外、无线数据发射!
//						wifi_led(LED_OFF);  
//						if(rec_buf[1]=='H')//红外
//						{							
//							i = 4;//第3与4位是数据长度,从第4位是红外、无线控制数据
//							M.u[1] = rec_buf[3];
//							M.u[0] = rec_buf[4];
//							j = M.ue;						
//							Enable_SysTick();		//启动定时器0
//							while(i < j)//j是数据长度-1!
//							{
//                                __disable_irq();
//								T = 0;
//								Flag = 1;
//                                __enable_irq(); 
//								i++;
//								if(rec_buf[i] == 0)//&&rec_buf[i+1]==0)
//								{
//									i += 2;
//									M.u[1] = rec_buf[i];
//									i++;	
//									M.u[0] = rec_buf[i];
//								}
//								else
//								{
//									M.u[1] = 0;	
//									M.u[0] = rec_buf[i];
//								}
//								while(T < M.ue);
//                                 __disable_irq();
//								T = 0;
//								Flag =0;
//                                 __enable_irq();
//            					SET_INFRARED;
//								i++;
//								if(rec_buf[i] == 0)//&&uip_appdata[i+1]==0)
//								{
//									i += 2;
//									M.u[1] = rec_buf[i];
//									i++;	
//									M.u[0] = rec_buf[i];
//								}
//								else
//								{
//									M.u[1] = 0;	
//									M.u[0] = rec_buf[i];
//								}
//								while(T < M.ue);								
//							}
//							Disable_SysTick();		//关闭13us定时器
//							U1_sendS("FH<<", 4); 
//							wifi_led(LED_ON);  
//						}
//						else if(rec_buf[1]=='W')  //315M module 发送
//						{						
//							c = 0;
//							while(c < 6)//重复次数!
//							{
//								RF315_Send((RF315_DATA_t*) (&rec_buf));
//								c++;
//							}
//							 m3_315_clr();		//关闭定时器0
//							U1_sendS("FW<<", 4);
//						}
//						else if(rec_buf[1]=='S')
//						{
//							U1_sendS("FS<<", 4);
//						}
//						break;
//
//					case 'C'://红外采集!
//				   	    U1_sendS("CA<<", 4);//返回到主机请按遥控器("<<"在U1_sendS中添加)
//						i = 5;//第3与4位是数据长度,从第4位是红外、无线控制数据
//						j = 0;
//						Enable_SysTick();		//启动定时器0
//						while(i < 756) //长度给这句有关-->>rec_buf[2] = i;//第三位是数据长度
//						{ 
//                            __disable_irq();
//							T = 1;  
//                            __enable_irq();
//     	  			        while(READ_INFRARED == 0);
//   	  				        if(T > 5)
//							{
//								M.ue = T;
//                                __disable_irq();
//	                            T = 1;
//                                __enable_irq();
//								if(M.u[1] > 0)
//								{
//									rec_buf[i] = 0;	//将接收的数据发送回去（删除//即生效）
//									i++;
//									rec_buf[i] = 0;	//将接收的数据发送回去（删除//即生效）
//									i++;
//									rec_buf[i] = M.u[1];	//将接收的数据发送回去（删除//即生效）
//									i++;
//								}
//								rec_buf[i] = M.u[0];
//								i++;	
//								while(READ_INFRARED == 1)
//								{								
//									if(T > 6000)//无数据退出								
//									{
//										rec_buf[i] = 0;
//										i++;
//
//										M.ue = i;
//										rec_buf[3] = M.u[1];//第3与4位是数据长度(包括数据头,不包括结尾!)
//										rec_buf[4] = M.u[0];//第3与4位是数据长度(包括数据头,不包括结尾!)
//									 		   
//										rec_buf[i] = '<';
//										i++;
//										rec_buf[i] = '<';
//										i++;
//
//										rec_buf[0] = 'C';
//										rec_buf[1] = 'H';
//										rec_buf[2] = ':';																	
//										U1_sendS(rec_buf, i);//红外采集成功
//
//										i = 756;
//										break;
//									}
//								}
//								if(i < 756)
//								{				
//									M.ue = T;
//				
//									if(M.u[1] > 0)
//									{
//										rec_buf[i] = 0;
//										i++;
//										rec_buf[i] = 0;
//										i++;
//										rec_buf[i] = M.u[1];
//										i++;
//									}
//									rec_buf[i] = M.u[0];
//									i++;
//									j = 0;
//								}
//							}
//							else
//							{
//								while(READ_INFRARED == 1)
//								{
//									if(T > 50000)
//									{
//                                        __disable_irq();
//										T = 0;
//                                        __enable_irq();
//										j++;
//										if(j > 30)
//										{
//											i = 756;
//											U1_sendS("CC<<", 4);//超时退出!大约20秒无操作退出!
//											break;
//										}
//									}
//								}
//							}
//						}
//						Disable_SysTick();		//关闭定时器0
//						break;
//					case 'D':
//
//							_315MHz_Flag = 1;
//
//					
//					 		RFM69H_Config();
//							RFM69H_EntryTx();		// 每间隔一段时间，发射一包数据，并接收 Acknowledge 信号
//							RFM69H_TxPacket(&TxBuf);
//							//CLOSE_RX_OK;	//熄灭指定的LED
//							RFM69H_EntryRx();
//							delay_ms(200);
//							U1_sendS("BF<<",4);	
//
//							switch(rec_buf[1])
//							{
//								case 'T'://温度
//										memset(rec_buf,0x00,sizeof(rec_buf));
//										rec_buf[0] = 'D';
//										rec_buf[1] = 'T';
//										while((rec_buf[2] = GetTemperature()) == 0x55);
//										rec_buf[3] = '<';
//										rec_buf[4] = '<';
//										U1_sendS(rec_buf, 5);	
//										break;
//								case 'S'://检测wifi工作模式
//										Check_wifi = 1;
//										Wifi_Command_Mode = 0;
//										U1_sendS("DS<<",4);
//										break;
//								default:break;				
//							}
//							break;
//					case 'L': //唤醒状态指示灯
//							if(rec_buf[1] == 'B')
//							{
//								int i = 0;
//								for(i = 0;i < 17; i++)
//									tx_buf[i] = tx_test[i];
//								tx_buf[17]=0x01;
//								tx_buf[18]=0x01;
//								for(i = 19;i < 32; i++)
//									tx_buf[i] = i;
//								do_2_4G();
//
//								U1_sendS("TF<<",4);
//                                debug_led_on();        //唤醒led点亮
//								U1_sendS("LB<<",4);
//							}	
//							else if(rec_buf[1] == 'D')
//							{
//
//								int i = 0;
//								for(i = 0;i < 17; i++)
//									tx_buf[i] = tx_test[i];
//								tx_buf[17]=0x01;
//								tx_buf[18]=0x00;
//								for(i = 19;i < 32; i++)
//									tx_buf[i] = i;
//								do_2_4G();
//
//								U1_sendS("TF<<",4);	
//                                debug_led_off();        
//								U1_sendS("LD<<",4);
//							}
//							break;
//                     case 'S':
//                        if(rec_buf[1]=='X')
//                        {
//                            Boot_UsartSend("enter_upg_mode",sizeof("enter_upg_mode")-1);
//                            timer2_disable();
//                            Command_Process();
//                        }
//						break;
//
//				   	case 'G': //2.4 GH: 第3,4位是数据长度，从第5位是数据位
//						//int i = 0;
//						for(i=0;i<32;i++)
//						{
//							tx_buf[i] = rec_buf[i+3];
//							//SendUart(tx_buf[i]);
//						}
// 						do_2_4G();
//					    U1_sendS("TF<<",4);
//					    break;
//
//					case 'B':
//							RFM69H_Config();
//							RFM69H_EntryTx();		// 每间隔一段时间，发射一包数据，并接收 Acknowledge 信号
//							RFM69H_TxPacket(&TxBuf);
//							//CLOSE_RX_OK;	//熄灭指定的LED
//							RFM69H_EntryRx();
//							delay_ms(200);
//							U1_sendS("BF<<",4);	
//                        break;
//
//					case 'L':	//315M
//					{
//						switch(rec_buf[1])
//							case 'W':
//								_315MHz_Flag = 1;
//							break;
//								
//							case 'H':
//								
//					}					
//
//
//
//
//						if(rec_buf[1] == 'W')
//						{
//							_315MHz_Flag = 1;
//							delay_ms(200);
//							U1_sendS("WF<<",4);
//						}
//						else if(rec_buf[1] == 'H')
//						{
//
//						}
//					  break;
//					default:break;	
//				}
//				timer2_enable(); 
			}
			else if(strstr(rec_buf,"+o") != NULL) //收到wifi模块返回的数据 +ok
			{
				if(strstr(rec_buf,"AP") != NULL) 	//wifi工作在AP模式
				{
					delay_ms(10);
					U1_sendS("AT+WAKEY\r\n",10);
				}
				else if(strstr(rec_buf,"OPEN") != NULL) //AP模式下的open加密  
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
		rec_buf[2] = 0x00;//一个串口命令执行完毕, 清空
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

