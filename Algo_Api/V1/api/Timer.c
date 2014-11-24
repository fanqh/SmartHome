#include"Timer.h"

extern  volatile unsigned int T ;	//����
extern  uint32  F;     //�Ƿ��38KH��������
extern  uint32  Wifi_AP_OPEN_MODE;     //�Ƿ��38KH��������
extern  uint32  Wifi_MAC_Count;
extern  uint32  Get_Wifi_MAC;
volatile uint32 Turn=1;             //��ת��־
volatile uint8  read_led_status=0;


//5ms
void TIMER2_Handler(void)
{
  
    TIM2->SR = ~TIM_FLAG_Update;  //�����־λ
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
     
		if(Wifi_MAC_Count == 2*200) //3�붨ʱ
		{
			Get_Wifi_MAC = 1;
			Wifi_MAC_Count = 0;
		}	
	}
    #else
    read_led_status =~read_led_status;    
    wifi_led(read_led_status);
    #endif
}




/* TIM2�ж����ȼ����� */
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

/*�ж�����Ϊ13us*/
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=5000;	//�Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) 
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler= (64 - 1);	//ʱ��Ԥ��Ƶ�� 72M/72      
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);	// �������жϱ�־ 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);	// ����ʱ��    
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

}

void Disable_SysTick(void)
{
     SysTick->CTRL &= (~(1<<SYSTICK_ENABLE) ); 
}
void Enable_SysTick(void)
{
    SysTick->CTRL |= ((1<<SYSTICK_ENABLE) ); 
}

