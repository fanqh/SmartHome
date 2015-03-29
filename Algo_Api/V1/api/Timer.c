#include"Timer.h"
#include "DK_RFM.h"
#include "nrf24l01.h"
#include "M3_315.h"


//extern unsigned char RxBuf[RxBuf_Len];

TimeCountTypeDef RF315TimeCount;
TimeCountTypeDef RF24GTimeCount;
TimeCountTypeDef RF433TimeCount;
TimeCountTypeDef InfraredTimeCount;
extern uint8 FlagReloadKey;
extern volatile uint16	TimeCount;
extern uint16 RF433_Rec_Timeout;




//5ms
void TIMER2_Handler(void)
{
  	static uint16 t = 0;
	static uint8 led = 0;

    TIM2->SR = ~TIM_FLAG_Update;  //清除标志位

	t ++;
	if((wifi_state != WIFI_ENTM)&&(FlagReloadKey==0))
	{
		if(t%200==0)
		{
			led = ~led;
			wifi_led(led);
		}			
	}
	if(RF433_Rec_Timeout>0)
	{
		RF433_Rec_Timeout--;
	}
//	else
//	{
//		led_on();
//	}

	if(RF315TimeCount.FlagStart)
	{
		RF315TimeCount.TimeCount ++;
	}
	if(RF24GTimeCount.FlagStart)
	{
		RF24GTimeCount.TimeCount ++;
	}
	if(RF433TimeCount.FlagStart)
	{
		RF433TimeCount.TimeCount ++;
	}
	if(InfraredTimeCount.FlagStart)
	{
		InfraredTimeCount.TimeCount++;	
	}

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
    if(GetFlagTimeCount())
	{
		++ TimeCount;			
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


/***********************************************************
*added by fan
*************************************************************/

#define TIM_MSEC_DELAY                     0x01
#define TIM_USEC_DELAY                     0x02

volatile  uint32 BSP_delay = 0;



void USB_OTG_BSP_TimerIRQ (void)
{
    
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    if (BSP_delay > 0x00)
    { 
      BSP_delay--;
    }
    else
    {
      TIM_Cmd(TIM3,DISABLE);
    }
  }
} 

/* TIM3中断优先级配置 M3*/
void TIM3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	BSP_IntVectSet(BSP_INT_ID_TIM3, USB_OTG_BSP_TimerIRQ);
}



/**
  * @brief  BSP_SetTime
  *         Configures TIM2 for delay routine based on TIM2
  * @param  unit : msec /usec
  * @retval None
  */
static void BSP_SetTime(uint8_t unit)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  TIM_Cmd(TIM3,DISABLE);
  TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE); 
  
  
  if(unit == TIM_USEC_DELAY)
  {  
    TIM_TimeBaseStructure.TIM_Period = 1;		 //1us
  }
  else if(unit == TIM_MSEC_DELAY)
  {
    TIM_TimeBaseStructure.TIM_Period = 1000-1;	  //ms
  }
  TIM_TimeBaseStructure.TIM_Prescaler = 64-1;  //1M时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  
  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  
  /* TIM2 enable counter */ 
  TIM_Cmd(TIM3, ENABLE);  
} 




static void BSP_Delay(uint32_t nTime, uint8_t unit)
{
  
  BSP_delay = nTime;
  BSP_SetTime(unit);  
  while(BSP_delay != 0);
  TIM_Cmd(TIM3,DISABLE);
}



void BSP_uDelay (const uint32_t usec)
{
 
  BSP_Delay(usec,TIM_USEC_DELAY); 
 
}


/**
  * @brief  USB_OTG_BSP_mDelay
  *          This function provides delay time in milli sec
  * @param  msec : Value of delay required in milli sec
  * @retval None
  */
void BSP_mDelay (const uint32_t msec)
{  
    BSP_Delay(msec,TIM_MSEC_DELAY);   

}

