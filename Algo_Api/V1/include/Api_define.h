#ifndef _API_DEFINE_H_
#define _API_DEFINE_H_



//œµÕ≥ ±÷”≈‰÷√
/*
#define HIGH_CLK				160
#define SYS_HIGH_CLK			120
#define SYS_LOW_CLK			30
#define CLK_SRC_INTERNAL		0
#define CLK_SRC_EXTERNAL		1 */
#define PLL_120M				120
#define PLL_30M					30
#define PLL_12M					12


#define TCS2_CALIBRATE_DELAY		300
#define TCS2_READ_DELAY			60
// Command Type
#define  CMD_REGISTER_USER				(0x01)
#define  CMD_REGISTER_TWO				(0X02)
#define  CMD_REGISTER_THREE				(0X03)

#define  CMD_DELETE_SPECIFIC_USER			(0x04)
#define  CMD_DELETE_ALL_USER				(0x05)
#define  CMD_RESET_MODULE				(0x06)
#define  CMD_AUTO_SET_LIGHT				(0X07)
#define  CMD_SET_MODULE_SERIAL_NUM	     	(0x08)
#define  CMD_GET_TOTAL_USER_NUM	    		(0x09)
#define  CMD_GET_USER_ROLE				(0x0A)
#define  CMD_VERIFY_ONE_TO_ONE			(0x0B)
#define  CMD_VERIFY_ONE_TO_ALL			(0x0C)
#define  CMD_SEARCH_NOT_USE_ID			(0x0D)
#define  CMD_SET_BAUD					(0x21 )
#define  CMD_GET_FEA						(0x23)
#define  CMD_GET_FINGER_IMAGE			(0x24) 
#define CMD_IDENTIFY_TEMPLATE			(0x25)
#define CMD_GET_MODULE_VERSION			(0x26)
#define CMD_READ_MODULE_SET_DATA		(0x27)
#define CMD_SET_MATCH_LEVEL				(0x28)
#define CMD_READ_LIGHT_LEVEL				(0x29)
#define CMD_READ_SERIAL_NUM				(0x2A)
#define CMD_READ_ALL_USER_INFO			(0x2B)
#define CMD_ENTER_LOW_POWER_MODE		(0x2C)  
#define CMD_SET_FINGER_ADD_MODE			(0x2D)



#define CMD_CHECK_FINGER_EXIST			(0x30)
#define CMD_READ_USER_FEATURE			(0x31)
#define CMD_W_FEA_TO_MODULE_STORE		(0x41)
#define CMD_W_FEA_TO_M_VERIFY_1_TO_1	(0x42)
#define CMD_W_FEA_TO_M_VERIFY_1_TO_N	(0x43)
#define CMD_W_FEA_TO_M_VERIFY_TO_IMG	(0x44)
#define CMD_VERIFY_IMG_AND_W_FEA_N		(0x45)
#define CMD_SET_MODULE_CONFIG_DATA		(0x46)   


#define CMD_PROG_UPGRADE					(0x57)

#define CMD_GET_DEV_ID						(0x58)



#define CMD_Calibration						(0x80)
#define CMD_LOCK							(0x81)
#define CMD_UNLOCK							(0x82)
#define CMD_GET_Authorize					(0x87)

#define CMD_FEED_DOG						(0xC2)


#define CMD_Get_Video						(0xf0)
#define CMD_GET_IMG						(0xf1)
#define CMD_QUIT_GET_IMG					(0xf2)
#endif
