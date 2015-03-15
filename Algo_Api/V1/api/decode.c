 #include "include.h"

 
#define  NARROW_MAX    1000/TIME_UNIT		//1MS	 4T	窄脉冲最长
#define  WIDE_MAX      31*NARROW_MAX	    //31MS	 124T 宽脉冲最长
#define  NRROW_MIN     100/TIME_UNIT		//窄脉冲最短 100us

//NRVIBO 欧瑞博 曼切斯特码协议
#define   SYN_L		(700-100)/TIME_UNIT
#define   SYN_H		(700+100)/TIME_UNIT
#define   SINGLE_L  2//(5)/TIME_UNIT
#define   SINGLE_H  (175+70)/TIME_UNIT
#define   DOUBLE_L  (250)/TIME_UNIT	    
#define   DOUBLE_H  (350+230)/TIME_UNIT

uint8 FlagTimeCount = 0;
volatile uint16	TimeCount = 0;

		bool sucode = 0;
		uint16 time = 0;
		uint16  RF_ORVIBO_RECE[4];

//RF315_STATA Get_rf315_flag(void)
//{
//	return 	 rf315_state;
//}

static uint16 Get_TimeCount_CleanAndStart(void)
{
	uint16 temp = 0;

	__disable_irq();
	temp = TimeCount;
	TimeCount = 0;
    __enable_irq(); 
	
	return 	temp;
}

uint8 GetFlagTimeCount(void)
{
	return 	 FlagTimeCount;
}

