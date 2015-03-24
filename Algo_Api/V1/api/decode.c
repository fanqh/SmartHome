 #include "include.h"

 
 
#define  NARROW_MAX    1000/TIME_UNIT		//1MS	 4T	խ�����
#define  WIDE_MAX      31*NARROW_MAX	    //31MS	 124T �������
#define  NRROW_MIN     100/TIME_UNIT		//խ������� 100us

//NRVIBO ŷ�� ����˹����Э��
#define   SYN_L		(700-80)/TIME_UNIT
#define   SYN_H		(700+100)/TIME_UNIT
#define   SINGLE_L  80/TIME_UNIT
#define   SINGLE_H  (175+70)/TIME_UNIT
#define   DOUBLE_L  (250)/TIME_UNIT	    
#define   DOUBLE_H  100//(350+230)/TIME_UNIT

//OVRIBO
#define HALF_CYLE	175
//1T
#define  CYLE_1T_L  (350-100)/TIME_UNIT
#define  CYLE_1T_H  (350+80)/TIME_UNIT
//1.5T
#define  CYLE_15T_L  (525-80)/TIME_UNIT
#define  CYLE_15T_H  (525+80)/TIME_UNIT
//2T
#define  CYLE_2T_L  (700-80)/TIME_UNIT
#define  CYLE_2T_H  (700+100)/TIME_UNIT

uint8 FlagTimeCount = 0;
volatile uint16	TimeCount = 0;

		bool sucode = 0;
		uint16 time = 0;
		uint16  RF_ORVIBO_RECE[4];
		uint16 d[36];

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
	uint16 *p;
	uint8 ii=0,k=0;
	uint16 narrow = 0;
	uint16 wide = 0;
	uint16 TimeHigh = 0;  //�����峤��
	uint16 TimeLow = 0;	  //�����峤��
	uint16 Timebase = 0;

	sucode = 0;
	time = 0;
	p = pdata->buff;

    Enable_SysTick();		//������ʱ��0
	Get_TimeCount_CleanAndStart();
	 
	while(GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))	   //���ͬ������
	{
		if(TimeCount > NARROW_MAX)	  //4T
			return 0;
	}
	narrow = Get_TimeCount_CleanAndStart();
	if(narrow<10)
		return 0;
	while(!(GPIO_ReadInputDataBit(GPIOx,  GPIO_Pin))) 
	{
		if(TimeCount > WIDE_MAX)  //128T
			return 0;
		
	}
	wide = Get_TimeCount_CleanAndStart();

	//printf("narrow = %d�� wide = %d\r\n", narrow, wide);
	if(((narrow>NRROW_MIN )&& (narrow*24)<wide) && (wide<(narrow*38)))  //narrow >20 Ϊ��LV
	{  
	  	pdata->type = 0;
#if 1
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
				TimeHigh = Get_TimeCount_CleanAndStart();	 //������ʱ��
				while(!(GPIO_ReadInputDataBit(GPIOx,  GPIO_Pin)) )
				{
					if(TimeCount > WIDE_MAX)
						return 0;
				}   
				TimeLow = Get_TimeCount_CleanAndStart();	//������ʱ��
#if 1
				if(TimeHigh>(Timebase-Timebase/2-Timebase/3)&&TimeHigh<(Timebase*1.96))
				{	
					if((TimeLow>(Timebase*1.96))&&(TimeLow<(Timebase*5)))
						*p &= ~ (1<<((7-k)));		//����0	
					else if(((Timebase*24)<TimeLow) && (TimeLow<(Timebase*38)))	  //��һ��ͬ����
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
						*p |= (1<<(7-k));		   //����1
					else
					{
//						printf("2: TimeLow = %d, TimeHigh = %d\r\n", TimeLow,TimeHigh);
						return 0;
					}
				}      			        
               else 
			   {
//			   		printf("3: TimeLow = %d, TimeHigh = %d\r\n", TimeLow,TimeHigh);
			   		return 0;	  //�����˳�
			   }  
#endif       	
	    	}
			++p;
		}
