#include"M3_315.h"
#include "gpio_config.h"
#include "stm32f10x_gpio.h"

#define M3_315_DATA_PIN         GPIO_Pin_8
#define M3_315_DATA_PORT        GPIOA

#define M3_315_CTRL             GPIO_Pin_3
#define M3_315_GPIO             GPIOB

#define Rx_315                  PAin(8)



#define  NARROW_TIMEOUT   1000    //UNIT : us

RF315_STATA  rf315_state = RF315_IDLE;





void m3_315_set(void)
{
    GPIO_SetBits(M3_315_GPIO,M3_315_CTRL);
}

void m3_315_clr(void)
{
    GPIO_ResetBits(M3_315_GPIO, M3_315_CTRL);
}

void    m3_315_io_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = M3_315_CTRL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(M3_315_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(M3_315_DATA_PORT, &GPIO_InitStructure);


    m3_315_clr();
}


//
//
//RF315_STATA Get_rf315_flag(void)
//{
//	return 	 rf315_state;
//}
//
//uint16 Get_TimeCount_CleanAndStart(void)
//{
//	uint16 temp = 0;
//
//	__disable_irq();
//	temp = RF315_TimeCount;
//	RF315_TimeCount = 0;
//    __enable_irq(); 
//	
//	return 	temp;
//}
//
//
//
//
//uint8 FlagTimeCount = 0;
//volatile uint16	RF315_TimeCount = 0;
//
//
//
//uint8 GetFlagTimeCount(void)
//{
//	return 	 FlagTimeCount;
//}
//
//uint16 RF_decode(RF315_DATA_t *pdata, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) 
//{
//	uint8 *p;
//	uint8 ii=0,k=0;
//	uint16 narrow = 0;
//	uint16 wide = 0;
//	uint16 TimeHigh = 0;  //高脉冲长度
//	uint16 TimeLow = 0;	  //低脉冲长度
//	uint16 Timebase = 0;
//
////	rf315_state = RF315_RECEIVING;
//	p = pdata->buff;
//
//
//    Enable_SysTick();		//启动定时器0
//	__disable_irq();
//	RF315_TimeCount = 0;
//    __enable_irq();
//	 
//	while(GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))	   //检测同步脉冲
//	{
//		if(RF315_TimeCount > NARROW_MAX)	  //4T
//			return 0;
//	}
//	narrow = Get_TimeCount_CleanAndStart();
//	while(!(GPIO_ReadInputDataBit(GPIOx,  GPIO_Pin))) 
//	{
//		if(RF315_TimeCount > WIDE_MAX)  //128T
//			return 0;
//		
//	}
//	wide = Get_TimeCount_CleanAndStart();
//
////	printf("narrow = %d， wide = %d\r\n", narrow, wide);
//	if(((narrow>NRROW_MIN )&& (narrow*24)<wide) && (wide<(narrow*38)))  //narrow >20 为过LV
//	{
//		Timebase = pdata->TimeBase = (wide + narrow) / 32;     // T
//		for(ii=0; ii<LEN_MAX; ii++)
//		{
//			for(k=0; k<8; k++)
//			{
//				Get_TimeCount_CleanAndStart();
//				while((GPIO_ReadInputDataBit(GPIOx,  GPIO_Pin))) 
//				{
//					if(RF315_TimeCount > WIDE_MAX)
//					return 0;
//				}
//				TimeHigh = Get_TimeCount_CleanAndStart();	 //高脉冲时间
//				while(!(GPIO_ReadInputDataBit(GPIOx,  GPIO_Pin)) )
//				{
//					if(RF315_TimeCount > WIDE_MAX)
//						return 0;
//				}   
//				TimeLow = Get_TimeCount_CleanAndStart();	//低脉冲时间
//#if 1
//				if(TimeHigh>(Timebase-Timebase/2-Timebase/3)&&TimeHigh<(Timebase*1.96))
//				{	
//					if((TimeLow>(Timebase*1.96))&&(TimeLow<(Timebase*5)))
//						*p &= ~ (1<<((7-k)));		//解码0	
//					else if(((Timebase*24)<TimeLow) && (TimeLow<(Timebase*38)))	  //下一个同步码
//					{
////						printf("k = %d, ii = %d\r\n",k, ii);
////						for(k= 0; k<ii;k++)
////						{
////							printf("%X\r\n", pdata->buff[k]);
////						}
//						pdata->len = ii;								  
//						return ii;
//					}
//					else
//					{
////						printf("1: TimeLow = %d, TimeHigh = %d\r\n", TimeLow,TimeHigh);
//						return 0;
//					}
//				}	
//				else if((TimeHigh>(Timebase*1.96))&&(TimeHigh<(Timebase*5)))
//				{
//					if(TimeLow>(Timebase-Timebase/2-Timebase/3)&&TimeLow<(Timebase*1.96))
//						*p |= (1<<(7-k));		   //解码1
//					else
//					{
////						printf("2: TimeLow = %d, TimeHigh = %d\r\n", TimeLow,TimeHigh);
//						return 0;
//					}
//				}      			        
//               else 
//			   {
////			   		printf("3: TimeLow = %d, TimeHigh = %d\r\n", TimeLow,TimeHigh);
//			   		return 0;	  //乱码退出
//			   }  
//#endif       	
//	    	}
//			++p;
//		}	
//	}
//	return 0;
//}
//
//
//uint16 RFDecodeAC(RF315_DATA_t *pdata, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
//{
//		uint16 len = 0;
//
//	  	FlagTimecount = 1; //开启记时标记
//		len = RF_decode(pdata, GPIOx, GPIO_Pin);
//		FlagTimecount = 1; //开启记时标记
//
//		return len;
//			
//}
//
//
////uint8 RF315_Rec(RF315_DATA_t *pdata) // 改为，解析成功，超时，
////{
////	
////	if(RF_decode(pdata)== M315_DATA_LEN)  ///可变
////	{
////		return 1;
////	}
////		
////    return 0;  
////
////}
////315M 字码发送
//void RF315_SendBit1(uint32 TimeBase)
//{
//	__disable_irq();
//	RF315_TimeCount = 0;
//    __enable_irq();	
//	
//	m3_315_set();
//	while(RF315_TimeCount <= TimeBase*3) ;
//
//	__disable_irq();
//	RF315_TimeCount = 0;
//    __enable_irq();	
//	m3_315_clr();
//	while(RF315_TimeCount <= TimeBase) ;
//			
//}
//
//void RF315_SendBit0(uint32 TimeBase)
//{
//	__disable_irq();
//	RF315_TimeCount = 0;
//    __enable_irq();	
//	
//	m3_315_set();
//	while(RF315_TimeCount <= TimeBase) ;
//
//	__disable_irq();
//	RF315_TimeCount = 0;
//    __enable_irq();	
//	m3_315_clr();
//	while(RF315_TimeCount <= TimeBase*3) ;
//			
//}
//
//void RF315_SendSyn(uint32 TimeBase)	   //同步脉冲
//{
//	__disable_irq();
//	RF315_TimeCount = 0;
//    __enable_irq();	
//	
//	m3_315_set();
//	while(RF315_TimeCount <= TimeBase) ;
//
//	__disable_irq();
//	RF315_TimeCount = 0;
//    __enable_irq();	
//	m3_315_clr();
//	while(RF315_TimeCount <= TimeBase*31) ;
//}
//
//int RF315_Send(RF315_DATA_t *pdata)
//{
//	uint8 *p;
//	uint8 i, j;
//	p = pdata->buff;
//
//	rf315_state = RF315_SENDING;
//	Enable_SysTick();		//启动定时器0
//	RF315_SendSyn(pdata->TimeBase);
//	for(i=0; i<pdata->len; i++)
//	{
//		for(j= 0; j<8; j++)
//		{
//			if(*p & (1<<(7-j)))	 //先发送高位
//				RF315_SendBit1(pdata->TimeBase);
//			else
//				RF315_SendBit0(pdata->TimeBase);		
//		}
//		p++;	
//	}
//	rf315_state = RF315_IDLE;	
//	Disable_SysTick();		//关闭定时器
//	return 1;
//}
//