static uint16 RF_decode(RF_AC_DATA_TYPE *pdata, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) 
{
	uint8 *p;
	uint8 ii=0,k=0;
	uint16 narrow = 0;
	uint16 wide = 0;
	uint16 TimeHigh = 0;  //高脉冲长度
	uint16 TimeLow = 0;	  //低脉冲长度
	uint16 Timebase = 0;
     uint16     Head = 0;

	sucode = 0;
	time = 0;
	p = pdata->buff;

    Enable_SysTick();		//启动定时器0
	__disable_irq();
	TimeCount = 0;
    __enable_irq();

//	if(!GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))
//	{
		while(!(GPIO_ReadInputDataBit(GPIOx,  GPIO_Pin))) 
		{
			if(TimeCount > WIDE_MAX)  //128T
				return 0;
			
		}
		Head = Get_TimeCount_CleanAndStart(); 
		if((Head>75/TIME_UNIT)&&(Head<250/TIME_UNIT))
		{
		}
		else
			return 0;
//		printf("Head = %d\r\n", Head);
//	}
	 
	while(GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))	   //检测同步脉冲
	{
		if(TimeCount > NARROW_MAX)	  //4T
			return 0;
	}
	narrow = Get_TimeCount_CleanAndStart();
	while(!(GPIO_ReadInputDataBit(GPIOx,  GPIO_Pin))) 
	{
		if(TimeCount > WIDE_MAX)  //128T
			return 0;
		
	}
	wide = Get_TimeCount_CleanAndStart();

	//printf("narrow = %d， wide = %d\r\n", narrow, wide);
	if(((narrow>NRROW_MIN )&& (narrow*24)<wide) && (wide<(narrow*38)))  //narrow >20 为过LV
	{  
#if 0
		Timebase = pdata->TimeBase = (wide + narrow) / 32;     // T
		for(ii=0; ii<LEN_MAX; ii++)
		{
			for(k=0; k<8; k++)
			{
				Get_TimeCount_CleanAndStart();
				while((GPIO_ReadInputDataBit(GPIOx,  GPIO_Pin))) 
				{
					if(TimeCount > WIDE_MAX)
					return 0;
				}
				TimeHigh = Get_TimeCount_CleanAndStart();	 //高脉冲时间
				while(!(GPIO_ReadInputDataBit(GPIOx,  GPIO_Pin)) )
				{
					if(TimeCount > WIDE_MAX)
						return 0;
				}   
				TimeLow = Get_TimeCount_CleanAndStart();	//低脉冲时间
#if 1
				if(TimeHigh>(Timebase-Timebase/2-Timebase/3)&&TimeHigh<(Timebase*1.96))
				{	
					if((TimeLow>(Timebase*1.96))&&(TimeLow<(Timebase*5)))
						*p &= ~ (1<<((7-k)));		//解码0	
					else if(((Timebase*24)<TimeLow) && (TimeLow<(Timebase*38)))	  //下一个同步码
					{
//						printf("k = %d, ii = %d\r\n",k, ii);
//						for(k= 0; k<ii;k++)
//						{
//							printf("%X\r\n", pdata->buff[k]);
//						}
						pdata->len = ii;								  
						return ii;
					}
					else
					{
//						printf("1: TimeLow = %d, TimeHigh = %d\r\n", TimeLow,TimeHigh);
						return 0;
					}
				}	
				else if((TimeHigh>(Timebase*1.96))&&(TimeHigh<(Timebase*5)))
				{
					if(TimeLow>(Timebase-Timebase/2-Timebase/3)&&TimeLow<(Timebase*1.96))
						*p |= (1<<(7-k));		   //解码1
					else
					{
//						printf("2: TimeLow = %d, TimeHigh = %d\r\n", TimeLow,TimeHigh);
						return 0;
					}
				}      			        
               else 
			   {
//			   		printf("3: TimeLow = %d, TimeHigh = %d\r\n", TimeLow,TimeHigh);
			   		return 0;	  //乱码退出
			   }  
#endif       	
	    	}
			++p;
		}
#endif	
	}
	else if((narrow>SYN_L)&&(narrow<SYN_H)&&(wide>150/TIME_UNIT)&&(wide<250/TIME_UNIT)) //(Head>75/TIME_UNIT)&&(Head<250/TIME_UNIT)&&
	{
		printf("narrow = %d, wide = %d\r\n\r\n", narrow*5, wide*5);
//		printf("**start anylysis orvibo**\r\n\r\n");
		for(ii=0; ii<4; ii++)
		{
			for(k=9; k>0; k--)
			{
				if(sucode==0)
				{
				    Get_TimeCount_CleanAndStart();
					if(GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))
					{
						while(GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))	   //检测1 脉冲长度
						{
							if(TimeCount > DOUBLE_H)	  //4T
							{   
							    printf("erro: sucode: 0, GPIO: 1, >DOUBLE_H, sucode= %d, 1. timecount is %d, i= %d, k= %d\r\n\r\n",sucode,time*5, ii,k);
								//printf("erro: sucode: 0, GPIO: 1, >DOUBLE_H\r\n\r\n");
							//	BSP_mDelay(1000);
								return 0;
							}		
						}
						time = Get_TimeCount_CleanAndStart();							

						//printf("sucode= %d, 1. timecount is %d, i= %d, k= %d\r\n",sucode,time*5, ii,k);
						if(time > SINGLE_L)
							sucode = 1;
						else
						{									  
							printf("*****erro time = %d", time);  
							return 0;
						}
					}
				    else
					{
						while(!GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))	   //检测1 脉冲长度
						{
							if(TimeCount > DOUBLE_H)	  //4T
							{
								printf("erro: sucode: 0, GPIO: 0, >DOUBLE_H, 0. timecount is %d,i= %d, k= %d\r\n\r\n", time*5,ii,k);
							//	printf("erro: sucode: 0, GPIO: 0, >DOUBLE_H\r\n\r\n");
							//BSP_mDelay(1000);
								return 0;
							}
								
						}
						time = Get_TimeCount_CleanAndStart();	
						//printf("sucode= %d,  0. timecount is %d,i= %d, k= %d\r\n",sucode,time*5,ii,k);
						if(time>SINGLE_L)
							sucode = 1;
						else
						{									  
							printf("*****erro time = %d", time);  
							return 0;
						}
					}
				}
				else if(sucode==1)
				{
					Get_TimeCount_CleanAndStart();
					if(GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))
					{
						while(GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))	   //检测1 脉冲长度
						{
							if(TimeCount > DOUBLE_H)	  //4T
							{
								printf("erro: sucode: 1, GPIO: 1, >DOUBLE_H, timecount is %d, i= %d, k= %d\r\n\r\n", time*5,ii,k);
							//	printf("erro: sucode: 1, GPIO: 1, >DOUBLE_H\r\n\r\n");
						//	BSP_mDelay(1000);
								return 0;
							}
						}
						time = Get_TimeCount_CleanAndStart();	
						//printf("sucode= %d,  1. timecount is %d, i= %d, k= %d\r\n",sucode, time*5,ii,k);

					    if((time>SINGLE_L)&&(time<=SINGLE_H))
						{
							sucode = 0;	
							RF_ORVIBO_RECE[ii] &= ~(1UL<<ii);
						//	printf("bit = 0\r\n");

						}
						else if((time>DOUBLE_L)&&(time<DOUBLE_H))
						{
							sucode = 1;
							RF_ORVIBO_RECE[ii] &= ((1UL<<ii));
							//printf("sucode =1, bit = 0\r\n");
						}
						else
						{
							printf("erro: sucode: 1, GPIO: 1, other err\r\n\r\n,  1. timecount is %d, i= %d, k= %d\r\n\r\n", time*5,ii,k);
						//	printf("erro: sucode: 1, GPIO: 1, other err\r\n\r\n");
					//	BSP_mDelay(1000);
							return 0;
						}
					}
				    else
					{
						Get_TimeCount_CleanAndStart();
						while(!GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))	   //检测1 脉冲长度
						{
							if(TimeCount >= DOUBLE_H)	  //4T
							{
								printf("erro: sucode: 1, GPIO: 0, >DOUBLE_H, timecount is %d,i= %d, k= %d\r\n\r\n",time*5,ii,k);
							//	printf("erro: sucode: 1, GPIO: 1, >DOUBLE_H\r\n\r\n");
							//BSP_mDelay(1000);
								return 0;
							}
						}
						time = Get_TimeCount_CleanAndStart();	
					//	printf("sucode= %d,  0. timecount is %d,i= %d, k= %d\r\n",sucode,time*5,ii,k);
					    if((time>SINGLE_L)&&(time<=SINGLE_H))
						{
							sucode = 0;	
							RF_ORVIBO_RECE[ii] |= (1UL<<ii);
							//

						}
						else if((time>DOUBLE_L)&&(time<DOUBLE_H))
						{
							sucode = 1;
							RF_ORVIBO_RECE[ii] |= (1UL<<ii);
						}
						else
						{
							printf("erro: sucode: 1, GPIO: 0, other err, timecount is %d,i= %d, k= %d\r\n\r\n",time*5,ii,k);
							//printf("erro: sucode: 1, GPIO: 0, other err\r\n");
						//	BSP_mDelay(1000);
							return 0;	
						}
					}		
				}
					
			}
		}
		printf("receive RC800 is ok, i = %d, k = %d,  titme = %d\r\n\r\n", ii, k, time);
		for(ii=0; ii<4; ii++)
		{
			printf("%d  ", RF_ORVIBO_RECE[ii]);
		}
		printf("\r\n");
		return 0;	
	}																			   
		return 0;
}