#endif	
	}
	else if((narrow>SYN_L)&&(narrow<SYN_H)&&(wide>150/TIME_UNIT)&&(wide<250/TIME_UNIT)) //(Head>75/TIME_UNIT)&&(Head<250/TIME_UNIT)&&
	{
		k = 0;
		pdata->type = 1;
//		printf("narrow = %d, wide = %d\r\n\r\n", narrow*5, wide*5);	 
		while(k<36)
		{	
			while(GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))	   //���ͬ������
			{
				if(TimeCount > CYLE_2T_H)	  //2T
					return 0;
			}
			while(!(GPIO_ReadInputDataBit(GPIOx,  GPIO_Pin))) 
			{ 
				if(TimeCount > CYLE_2T_H)	  //2T
					return 0;		
			}
			time = d[k] = Get_TimeCount_CleanAndStart();
#if 1
			if((time>=CYLE_1T_L)&&(time<=CYLE_1T_H))
			{
				if(sucode==0)
					*(p+ k/9) |= (1UL<<(k%9));	
				else
					*(p+ k/9) &= ~(1UL<<(k%9));	
				++k;
			}
			else if((time>=CYLE_15T_L)&&(time<=CYLE_15T_H))
			{
				if(sucode==0)
				{
					*(p+ k/9) |= (1UL<<(k%9));	
					sucode = 1;
					++k;
				}
				else
				{
					sucode = 0;
					*(p+ k/9) &= ~(1UL<<(k%9));  ///     	
					++k;
					if(k>=36)
						return 0;
					///K�Ƿ����9
					*(p+ k/9) |= (1UL<<(k%9));
					++k;			
				}
			}
			else if((time>=CYLE_2T_L)&&(time<=CYLE_2T_H))
			{
				if(sucode==1)
				{
					sucode = 1;
					*(p+ k/9) &= ~(1UL<<(k%9));  ///     	
					++k;
					if(k>=36)
						return 0;
					///K�Ƿ����9
					*(p+ k/9) |= (1UL<<(k%9));
					++k;	
				}
				else
				{
//					printf("sucode: 0, time = 2T  High= %d, Low= %d, k= %d\r\n", TimeHigh, TimeLow, k);
					return 0;	
				}		
			}
			else
			{
//				printf("TIME_CYLE is overtime High= %d, Low= %d, time= %d, k= %d\r\n",TimeHigh, TimeLow, time, k);	
				return 0;
			}
#endif
		}	
		if(k>=36)
		{  	
			while(GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))	   //���ͬ������
			{	
				if(TimeCount > CYLE_2T_H)	  //2T
					return 0;
			}	
			if((TimeCount>=CYLE_2T_L)&&(TimeCount<=CYLE_2T_H))
			{
//				printf("receive RC800 is ok,\r\n\r\n");	
//				for(k=0;k<4;k++)
//					printf("%X\r\n\r\n", *(pdata->buff+ k));
				pdata->len = 4;
				return pdata->len;
			}
			else
				return 0;
		}
//	 	if(k>=36)
//		{
//			printf("receive RC800 is ok, i = %d, k = %d,  titme = %d\r\n\r\n", ii, k, time);
//			for(k=0; k<36; k++)
//				printf("k = %d, data = %d\r\n", k, d[k]);

//		}
//		printf("\r\n");
	}
//	if(k>=36)
//	{
//		printf("receive RC800 is ok, i = %d, k = %d,  titme = %d\r\n\r\n", ii, k, time);
//		for(k=0; k<36; k++)
//			printf("k = %d, data = %d\r\n", k, d[k]);
//		for(k=0;k<4;k++)
//			printf("%X\r\n\r\n", RF_ORVIBO_RECE[k]);
//	}																		   
	return 0;
}


uint16 RFDecodeAC(RF_AC_DATA_TYPE *pdata, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
		uint16 len = 0;

	  	FlagTimeCount = 1; //������ʱ���
		len = RF_decode(pdata, GPIOx, GPIO_Pin);
		FlagTimeCount = 0; //������ʱ���

		return len;
			
}



//315M ���뷢��
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

static void SendSyn(uint32 TimeBase, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)	   //ͬ������
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
static void OVRIBO_SendSyn( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	__disable_irq();
	TimeCount = 0;
    __enable_irq();	
	
	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_RESET);
	while(TimeCount <= HALF_CYLE/TIME_UNIT) ;

	__disable_irq();
	TimeCount = 0;
    __enable_irq();	

	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_SET);
	while(TimeCount <= 700/TIME_UNIT) ;	

	__disable_irq();
	TimeCount = 0;
    __enable_irq();	
	
	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_RESET);
	while(TimeCount <= HALF_CYLE/TIME_UNIT) ;
}

static void OVRIBO_SendBit0( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
		__disable_irq();
	TimeCount = 0;
    __enable_irq();	
	
	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_RESET);
	while(TimeCount <= HALF_CYLE/TIME_UNIT) ;

	__disable_irq();
	TimeCount = 0;
    __enable_irq();	

	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_SET);
	while(TimeCount <= HALF_CYLE/TIME_UNIT) ;	
}

static void OVRIBO_SendBit1( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
		__disable_irq();
	TimeCount = 0;
    __enable_irq();	
	
	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_SET);
	while(TimeCount <= HALF_CYLE/TIME_UNIT) ;

	__disable_irq();
	TimeCount = 0;
    __enable_irq();	

	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_RESET);
	while(TimeCount <= HALF_CYLE/TIME_UNIT) ;	
}

int RFCodeAC_Send(RF_AC_DATA_TYPE *pdata , GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	uint16 *p;
	uint8 i, j;
	p = pdata->buff;

//	rf315_state = RF315_SENDING;
	FlagTimeCount = 1; //������ʱ���
	Enable_SysTick();		//������ʱ��0
	if(pdata->type==0)
	{
		SendSyn(pdata->TimeBase, GPIOx, GPIO_Pin);
		for(i=0; i<pdata->len; i++)
		{
			for(j= 0; j<8; j++)
			{
				if(*p & (1<<(7-j)))	 //�ȷ��͸�λ
					SendBit1(pdata->TimeBase, GPIOx, GPIO_Pin);
				else
					SendBit0(pdata->TimeBase, GPIOx, GPIO_Pin);		
			}
			p++;	
		}																			
	}
	else																		  
	{
		OVRIBO_SendSyn( GPIOx, GPIO_Pin);	
		for(i=0; i<4; i++)
		{
			for(j=0; j<9; j++)
			{
				if(*p & (1<<(8-j)))	 //�ȷ��͸�λ
					OVRIBO_SendBit1(GPIOx, GPIO_Pin);
				else
					OVRIBO_SendBit0(GPIOx, GPIO_Pin);		
			}
			p++;
		}
	}
	FlagTimeCount = 0; //������ʱ���
//	rf315_state = RF315_IDLE;	
	Disable_SysTick();		//�رն�ʱ��
	return 1;
}

