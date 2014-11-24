#ifndef _APP_DEFINE_H_
#define _APP_DEFINE_H_


#define STM32F1_FLASH_BASE		    0x08000000UL
#define STM32F103C8				    0x10000UL	//E 系列 64K
#define STM32F103CB				    0x20000UL	//E 系列 128K
#define STM32F103CC				    0x40000UL	//E 系列 256K

/*----------------------------------------------------------------------
人为定义
扇区        区域说明        f1xxx flash页段     起始地址
sector 1    bootloader      page0-15            0x08000000UL
sector 2    para1           page16-page23       0x08004000UL
sector 3    para2           page24-page31       0x08006000UL
sector 4    m3_app          page32-page47       0x08008000UL
sector 5    backup          page48-page63       0x0800c000UL
----------------------------------------------------------------------*/
#define FLASH_LEN				    STM32F103C8
#define BOOTLOADER_START		    STM32F1_FLASH_BASE
#define BOOTLOADER_LEN			    0x4000UL		//bootloader长度 16K 

#define TA_PARAM1_START	            (BOOTLOADER_START+BOOTLOADER_LEN)
#define TA_PARAM1_LEN		        0x2000UL		//参数区 8K 
#define TA_PARAM2_START	            (TA_PARAM1_START+TA_PARAM1_LEN)
#define TA_PARAM2_LEN		        0x2000UL		//参数区 8K 


#define APP_PROG_START			    (TA_PARAM2_START+TA_PARAM2_LEN)
#define APP_PROG_LEN			    0x4000UL		//应用层占空间 16k

#define BACK_UP_START               (APP_PROG_START+APP_PROG_LEN)
#define BACK_UP_LEN                 (0x4000UL)      //备份falsh 16k

#define TA_SUCCESS                  0
#define TA_FAIL                     1


#define TA_PRO_FORMAT_ERR			(0x10)		//程序格式错误
#define TA_CHECK_ERROR				(0x11)		//数据校验出错
#define TA_LEN_OVERFLOW				(0x12)		//长度超长
#define TA_ADDR_FORMAT_ERR			(0x13)		//地址格式错误
#define TA_PKGR_LEN_ERR				(0x14)		//数据包长度不对
#define TA_FLASH_ERR				(0x15)		//flash编程出错
#define  FINISHED_UPG			    (0x80)


#define  PROGRAM_LEN_SIGN			(4)

#define  PKG_LEN					(0x78+1)	//120+1
#define  REAL_PRO_PKG_LEN			(0x78)	    //120


//buad id
#define TA_BAUD_9600    			1
#define TA_BAUD_19200				2                          
#define TA_BAUD_38400				3                          
#define TA_BAUD_57600				4                          
#define TA_BAUD_115200				5  



