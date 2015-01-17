#include"M3_315.h"
#include "gpio_config.h"

#define M3_315_DATA_PIN         GPIO_Pin_8
#define M3_315_DATA_PORT        GPIOA

#define M3_315_CTRL             GPIO_Pin_3
#define M3_315_GPIO             GPIOB

#define Rx_315                  PAin(8)


#define  NARROW_MAX    1000/TIME_UNIT		//1MS	 4T	窄脉冲最长
#define  WIDE_MAX      31*NARROW_MAX	    //32MS	 124T 宽脉冲最长
#define  NRROW_MIN     100/TIME_UNIT		//窄脉冲最短 100us



extern uint8 _315MHz_Flag;





#define  NARROW_TIMEOUT   1000    //UNIT : us

RF315_STATA  rf315_state = RF315_IDLE;
volatile uint16	RF315_TimeCount = 0;



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




RF315_STATA Get_rf315_flag(void)
{
	return 	 rf315_state;
}

uint16 Get_TimeCount_CleanAndStart(void)
{
	uint16 temp = 0;

	__disable_irq();
	temp = RF315_TimeCount;
	RF315_TimeCount = 0;
    __enable_irq(); 
	
	return 	temp;
}




uint16 RF_decode(RF315_DATA_t *pdata) 
{
	uint8 *p;
	uint8 ii=0,k=0;
	uint16 narrow = 0;
	uint16 wide = 0;
	uint16 TimeHigh = 0;  //高脉冲长度
	uint16 TimeLow = 0;	  //低脉冲长度
	uint16 Timebase = 0;

	rf315_state = RF315_RECEIVING;
	p = pdata->buff;


   Enable_SysTick();		//启动定时器0
	__disable_irq();
	RF315_TimeCount = 0;
    __enable_irq(); 
	while(Rx_315)	   //检测同步脉冲
	{
		if(RF315_TimeCount > NARROW_MAX)	  //4T
			return 0;
	}
	narrow = Get_TimeCount_CleanAndStart();
	while(!Rx_315) 
	{
		if(RF315_TimeCount > WIDE_MAX)  //128T
			return 0;
		
	}
	wide = Get_TimeCount_CleanAndStart();
	if(((narrow>NRROW_MIN )&& (narrow*24)<wide) && (wide<(narrow*38)))  //narrow >20 为过LV
	{
		Timebase = pdata->TimeBase = (wide + narrow)/32;     // T
//		printf("narrow = %d， wide = %d\r\n", narrow, wide);
		for(ii=0; ii<LEN_MAX; ii++)//3字节
		{
			for(k=0;k<8;k++)
			{
				Get_TimeCount_CleanAndStart();
				while(Rx_315) 
				{
					if(RF315_TimeCount > WIDE_MAX)
					return 0;
				}
				TimeHigh = Get_TimeCount_CleanAndStart();	 //高脉冲时间
				while(!Rx_315 )
				{
					if(RF315_TimeCount > WIDE_MAX)
						return 0;
				}   
				TimeLow = Get_TimeCount_CleanAndStart();	//低脉冲时间

				if(TimeHigh>(Timebase-Timebase/2-Timebase/3)&&TimeHigh<(Timebase*1.96))
				{	
					if((TimeLow>(Timebase*1.96))&&(TimeLow<(Timebase*5)))
						*p &= ~ (1<<((7-k)));		//解码0	
					else if(((Timebase*24)<TimeLow) && (Timebase<(narrow*38)))	  //下一个同步码
					{
//						printf("k = %d, ii = %d, rep = %d\r\n",k, ii, rep);
						pdata->len = ii;								  
						return ii;
					}
					else 
						return 0;
				}	
				else if((TimeHigh>(Timebase*1.96))&&(TimeHigh<(Timebase*5)))
				{
					if(TimeLow>(Timebase-Timebase/2-Timebase/3)&&TimeLow<(Timebase*1.96))
						*p |= (1<<(7-k));		   //解码1
					else
						return 0;
				}      			        
               else 
			   {
			   		return 0;	  //乱码退出
			   }         	
	    	}
			++p;
		}	
	}
	return 0;
}


uint8 RF315_Rec(RF315_DATA_t *pdata) // 改为，解析成功，超时，
{
	
	if(RF_decode(pdata)== M315_DATA_LEN)  ///可变
	{
		return 1;
	}
		
    return 0;  

}
//315M 字码发送
void RF315_SendBit1(uint32 TimeBase)
{
	__disable_irq();
	RF315_TimeCount = 0;
    __enable_irq();	
	
	m3_315_set();
	while(RF315_TimeCount <= TimeBase*3) ;

	__disable_irq();
	RF315_TimeCount = 0;
    __enable_irq();	
	m3_315_clr();
	while(RF315_TimeCount <= TimeBase) ;
			
}

void RF315_SendBit0(uint32 TimeBase)
{
	__disable_irq();
	RF315_TimeCount = 0;
    __enable_irq();	
	
	m3_315_set();
	while(RF315_TimeCount <= TimeBase) ;

	__disable_irq();
	RF315_TimeCount = 0;
    __enable_irq();	
	m3_315_clr();
	while(RF315_TimeCount <= TimeBase*3) ;
			
}

void RF315_SendSyn(uint32 TimeBase)	   //同步脉冲
{
	__disable_irq();
	RF315_TimeCount = 0;
    __enable_irq();	
	
	m3_315_set();
	while(RF315_TimeCount <= TimeBase) ;

	__disable_irq();
	RF315_TimeCount = 0;
    __enable_irq();	
	m3_315_clr();
	while(RF315_TimeCount <= TimeBase*31) ;
}

int RF315_Send(RF315_DATA_t *pdata)
{
	uint8 *p;
	uint8 i, j;
	p = pdata->buff;

	rf315_state = RF315_SENDING;
	Enable_SysTick();		//启动定时器0
	RF315_SendSyn(pdata->TimeBase);
	for(i=0; i<pdata->len; i++)
	{
		for(j= 0; j<8; j++)
		{
			if(*p & (1<<(7-j)))	 //先发送高位
				RF315_SendBit1(pdata->TimeBase);
			else
				RF315_SendBit0(pdata->TimeBase);		
		}
		p++;	
	}
	rf315_state = RF315_IDLE;	
	Disable_SysTick();		//关闭定时器
	return 1;
}




