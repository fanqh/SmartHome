#include"M3_315.h"

#define M3_315_CTRL             GPIO_Pin_3
#define M3_315_GPIO             GPIOB


void    m3_315_io_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = M3_315_CTRL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(M3_315_GPIO, &GPIO_InitStructure);

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