#define  BSP_INT_ID_WWDG                                   0    /* Window WatchDog Interrupt                            */
#define  BSP_INT_ID_PVD                                    1    /* PVD through EXTI Line detection Interrupt            */
#define  BSP_INT_ID_TAMPER                                 2    /* Tamper Interrupt                                     */
#define  BSP_INT_ID_RTC                                    3    /* RTC global Interrupt                                 */
#define  BSP_INT_ID_FLASH                                  4    /* FLASH global Interrupt                               */
#define  BSP_INT_ID_RCC                                    5    /* RCC global Interrupt                                 */
#define  BSP_INT_ID_EXTI0                                  6    /* EXTI Line0 Interrupt                                 */
#define  BSP_INT_ID_EXTI1                                  7    /* EXTI Line1 Interrupt                                 */
#define  BSP_INT_ID_EXTI2                                  8    /* EXTI Line2 Interrupt                                 */
#define  BSP_INT_ID_EXTI3                                  9    /* EXTI Line3 Interrupt                                 */
#define  BSP_INT_ID_EXTI4                                 10    /* EXTI Line4 Interrupt                                 */
#define  BSP_INT_ID_DMA1_CH1                              11    /* DMA1 Channel 1 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH2                              12    /* DMA1 Channel 2 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH3                              13    /* DMA1 Channel 3 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH4                              14    /* DMA1 Channel 4 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH5                              15    /* DMA1 Channel 5 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH6                              16    /* DMA1 Channel 6 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH7                              17    /* DMA1 Channel 7 global Interrupt                      */
#define  BSP_INT_ID_ADC1_2                                18    /* ADC1 et ADC2 global Interrupt                        */
#define  BSP_INT_ID_USB_HP_CAN_TX                         19    /* USB High Priority or CAN TX Interrupts               */
#define  BSP_INT_ID_USB_LP_CAN_RX0                        20    /* USB Low Priority or CAN RX0 Interrupts               */
#define  BSP_INT_ID_CAN_RX1                               21    /* CAN RX1 Interrupt                                    */
#define  BSP_INT_ID_CAN_SCE                               22    /* CAN SCE Interrupt                                    */
#define  BSP_INT_ID_EXTI9_5                               23    /* External Line[9:5] Interrupts                        */
#define  BSP_INT_ID_TIM1_BRK                              24    /* TIM1 Break Interrupt                                 */
#define  BSP_INT_ID_TIM1_UP                               25    /* TIM1 Update Interrupt                                */
#define  BSP_INT_ID_TIM1_TRG_COM                          26    /* TIM1 Trigger and Commutation Interrupt               */
#define  BSP_INT_ID_TIM1_CC                               27    /* TIM1 Capture Compare Interrupt                       */
#define  BSP_INT_ID_TIM2                                  28    /* TIM2 global Interrupt                                */
#define  BSP_INT_ID_TIM3                                  29    /* TIM3 global Interrupt                                */
#define  BSP_INT_ID_TIM4                                  30    /* TIM4 global Interrupt                                */
#define  BSP_INT_ID_I2C1_EV                               31    /* I2C1 Event Interrupt                                 */
#define  BSP_INT_ID_I2C1_ER                               32    /* I2C1 Error Interrupt                                 */
#define  BSP_INT_ID_I2C2_EV                               33    /* I2C2 Event Interrupt                                 */
#define  BSP_INT_ID_I2C2_ER                               34    /* I2C2 Error Interrupt                                 */
#define  BSP_INT_ID_SPI1                                  35    /* SPI1 global Interrupt                                */
#define  BSP_INT_ID_SPI2                                  36    /* SPI2 global Interrupt                                */
#define  BSP_INT_ID_USART1                                37    /* USART1 global Interrupt                              */
#define  BSP_INT_ID_USART2                                38    /* USART2 global Interrupt                              */
#define  BSP_INT_ID_USART3                                39    /* USART3 global Interrupt                              */
#define  BSP_INT_ID_EXTI15_10                             40    /* External Line[15:10] Interrupts                      */
#define  BSP_INT_ID_RTCAlarm                              41    /* RTC Alarm through EXTI Line Interrupt                */
#define  BSP_INT_ID_USBWakeUp                             42    /* USB WakeUp from suspend through EXTI Line Interrupt  */
#define  BSP_INT_ID_TIM8_BRK                              43    /* TIM8 Break Interrupt                                 */
#define  BSP_INT_ID_TIM8_UP                               44    /* TIM8 Update Interrupt                                */
#define  BSP_INT_ID_TIM8_TRG_COM                          45    /* TIM8 Trigger and Commutation Interrupt               */
#define  BSP_INT_ID_TIM8_CC                               46    /* TIM8 Capture Compare Interrupt                       */
#define  BSP_INT_ID_ADC3                                  47    /* ADC3 global Interrupt                                */
#define  BSP_INT_ID_FSMC                                  48    /* FSMC global Interrupt                                */
#define  BSP_INT_ID_SDIO                                  49    /* SDIO global Interrupt                                */
#define  BSP_INT_ID_TIM5                                  50    /* TIM5 global Interrupt                                */
#define  BSP_INT_ID_SPI3                                  51    /* SPI3 global Interrupt                                */
#define  BSP_INT_ID_UART4                                 52    /* UART4 global Interrupt                               */
#define  BSP_INT_ID_UART5                                 53    /* UART5 global Interrupt                               */
#define  BSP_INT_ID_TIM6                                  54    /* TIM6 global Interrupt                                */
#define  BSP_INT_ID_TIM7                                  55    /* TIM7 global Interrupt                                */
#define  BSP_INT_ID_DMA2_CH1                              56    /* DMA2 Channel 1 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH2                              57    /* DMA2 Channel 2 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH3                              58    /* DMA2 Channel 3 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH4_5                            59    /* DMA2 Channel 4 and DMA2 Channel 5 global Interrupt   */


#endif
