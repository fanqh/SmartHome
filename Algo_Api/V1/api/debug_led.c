#include"debug_led.h"




#define  LED1_PIN				GPIO_Pin_4		
#define  DEBUG_LED_PORT			GPIOA


void led_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = LED1_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(DEBUG_LED_PORT, &GPIO_InitStructure);

    led_off();

}

uint8    led_on(void)
{
    GPIO_SetBits(DEBUG_LED_PORT,LED1_PIN);
    return 1;
}


uint8    led_off(void)
{
    GPIO_ResetBits(DEBUG_LED_PORT,LED1_PIN);
    return 0;
}

void    wifi_led(uint8 led_status)
{
    if(led_status)
    {
        led_on();
    }
    else
        led_off();

}

void    power_on_tip(void)
{


}

void delay_ms(int times)
{
    int i;
    if(times<0)
        times=1;
    for(i=0;i<times;i++)
    {
        delay_x_10us(100);
    }

}
