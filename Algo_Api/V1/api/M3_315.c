#include"M3_315.h"
#include "gpio_config.h"

#define M3_315_DATA_PIN         GPIO_Pin_8
#define M3_315_DATA_PORT        GPIOA

#define M3_315_CTRL             GPIO_Pin_3
#define M3_315_GPIO             GPIOB

#define Rx_315                  PAin(8)

uint8 recv_flg  = 0;//bit right or fault
uint8 lianji_flg = 0; //long or short(1,0)
uchar short_k;     //窄脉冲宽度
uchar da1527[2][3];  //解码过程中临时数组 
uint8 rf_ok;
uint8  decode_ok;




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


void delay(void)//10US  //原来注释8.5us 
{ 	
        uint8 i=3;
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        
        __NOP();
        __NOP();
        while(i--)
        {
           __NOP();
           __NOP();
           __NOP();
           __NOP();
           __NOP();
           
        }
}

void RF_decode() 
{
	uchar ii=0,j=0,k=0,rep=0;
	uint8 head_k=0;           //短脉冲宽度
	//uchar s;
	//数据接收
	short_k=0;
	while(Rx_315 && j<250)
	{
		BSP_uDelay(8);	
		short_k++;

	}
	while(!Rx_315) 
	{
		BSP_uDelay(8);
		head_k++;
		
	}
	if(((short_k*24)<head_k) && (head_k<(short_k*38)))
	{
		for(rep=0;rep<2;rep++)
		{
			for(ii=0;ii<3;ii++)//3字节
			{
				for(k=0;k<8;k++)
				{
					j=0;
					while(Rx_315 && j<245) 
					{
						BSP_uDelay(8);
						j++;
					}
					if(j>(short_k-short_k/2-short_k/3)&&j<(short_k*1.96))
					{					
						da1527[rep][ii]&=~(1<<((7-k)));
					}	
					else if(j>(short_k*1.96)&&j<(short_k*5))da1527[rep][ii]|=(1<<(7-k)); 	        			        
	             else {return;}          //乱码退出	
					j=0;
					while(!Rx_315 && j<150){BSP_uDelay(8);j++;}      //跳过低电平 
				}
			}
			j=0;while(Rx_315 && (j<200)){BSP_uDelay(8);j++;}            //跳个最后一个高脉冲
			head_k=0;while(!Rx_315) {BSP_uDelay(8);head_k++;} //检测下一个前导信号的寬度  
			if((head_k<(short_k*26)) || (head_k>(short_k*38)))  {return;}
		}
		if((da1527[0][0]==da1527[1][0]) && (da1527[0][1]==da1527[1][1]) && (da1527[0][2]==da1527[1][2]))	//两次接收到的数据相同
		{
			/*uchar u,i,x;
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
			}*/
			decode_ok=1;
		}
	}

}

extern uint8 _315MHz_Flag;
extern uint8  _315MHz_TimeCount2; 
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



