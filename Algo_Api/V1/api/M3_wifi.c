#include"M3_wifi.h"

extern uint32 ui;
extern uint8 rec_buf[800];
extern uint32 Wifi_Command_Mode;


void m3_wifi_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = M3_WIFI_NRELOAD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(M3_WIFI_GPIO, &GPIO_InitStructure);

}
void m3_wifi_rst_input(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = M3_WIFI_NRELOAD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(M3_WIFI_GPIO, &GPIO_InitStructure);
}

void m3_wifi_rst_output(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = M3_WIFI_NRELOAD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(M3_WIFI_GPIO, &GPIO_InitStructure);
}
#if 1
void U1_in(void)//串口1接收数据
{
    

    uint32 j=0,RI=0;
	ui = 0;

	
	while(j < 100)//超时退出 字节间隔超出100ms 退出
	{
        RI=get_usart_interrupt_flg();  //获取buff size
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
//        delayus(1000);
		BSP_mDelay (1);
	
	}
	printf("%c,%c\r\n",rec_buf[0],rec_buf[1]);	

}

int start_wifi_command(void)
{

	Boot_UsartSend("+++",3);
	memset(rec_buf,0x00,sizeof(rec_buf));	
    Boot_UsartGet(rec_buf,1,1200);
	if(rec_buf[0] == 'a')
	{	
		memset(rec_buf,0x00,sizeof(rec_buf));
		delay_ms(1000);
		U1_sendS("a",1);
        Boot_UsartGet(rec_buf,3,3000);
		if(strstr(rec_buf,"+ok") != NULL)
		{
			Wifi_Command_Mode = 1;
			memset(rec_buf,0x00,sizeof(rec_buf));
			return AT_MODE; //切换成功
		}	
	}
	memset(rec_buf,0x00,sizeof(rec_buf));
	return PIPE_MODE;

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