uint16 RFDecodeAC(RF_AC_DATA_TYPE *pdata, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
		uint16 len = 0;

	  	FlagTimeCount = 1; //开启记时标记
		len = RF_decode(pdata, GPIOx, GPIO_Pin);
		FlagTimeCount = 0; //开启记时标记

		return len;
			
}



//315M 字码发送
static void SendBit1(uint32 TimeBase, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	__disable_irq();
	TimeCount = 0;
    __enable_irq();	
	
//	m3_315_set();
	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_SET);
	while(TimeCount <= TimeBase*3) ;

	__disable_irq();
	TimeCount = 0;
    __enable_irq();	
//	m3_315_clr();
	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_RESET);
	while(TimeCount <= TimeBase) ;
			
}

static void SendBit0(uint32 TimeBase, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	__disable_irq();
	TimeCount = 0;
    __enable_irq();	
	
//	m3_315_set();
	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_SET);
	while(TimeCount <= TimeBase) ;

	__disable_irq();
	TimeCount = 0;
    __enable_irq();	
//	m3_315_clr();
	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_RESET);
	while(TimeCount <= TimeBase*3) ;
			
}

static void SendSyn(uint32 TimeBase, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)	   //同步脉冲
{
	__disable_irq();
	TimeCount = 0;
    __enable_irq();	
	
//	m3_315_set();
	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_SET);
	while(TimeCount <= TimeBase) ;

	__disable_irq();
	TimeCount = 0;
    __enable_irq();	
//	m3_315_clr();
	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_RESET);
	while(TimeCount <= TimeBase*31) ;
}

int RFCodeAC_Send(RF_AC_DATA_TYPE *pdata , GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	uint8 *p;
	uint8 i, j;
	p = pdata->buff;

//	rf315_state = RF315_SENDING;
	FlagTimeCount = 1; //开启记时标记
	Enable_SysTick();		//启动定时器0
	SendSyn(pdata->TimeBase, GPIOx, GPIO_Pin);
	for(i=0; i<pdata->len; i++)
	{
		for(j= 0; j<8; j++)
		{
			if(*p & (1<<(7-j)))	 //先发送高位
				SendBit1(pdata->TimeBase, GPIOx, GPIO_Pin);
			else
				SendBit0(pdata->TimeBase, GPIOx, GPIO_Pin);		
		}
		p++;	
	}
	FlagTimeCount = 0; //开启记时标记
//	rf315_state = RF315_IDLE;	
	Disable_SysTick();		//关闭定时器
	return 1;
}

