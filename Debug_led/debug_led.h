#ifndef   _DEBUG_LED
#define  _DEBUG_LED


#define WUYUAN			0	//ÎÞÔ´·äÃùÆ÷
#define YOUYUAN			1

#define BEEP_TYPE		WUYUAN



void LED_Init(void);
void Red_Led_On(void); ;
void Red_Led_Off(void);
void Green_Led_On(void);
void Green_Led_Off(void);;

void Beep_On(void);
void Beep_Off(void);

void Success_Voice(void);
void Fail_Voice(void);
void power_on_tip(void);
void Tip(void);
void Tip_Start(void);


void Beep_Delay_ms(unsigned int times);


#endif
