#include"debug_led.h"
#include "stm32f2xx_gpio.h"
#include"app_define.h"




#define  RED_LED				GPIO_Pin_0
#define  GREEN_LED				GPIO_Pin_1		




#define  BEEP					GPIO_Pin_7
#define  DEBUG_LED_PORT			GPIOB

void TIM_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);


    /* GPIOC Configuration: TIM3 CH1 (PC6), TIM3 CH2 (PC7), TIM3 CH3 (PC8) and TIM3 CH4 (PC9) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 

    /* Connect TIM3 pins to AF2 */  
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4); 
}
void Pwm_Start(void)
{
    //uint16_t PrescalerValue = 0;
    uint16_t CCR1_Val = 300;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    /* Compute the prescaler value */
    //PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 20000000) - 1;

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 500;
    TIM_TimeBaseStructure.TIM_Prescaler = 59;  //PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC2Init(TIM4, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);


    TIM_ARRPreloadConfig(TIM4, ENABLE);
	/* TIM4 enable counter */
  	//TIM_Cmd(TIM4, ENABLE);
	  /* TIM3 enable counter */
  	TIM_Cmd(TIM4, ENABLE);


	//TIM_Period  =  1000     按键     1K  hZ
	//TIM_Period  =  3436     错误提示以及报警的频率     291  hZ

	//TIM_Period(start)  =  1520	TIM_Period(end)  =  1000	  开门
}

void LED_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = RED_LED |GREEN_LED; 
	GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd =GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_2MHz;
	GPIO_Init(DEBUG_LED_PORT,&GPIO_InitStructure);


	#if BEEP_TYPE==YOUYUAN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = BEEP; 
	GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd =GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_2MHz;
	GPIO_Init(DEBUG_LED_PORT,&GPIO_InitStructure);
	#else
	TIM_Config();
	#endif
	Green_Led_Off();
	Red_Led_Off();
	Beep_Off();

}
 void Red_Led_On()
{
//	GPIO_ResetBits(DEBUG_LED_PORT,RED_LED);
          DEBUG_LED_PORT->BSRRH = RED_LED;
	
}
void Red_Led_Off()
{
//	GPIO_SetBits(DEBUG_LED_PORT,RED_LED);
        DEBUG_LED_PORT->BSRRL = RED_LED;
}
 void Green_Led_On()
{
//	GPIO_ResetBits(DEBUG_LED_PORT,GREEN_LED);
        DEBUG_LED_PORT->BSRRH = GREEN_LED;
}
void Green_Led_Off()
{
//	GPIO_SetBits(DEBUG_LED_PORT,GREEN_LED);
        DEBUG_LED_PORT->BSRRL = GREEN_LED;
}

void Beep_On(void)  
{
	#if BEEP_TYPE==YOUYUAN
	DEBUG_LED_PORT->BSRRL = GPIO_Pin_7;
	#else
	TIM_Config();
	Pwm_Start();
	#endif
}
void Beep_Off(void)    
{
	#if BEEP_TYPE==YOUYUAN
	DEBUG_LED_PORT->BSRRH = GPIO_Pin_7;
	#else
	GPIO_InitTypeDef GPIO_InitStructure;
	
	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = BEEP; 
	GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd =GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_25MHz;
	GPIO_Init(DEBUG_LED_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(DEBUG_LED_PORT,BEEP);

	TIM_Cmd(TIM4, DISABLE);
	#endif
}

void Beep_Delay_ms(unsigned int times)
{
	unsigned int i,j;
	#if 0
	i=times;
	while(i--)
	{
		j =40*1000;		//40
		while(j--)
		;
	}
	#else
	for(i=0;i<times;i++)
		for(j=0;j<20*1000;j++);
	#endif	
}
void Success_Voice(void)
{
	Beep_On();
	Beep_Delay_ms(20);
	Beep_Off();
	Beep_Delay_ms(40);
	Beep_On();
	Beep_Delay_ms(20);
	Beep_Off();
	Beep_Delay_ms(40);
	Beep_On();
	Beep_Delay_ms(20);
	Beep_Off();
}


void power_on_tip(void)
{

    Red_Led_On();
    Beep_On();
    Beep_Delay_ms(30);
    Beep_Off();
    Beep_Delay_ms(20);
    Red_Led_Off();
    Beep_Delay_ms(50);
    Red_Led_On();
    Beep_Delay_ms(50);
	Red_Led_Off();

}
void Fail_Voice(void)
{
	Beep_On();
	Beep_Delay_ms(200); //120
	Beep_Off();
	Beep_Delay_ms(80); //150
	Beep_On();
	Beep_Delay_ms(200);
	Beep_Off();
	Beep_Delay_ms(80);
	Beep_On();
	Beep_Delay_ms(200);
	Beep_Off();
 }
void Tip(void)
{
	Beep_On();
	Beep_Delay_ms(30);
	Beep_Off();
}

void Tip_Start(void)
{
	Beep_On();
	Beep_Delay_ms(50);
	Beep_Off();
}


