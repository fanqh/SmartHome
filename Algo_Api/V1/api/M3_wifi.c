#include"M3_wifi.h"
#include "gpio_config.h"

//#define   MISO     PAin(15)   ///key

extern uint32 ui;
extern uint8 rec_buf[512];
extern uint32 Wifi_Command_Mode;
wifi_state_t wifi_state;


void wifiReloadPinConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = M3_WIFI_NRELOAD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(M3_WIFI_GPIO, &GPIO_InitStructure);
}


void wifi_init(void)
{
	wifi_state = WIFI_IDLE;		//WIFI状态初始化
	wifiReloadPinConfig();	
}

uint8 ScanKey(void)
{
	uint8  key = 0;

	if(PAin(15)==0)
	{
		BSP_mDelay(10);
		if(0 == PAin(15))
			key = 1;		
	}
	return key;		
}

uint8 Wifi_EnterEntmProcess(void)
{
	uint8 temp[64];
	uint16 count;
	uint8 ret = 0;	

	if(wifi_state==WIFI_IDLE)
	{
		if(start_wifi_command()==1)
			wifi_state = WIFI_COMMAND;	
	}
	else if(wifi_state == WIFI_COMMAND)
	{

		Boot_UsartClrBuf();
		memset(temp, 0x00, 64);
		U1_sendS("AT+WMODE\r\n", 10);
		BSP_mDelay(300);

		if(get_usart_interrupt_flg())
		{
			count =  get_usart_interrupt_flg();
			Boot_UsartGet(temp, count, 10);	
		}
		if(strstr(temp, "AP") != NULL)
		{ 	
			wifi_state = WIFI_WORKING_AP;
		}		
		else
			wifi_state = WIFI_IDLE;			
	}
	else if(wifi_state == WIFI_WORKING_AP)
	{
		Boot_UsartClrBuf();
		U1_sendS("AT+WAKEY\r\n",10);
		BSP_mDelay(300);
		if(get_usart_interrupt_flg())
		{
			count =  get_usart_interrupt_flg();
			Boot_UsartGet(temp, count, 10);	
		}
		if(strstr(temp, "OPEN") != NULL)
		{
			memset(temp, 0x00, 64);
			wifi_state = WIFI_SET_AUTH_OPEN;		
		}
		else
			wifi_state = WIFI_IDLE;		
	}
	else if(wifi_state == WIFI_SET_AUTH_OPEN)
	{
		Boot_UsartClrBuf();
		Boot_UsartSend("AT+ENTM\r\n",9);
		BSP_mDelay(300);
		if(get_usart_interrupt_flg())
		{
			count =  get_usart_interrupt_flg();
			Boot_UsartGet(temp, count, 10);	
		}
		if(strstr(temp, "+ok") != NULL)
			wifi_state = WIFI_ENTM;		
		else
			wifi_state = WIFI_IDLE;		
	}
	else if(wifi_state == WIFI_ENTM)
	{
		ret = 1;
	}
	memset(temp, 0x00, 64);
	return ret;

} 



#if 1
void U1_in(void)//串口1接收数据
{
    

    uint32 j=0,RI=0;
	ui = 0;

	while(j < 20)//超时退出 字节间隔超出20ms 退出
	{
        RI = get_usart_interrupt_flg();  //获取buff size
		if(RI)
		{
//			printf("%d", RI);
			Boot_UsartGet(&rec_buf[ui], RI, RI);	   //超时参数为什么填了个RI
			if(rec_buf[ui] == '<' && rec_buf[ui - 1] == '<')
				break;
			ui += RI;
            RI=0;
			j = 0;			
		}
		else
			j++;
		BSP_mDelay (1);
	}	
}

int start_wifi_command(void)
{
	Boot_UsartClrBuf();
	Boot_UsartSend("+++",3);
	memset(rec_buf,0x00,sizeof(rec_buf));	
    Boot_UsartGet(rec_buf,1,1200);
	if(rec_buf[0] == 'a')
	{	
		memset(rec_buf,0x00,sizeof(rec_buf));
//		BSP_mDelay(1000);
		U1_sendS("a",1);
        Boot_UsartGet(rec_buf,3,3000);
		if(strstr(rec_buf,"+ok") != NULL)
		{
			Wifi_Command_Mode = 1;
			memset(rec_buf,0x00,sizeof(rec_buf));
			return 1; //切换成功
		}	
	}
	memset(rec_buf,0x00,sizeof(rec_buf));
	return 0;

}



int start_wifi_data()
{
	Boot_UsartSend("AT+ENTM\r\n",9);
    memset(rec_buf,0x00,sizeof(rec_buf));	
    Boot_UsartGet(rec_buf,20,1200);
	if(strstr(rec_buf,"+ok") != NULL)
	{		
		Wifi_Command_Mode = 0;
		memset(rec_buf,0x00,sizeof(rec_buf));
		return PIPE_MODE; //切换成功
	}
	memset(rec_buf,0x00,sizeof(rec_buf));
	return AT_MODE;	

}

#endif

void U1_send(uint8 data)
{

    Boot_UsartSend(&data, 1);

}

void U1_sendS(uint8 *data,uint32 len)
{
    Boot_UsartSend(data, len);

}

void m3_wifi_rec_data(uint8 *data)
{


}

void m3_wifi_rec_buf(uint8 *data,uint32 len,uint32 timeout)
{


}

