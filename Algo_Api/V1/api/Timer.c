#include"Timer.h"
#include "DK_RFM.h"

extern  volatile unsigned int T ;	//计数
extern  uint32  F;     //是否打开38KH方波调制
extern  uint32  Wifi_AP_OPEN_MODE;     //是否打开38KH方波调制
extern  uint32  Wifi_MAC_Count;
extern  uint32  Get_Wifi_MAC;
volatile uint32 Turn=1;             //反转标志
volatile uint8  read_led_status=0;

uint16_t ted = 0;
extern unsigned char RxBuf[RxBuf_Len];


//5ms
void TIMER2_Handler(void)
{
	unsigned char Rec;
  
    TIM2->SR = ~TIM_FLAG_Update;  //清除标志位
    #if 1
    if(Wifi_AP_OPEN_MODE)
	{
		Wifi_AP_OPEN_MODE++;
		if(Wifi_AP_OPEN_MODE == 50)
		{
            read_led_status =~read_led_status;    
			wifi_led(read_led_status);
			Wifi_AP_OPEN_MODE = 1;
		}
	}
	else
	{
		Wifi_MAC_Count++;
		ted ++;
     
		if(Wifi_MAC_Count == 2*200) //3秒定时
		{
			Get_Wifi_MAC = 1;
			Wifi_MAC_Count = 0;
		}
		if(ted == 200)	
		{
			uint8 i = 0;

			ted = 0;
			Rec = RFM69H_RxPacket(RxBuf);
			if(Rec == 1)  //接收数据
		  	{
				Rec = 0;
				//WAKEUP_LED = ~WAKEUP_LED;
				if(RxBuf[0] != RxBuf[1])
				{
					U1_send(0x7E);
					U1_send(0x7F);
					for(i=0;i<RxBuf_Len;i++)	
					{
						U1_send(RxBuf[i]);//Uart_sendB(RxBuf[i]);
					}
					U1_sendS("<<",2);
		 			RFM69H_EntryRx();
				}
		  	}
			else
			{
				RFM69H_EntryRx();
			}
		}
	}
    #else
    read_led_status =~read_led_status;    
    wifi_led(read_led_status);
    #endif
}




/* TIM2中断优先级配置 */
void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*中断周期为5ms*/
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=5000;	//自动重装载寄存器周期的值(计数值) 
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= (64 - 1);	//时钟预分频数 72M/72      
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);	// 清除溢出中断标志 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);	// 开启时钟    
    BSP_IntVectSet(BSP_INT_ID_TIM2, TIMER2_Handler);
 }

void timer2_init(void)
{
     TIM2_NVIC_Configuration();
     TIM2_Configuration();
  //   timer2_disable();

}
void timer2_enable(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);	
}
void timer2_disable(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);
}
void isr_13us(void)
{
    T++;
    if(F)                                                          
	{
        if (Turn)
        {
            Turn=0;
            SET_INFRARED;
        }
        else
        {
            Turn=1;
            CLR_INFRARED;
        }
    }
	if(Get_RFM69H_Status()!=RFM69H_IDLE)
	{
		DataTimeCount ++;
	}

}

void Disable_SysTick(void)
{
     SysTick->CTRL &= (~(1<<SYSTICK_ENABLE) ); 
}
void Enable_SysTick(void)
{
    SysTick->CTRL |= ((1<<SYSTICK_ENABLE) ); 
}

