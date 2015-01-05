#include"M3_315.h"
#include "gpio_config.h"

#define M3_315_DATA_PIN         GPIO_Pin_8
#define M3_315_DATA_PORT        GPIOA

#define M3_315_CTRL             GPIO_Pin_3
#define M3_315_GPIO             GPIOB

#define Rx_315                  PAin(8)



extern uint8 _315MHz_Flag;
extern uint8  _315MHz_TimeCount2; 



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

int RF_decode(RF315_DATA_t *pdata) 
{
	uint8 data[2][3];
	uint8 ii=0,k=0,rep=0;
	uint32 temp = 0;
    uint16 timecount = 0;
	uint16 narrow = 0;
	uint16 wide = 0;
	rf315_state = RF315_RECEIVING;


   Enable_SysTick();		//启动定时器0
	__disable_irq();
	RF315_TimeCount = 0;
    __enable_irq(); 
	while(Rx_315)	   //检测同步脉冲
	{
		if(RF315_TimeCount > (100))
			return -1;
	}
	narrow = Get_TimeCount_CleanAndStart();
	while(!Rx_315) 
	{
		if(RF315_TimeCount > (1500))
			return -1;
		
	}
	wide = Get_TimeCount_CleanAndStart();
	pdata->TimeBase = (wide + narrow)/32;    

	if(((narrow>20)&&(narrow*24)<wide) && (wide<(narrow*38)))  //narrow >20 为过LV
	{

///		printf("narrow = %d， wide = %d\r\n", narrow, wide);
#if 1
		for(rep=0;rep<2;rep++)
		{
			for(ii=0;ii<3;ii++)//3字节
			{
				for(k=0;k<8;k++)
				{
					Get_TimeCount_CleanAndStart();
					while(Rx_315) 
					{
						if(RF315_TimeCount > (1500))
						return -1;
					}

					timecount = Get_TimeCount_CleanAndStart();
					if(timecount>(narrow-narrow/2-narrow/3)&&timecount<(narrow*1.96))
					{	
						data[rep][ii] &= ~ (1<<((7-k)));			
						//da1527[rep][ii]&=~(1<<((7-k)));	 //解码0
					}	
					else if((timecount>(narrow*1.96))&&(timecount<(narrow*5)))
					{
						data[rep][ii] |= (1<<(7-k));
						//da1527[rep][ii]|=(1<<(7-k)); 	 //解码1 
					}      			        
	               else 
				 	 {return -1;}          //乱码退出	
					while(!Rx_315 )
					{
						if(RF315_TimeCount > (1500))
							return -1;
					}      //跳过低电平 
				}
		     }
			Get_TimeCount_CleanAndStart();
			while(Rx_315 )
			{
				if(RF315_TimeCount > (100))
				return -1;
			}            //跳个最后一个高脉冲
			narrow = Get_TimeCount_CleanAndStart();
			while(!Rx_315) 
			{
				if(RF315_TimeCount > (1500))
				return -1;
			} //检测下一个前导信号的寬度  
			wide = Get_TimeCount_CleanAndStart();
			if((wide<(narrow*26)) || (wide > (narrow*38)))  
				{return -1;}
			temp = (narrow + wide)/32;

			pdata->TimeBase = (pdata->TimeBase + temp) / 2; //计算同步吗1bit的平均值
		}
		if((data[0][0]==data[1][0]) && (data[0][1] == data[1][1]) && (data[0][2]==data[1][2]))	//两次接收到的数据相同
		{

			memcpy(pdata->buff, data[0], 3);
			/*uint8 u,i,x;
			rf_ok=1;
			for(i=0;i<3;i++)  //判定2262与1527
			{
				for(u=0;u<4;u++) {if(((da1527[0][i]>>(u*2)) & 3)==2) {i=80;break;}}  //有10则为1527
				if(i==80) break;
			}
			if(i==80)  //1527
			{
				key_d=da1527[1][2] & 0x0f;         //分出1527的按键值
				da1527[0][2]=da1527[1][2]>>4; //分出1527的后4位地址
				jmnx=1;         //为0是2262，1是1527
			}
			else      //2262
			{
				key_d=0;
				for(i=0;i<4;i++){if(((da1527[0][2]>>(i*2))&3)==3) key_d|=1<<i;}   //计算出2262的按键数据                                  
				da1527[0][2]=00; //2262无后4位地址,全为0
				jmnx=0;         //为0是2262，1是1527
			}
			decode_ok=1; */

			return 3; //表示3个字节
		}	
	#endif
	}
	return -1;

}

void RF315_Rec(void)
{
	RF315_DATA_t  RF315_Receive;

	if(RF_decode(&RF315_Receive) == M315_DATA_LEN)  ///可变
	{
	
		U1_sendS("WF<<",4);
		U1_sendS((uint8*)&RF315_Receive, sizeof(RF315_DATA_t));
		U1_sendS("<<",2);
		_315MHz_Flag = 0;
		_315MHz_TimeCount2 = 1;
	}
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
	for(i=0; i<M315_DATA_LEN; i++)
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




