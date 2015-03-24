
#ifndef __COMMAND_
#define __COMMAND_
#include"Include.h"

const uint8 SwVesion[4] = "V1.0";
const uint8 ResSucess[6] = "RE:0<<";
const uint8 ResFail[6]  =  "RE:1<<";
const uint8 tail[2]  = "<<" ;

const uint8 InfraredSendCMD[3]  = "FH:";	//ºìÍâ·¢ÉäÃüÁî
const uint8 InfraredStudyCMD[2]  = "LH";
const uint8 InfraredRecCMD[3]  = "BH:";

const uint8 RF315SendCMD[3] = "FW:";
const uint8 RF315StudyCMD[2] = "LW";
const uint8 RF315RecCMD[3] = "BW:";

const uint8 RF433SendCMD[3] = "FF:";
const uint8 RF433StudyCMD[2] = "LF";
const uint8 RF433RecCMD[3] = "BF:";


const uint8 RF24GSendCMD[2] = "FT";
const uint8 RF24GStudyCMD[2] = "LT";
const uint8 RF24GRecCMD[3] = "BT:";

const uint8 HeartCMD[2] = "FT";
const uint8 SampleTemCMD[2] = "DT";
const uint8 CheckWifiCMD[2] = "DS";
const uint8 LEDONCMD2[] = "LB";
const uint8 LEDOFFCMD[2] = "LD";
const uint8 MACSendCMD[2] = "DM";
const uint8 BondCMD[2] = "BD";
const uint8 Upgrade[2] = "SX";
const uint8 RF433_CMD[3] = "BN:";

const uint8 HeadCMD[6] = "TK:0<<";

//ºìÍâÃüÁî
uint8 InfraredStudy[2] = {0xFA,0XF5}; 





#endif

  