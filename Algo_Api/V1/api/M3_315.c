#include"M3_315.h"
#include "gpio_config.h"

#define M3_315_DATA_PIN         GPIO_Pin_8
#define M3_315_DATA_PORT        GPIOA

#define M3_315_CTRL             GPIO_Pin_3
#define M3_315_GPIO             GPIOB

#define Rx_315                  PAin(8)



extern uint8 _315MHz_Flag;
extern uint8  _315MHz_TimeCount2; 

uint8 recv_flg  = 0;//bit right or fault
uint8 lianji_flg = 0; //long or short(1,0)
uchar da1527[2][3];  //�����������ʱ���� 
uint8  decode_ok;

#define  NARROW_TIMEOUT   1000    //UNIT : us

uint8 rf315_receive_flag = 0;
volatile uint16	RF315_TimeCount = 0;


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


void    m3_315_set(void)
{
    GPIO_SetBits(M3_315_GPIO,M3_315_CTRL);
}

void    m3_315_clr(void)
{
    GPIO_ResetBits(M3_315_GPIO, M3_315_CTRL);
}

uint8 Get_rf315_flag(void)
{
	return 	 rf315_receive_flag;
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

void RF_decode() 
{
	uint8 ii=0,k=0,rep=0;

    uint16 timecount = 0;
	uint16 narrow = 0;
	uint16 wide = 0;
	rf315_receive_flag = 1;

   Enable_SysTick();		//������ʱ��0
	__disable_irq();
	RF315_TimeCount = 0;
    __enable_irq(); 
	while(Rx_315)	   //���ͬ������
	{
		if(RF315_TimeCount > (100))
			return ;
	}
	narrow = Get_TimeCount_CleanAndStart();
	while(!Rx_315) 
	{
		if(RF315_TimeCount > (1500))
			return ;
		
	}
	wide = Get_TimeCount_CleanAndStart();

	if(((narrow>20)&&(narrow*24)<wide) && (wide<(narrow*38)))  //narrow >20 Ϊ����
	{

//	   printf("narrow = %d�� wide = %d\r\n", narrow, wide);
#if 1
		for(rep=0;rep<2;rep++)
		{
			for(ii=0;ii<3;ii++)//3�ֽ�
			{
				for(k=0;k<8;k++)
				{
					Get_TimeCount_CleanAndStart();
					while(Rx_315) 
					{
						if(RF315_TimeCount > (1500))
						return ;
					}

					timecount = Get_TimeCount_CleanAndStart();
					if(timecount>(narrow-narrow/2-narrow/3)&&timecount<(narrow*1.96))
					{					
						da1527[rep][ii]&=~(1<<((7-k)));	 //����0
					}	
					else if((timecount>(narrow*1.96))&&(timecount<(narrow*5)))
						da1527[rep][ii]|=(1<<(7-k)); 	 //����1       			        
	               else 
				 	 {return;}          //�����˳�	
					while(!Rx_315 )
					{
						if(RF315_TimeCount > (1500))
							return ;
					}      //�����͵�ƽ 
				}
		     }
			Get_TimeCount_CleanAndStart();
			while(Rx_315 )
			{
				if(RF315_TimeCount > (100))
				return ;
			}            //�������һ��������
			narrow = Get_TimeCount_CleanAndStart();
			while(!Rx_315) 
			{
				if(RF315_TimeCount > (1500))
				return ;
			} //�����һ��ǰ���źŵČ���  
			wide = Get_TimeCount_CleanAndStart();
			if((wide<(narrow*26)) || (wide > (narrow*38)))  
				{return;}
		}
		if((da1527[0][0]==da1527[1][0]) && (da1527[0][1]==da1527[1][1]) && (da1527[0][2]==da1527[1][2]))	//���ν��յ���������ͬ
		{
			/*uint8 u,i,x;
			rf_ok=1;
			for(i=0;i<3;i++)  //�ж�2262��1527
			{
				for(u=0;u<4;u++) {if(((da1527[0][i]>>(u*2)) & 3)==2) {i=80;break;}}  //��10��Ϊ1527
				if(i==80) break;
			}
			if(i==80)  //1527
			{
				key_d=da1527[1][2] & 0x0f;         //�ֳ�1527�İ���ֵ
				da1527[0][2]=da1527[1][2]>>4; //�ֳ�1527�ĺ�4λ��ַ
				jmnx=1;         //Ϊ0��2262��1��1527
			}
			else      //2262
			{
				key_d=0;
				for(i=0;i<4;i++){if(((da1527[0][2]>>(i*2))&3)==3) key_d|=1<<i;}   //�����2262�İ�������                                  
				da1527[0][2]=00; //2262�޺�4λ��ַ,ȫΪ0
				jmnx=0;         //Ϊ0��2262��1��1527
			}*/

			printf("received is ok\r\n");
			decode_ok=1;
		}
	#endif
	}

}

void RF315_Rec(void)
{

	unsigned char i,j;
	RF_decode();
	if(decode_ok) 
	{
		decode_ok = 0;
		U1_sendS("WF<<",4);
		for(i=0;i<2;i++)
		{
			for(j=0;j<3;j++)
			U1_send(da1527[i][j]);
		}
		U1_sendS("<<",2);
		_315MHz_Flag = 0;
		_315MHz_TimeCount2 = 1;
	}
}



