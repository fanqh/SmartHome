#include "stm32f10x.h"
#include "include.h"
#include "uart.h"



#define INFRARED_UART			    USART1
#define INFRARED_UART_TX		    GPIO_Pin_9
#define INFRARED_UART_RX		    GPIO_Pin_10
#define INFRARED_UART_GPIO		    GPIOA



#define UART_RXDATA         (INFRARED_UART->DR)
#define RX_BUF_START         UartBuff  
#define RX_BUF_END          (UartBuff + UART_LEN) 


volatile uint8 UartBuff[UART_LEN];
UART_INIT_STRUCT UartRx;
const uint32 BaudRate[6]={0,9600,19200,38400,57600,115200};

void InfraredUsartClrBuf(void)
{
    __disable_irq();
    UartRx.size = 0;
    UartRx.write = RX_BUF_START;
    UartRx.read = RX_BUF_START;
    memset(UartBuff, 0x00, sizeof(UartBuff));
    __enable_irq();
}



uint32   GetUartBuffSize(void)
{
    return UartRx.size;
}

void UART1_IRQHandler(void)
{
    __disable_irq();
	if(USART_GetITStatus(INFRARED_UART, USART_IT_RXNE) != RESET)
	{	
       if(UartRx.size<UART_LEN) //缓冲区未满 
       {
            if(UartRx.write >= RX_BUF_END)
        	{
        		UartRx.write = RX_BUF_START;
        	}
            *UartRx.write++ = UART_RXDATA;
            UartRx.size += 1;
       }
    }
    __enable_irq();
   
}




static void InfraredUsartConfig(uint32 newbaud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    
	USART_InitStructure.USART_BaudRate = BaudRate[newbaud];
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		  //使能 tx，RX

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = INFRARED_UART_TX;		         //设置TX引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		        //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(INFRARED_UART_GPIO, &GPIO_InitStructure);
	
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin =INFRARED_UART_RX;		        //设置RX引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//浮空输入
	GPIO_Init(INFRARED_UART_GPIO, &GPIO_InitStructure);
	
	/* USART configuration */
	USART_Init(INFRARED_UART, &USART_InitStructure);		        //初始化USART
	
	/* Enable USART */
	USART_Cmd(INFRARED_UART, ENABLE);		                       
	
	USART_ITConfig(INFRARED_UART, USART_IT_RXNE, ENABLE);           //接受中断
	while (USART_GetFlagStatus(INFRARED_UART, USART_FLAG_TC) == RESET);
	BSP_IntVectSet(BSP_INT_ID_USART1, UART1_IRQHandler);  

}
void Infrared_UsartInit(void)
{
    InfraredUsartClrBuf();
    InfraredUsartConfig(TA_BAUD_9600);
   // Reset_UsartDMA(cmd_data,0);

}
void Infrared_UsartSend(unsigned char *outptr,unsigned int len)
{
    while(len)
	{
		len--;
		USART_SendData(INFRARED_UART,(unsigned char)*outptr++);
		while (USART_GetFlagStatus(INFRARED_UART, USART_FLAG_TC) == RESET);
    }
	

}



int16 Infrared_UsartGet(uint8 *buff,uint32 len,uint32 timeout)
{
    uint32 i,tmpLen;    

    if(UartRx.size >= len)		            //数据可用
	{
         __disable_irq();
        if(UartRx.read + len > RX_BUF_END)  //>=
        {
           
            tmpLen = RX_BUF_END - UartRx.read;
            memcpy(buff,UartRx.read,tmpLen);
            UartRx.read = RX_BUF_START;
            memcpy(buff+tmpLen,(uint8 *)UartRx.read,len-tmpLen);
            UartRx.read+=len-tmpLen;
            UartRx.size-=len;
        }
        else
        {
            memcpy(buff, UartRx.read, len);
            UartRx.read += len;
            UartRx.size -= len;
        }
        __enable_irq();
        return len;
	}
    i=0;
    while(1)
    {
        BSP_mDelay(1);
        i++;

        if(UartRx.size >= len)		            //数据可用
    	{
            __disable_irq();
    		if(UartRx.read+len>RX_BUF_END)   //>==
            {
                tmpLen = RX_BUF_END - UartRx.read;
                memcpy(buff,UartRx.read,tmpLen);
                UartRx.read=RX_BUF_START;
                memcpy(buff+tmpLen,(uint8 *)UartRx.read,len-tmpLen);
                UartRx.read+=len-tmpLen;
                UartRx.size-=len;
            }
            else
            {
                memcpy(buff,(uint8 *)UartRx.read,len);
                UartRx.read+=len;
                UartRx.size-=len;
            }
            __enable_irq();
            return len;
    	}
        else
        {
            if(timeout && i >= timeout  )
            {
                __disable_irq();    
                if(UartRx.read+UartRx.size>=RX_BUF_END)
                {
                    tmpLen =RX_BUF_END-UartRx.read;
                    memcpy(buff,(uint8 *)UartRx.read,tmpLen);
                    UartRx.read=RX_BUF_START;
                    memcpy(buff+tmpLen,(uint8 *)UartRx.read,UartRx.size-tmpLen);
                    UartRx.read+=UartRx.size-tmpLen;                   
                    tmpLen=UartRx.size;
                    UartRx.size=0;
                }
                else
                {
            		memcpy(buff,(uint8 *)UartRx.read,UartRx.size);
                    UartRx.read+=UartRx.size;
                    tmpLen=UartRx.size;
                    UartRx.size=0;
                }
                __enable_irq();
                return tmpLen;
            }
        }

    }

}


