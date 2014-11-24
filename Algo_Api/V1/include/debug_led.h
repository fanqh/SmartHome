#ifndef _DEBUG_LED_H
#define _DEBUG_LED_H



#include"Include.h"

enum led_status{LED_OFF=0,LED_ON=!LED_OFF};

void    led_init(void);

uint8   led_on(void); 

uint8   led_off(void);

void    wifi_led(uint8 led_status);

void    power_on_tip(void);

void    delay_ms(int times);

#endif
