//#include"Include.h"

#ifndef __COMMAND_
#define __COMMAND_

const uint8 ResSucess[] = "RE:0<<";
const uint8 ResFail[]  =  "RE:1<<";
const uint8 tail[]  = "<<" ;

const uint8 InfraredSendCMD[]  = "FH";	//ºìÍâ·¢ÉäÃüÁî
const uint8 InfraredStudyCMD[]  = "LH";
const uint8 InfraredRecCMD[]  = "BH";

const uint8 RF315SendCMD[] = "FW";
const uint8 RF315StudyCMD[] = "LW";
const uint8 RF315RecCMD[] = "BW";

const uint8 RF433SendCMD[] = "FF";
const uint8 RF433StudyCMD[] = "LF";
const uint8 RF433RecCMD[] = "BF";


const uint8 RF24GSendCMD[] = "FT";
const uint8 RF24GStudyCMD[] = "LT";
const uint8 RF24GRecCMD[] = "BT";

const uint8 HeartCMD[] = "FT";
const uint8 SampleTemCMD = "DT";
const uint8 CheckWifiCMD[] = "DS";
const uint8 LEDONCMD[] = "LB";
const uint8 LEDOFFCMD[] = "LD";
const uint8 MACSendCMD[] = "DM";
const uint8 BondCMD = "BD";
const uint8 Upgrade = "SX";





#endif

  