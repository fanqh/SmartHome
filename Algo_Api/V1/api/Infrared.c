#include "Infrared.h"



void infrared_io_init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = INFRARED_SNED;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(INFRARED_PORT, &GPIO_InitStructure);

    SET_INFRARED;
    
    GPIO_InitStructure.GPIO_Pin = INFRARED_RECIEVE;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(INFRARED_PORT, &GPIO_InitStructure);

}

///发送口平常是高电平
void infrared_send_data(uint8 data)
{

}
uint8 infrared_read_data(void)
{

    return 1;
}

