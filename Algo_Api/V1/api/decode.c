 #include "include.h"

 
#define  NARROW_MAX    1000/TIME_UNIT		//1MS	 4T	խ�����
#define  WIDE_MAX      31*NARROW_MAX	    //32MS	 124T �������
#define  NRROW_MIN     100/TIME_UNIT		//խ������� 100us

uint8 FlagTimeCount = 0;
volatile uint16	TimeCount = 0;

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
	uint16 TimeHigh = 0;  //�����峤��
	uint16 TimeLow = 0;	  //�����峤��
	uint16 Timebase = 0;

	p = pdata->buff;

    Enable_SysTick();		//������ʱ��0
	__disable_irq();
	TimeCount = 0;
    __enable_irq();
	 
	while(GPIO_ReadInputDataBit( GPIOx,  GPIO_Pin))	   //���ͬ������
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

//	printf("narrow = %d�� wide = %d\r\n", narrow, wide);
	if(((narrow>NRROW_MIN )&& (narrow*24)<wide) && (wide<(narrow*38)))  //narrow >20 Ϊ��LV
	{
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
	}
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

int RFCodeAC_Send(RF_AC_DATA_TYPE *pdata , GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	uint8 *p;
	uint8 i, j;
	p = pdata->buff;

//	rf315_state = RF315_SENDING;
	FlagTimeCount = 1; //������ʱ���
	Enable_SysTick();		//������ʱ��0
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
	FlagTimeCount = 0; //������ʱ���
//	rf315_state = RF315_IDLE;	
	Disable_SysTick();		//�رն�ʱ��
	return 1;
}
