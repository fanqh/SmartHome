#ifndef _INCLUDE_H
#define _INCLUDE_H

#include    "App_define.h"

#include    "bootapi.h"
#include    "stm32f10x_conf.h"
#include    "System_var.h"

#include    "M3_wifi.h"
#include    "ds18b20.h"
#include    "Infrared.h"
#include    "M3_315.h"
#include    "debug_led.h"
#include    "Timer.h"
#include    "decode.h"
#include    "uart.h"

 #define TIME_UNIT		5
 #define RF315LEARNTIMECOUNT   6000	   //单位 5ms  30s
 #define RF24GLEARNTIMECOUNT   6000	   //单位 5ms  30s
 #define INFRAREDLEARNTIMECOUNT   6000	   //单位 5ms  30s

#endif


