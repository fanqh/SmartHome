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
//	Infrared_UsartSend(EnterSutdycmd, sizeof(EnterSutdycmd));
	Infrared_UsartGet(&temp, 1, 30);
	if(temp==0)
	{
		ret = 1;
//			printf("0\r\n");
	}
	else
	{
		ret =0;
//		printf("1\r\n");
	}
	InfraredUsartClrBuf();
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

#if 0
uint8 ParseInfrared(infrared_data_t*  ptr)
{
	uint16 i = 0;
	uint16 count;
	uint8 timecount = 0;
	uint8 buff[INFRARED_LEN];

	printf("waiting\r\n");
	if(GetUartBuffSize())
	{
//		printf("count > n\r\n");
		while(timecount<10)
		{
			count = GetUartBuffSize();
			if(count)
			{
				timecount = 0;
				Infrared_UsartGet(buff[i], count, 10);
//				U1_sendS((uint8*)&buff[buff[1]+1+2], 1);
//				U1_sendS((uint8*)&buff[0], 1);

//				U1_sendS((uint8*)buff, count);
				if((buff[0]==0xFA)&&(buff[buff[1]+1+2]==0xED))
				{
					//U1_sendS((uint8*)buff, count);
					ptr->len = buff[1] + 4;
					memcpy(ptr->buff, buff, ptr->len);
				//	U1_sendS((uint8*)ptr->buff, ptr->len);
					return 1;
				}
			}
			else
				timecount++;
			BSP_mDelay(1);		
		}
	}
	return 0;		
}
#endif


uint8 ParseInfrared(infrared_data_t*  ptr)
{
	uint16 i = 0;
	uint16 count;
	uint8 timecount = 0;
	uint8 buff[INFRARED_LEN];

	if(GetUartBuffSize())
	{
		while(timecount<10)
		{
			count = GetUartBuffSize();
			if(count)
			{
				timecount = 0;
				Infrared_UsartGet(&buff[i], count, 5);
				i += count; 
//				U1_sendS((uint8*)&buff[buff[1]+1+2], 1);
//				U1_sendS((uint8*)&buff[0], 1);
//				U1_sendS((uint8*)buff, count);
				if((i>=6)&&(buff[0]==0xFA)&&(buff[buff[1]+1+2]==0xED))
				{
					//U1_sendS((uint8*)buff, count);
					ptr->len = buff[1] + 4;
					memcpy(ptr->buff, buff, ptr->len);
					memset(buff, 0, i);
//					U1_sendS((uint8*)ptr->buff, ptr->len);
					return 1;
				}
			}
			else
				timecount++;
			BSP_mDelay(1);		
		}
	}
	return 0;		
}








