#include "Infrared.h"


uint8 GetVersioncmd[2] = {0xFA,0XF1};
uint8 GetIDcmd[2] = {0xFA,0XF2};
uint8 RepeatSentcmd[2] = {0xFA,0XF3};
uint8 ExitStudycmd[2] = {0xfa,0xf4};
uint8 EnterClassStudycmd[2] = {0xfa,0xf5};
uint8 Resetcmd[2] = {0xfa,0xf6};
uint8 Wakeupcmd[2] = {0xfa,0xf7};
uint8 Sleepcmd[2] = {0xfa,0xf8};
uint8 EnterSutdycmd[2] = {0xfa,0xfd};

uint8 InfraredEnterStudy(void)
{	 
	uint8 ret;
	uint8 temp = 0xff;

	InfraredUsartClrBuf();
	Infrared_UsartSend(EnterSutdycmd, sizeof(EnterSutdycmd));
	Infrared_UsartGet(&temp, 1, 30);
	if(temp==0)
		ret = 1;
	else
		ret =0;

	return ret;
}
uint8 InfraredExitStudy(void)
{
	uint8 ret;
	uint8 temp = 0xff;

	InfraredUsartClrBuf();
	Infrared_UsartSend(ExitStudycmd, sizeof(ExitStudycmd));
	Infrared_UsartGet(&temp, 1, 30);
	if(temp==0)
		ret = 1;
	else
		ret =0;

	return ret;	
}
uint8 InfraredReset(void)
{

	Infrared_UsartSend(Resetcmd, sizeof(Resetcmd));
	InfraredUsartClrBuf();
	return 1;	
}

uint8 ParseInfrared(infrared_data_t*  ptr)
{
	uint8 timecount = 0;
	uint16 count;
	uint8 buff[INFRARED_LEN];

	if(GetUartBuffSize())
	{
		while(timecount<20)
		{
			count = GetUartBuffSize();
			if(count)
			{
				Infrared_UsartGet(buff, count, count);
				if((buff[0]==0xFA)&&(buff[buff[1]+1+2]==0xED))
				{
					ptr->len = buff[1] + 3;
					memcpy(ptr->buff, buff, ptr->len);
					return 1;
				}
			}
			else
				count++;
			BSP_mDelay(1);		
		}
	}
	return 0;		
}








