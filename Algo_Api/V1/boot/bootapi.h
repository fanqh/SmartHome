#ifndef _BOOTAPI_H_
#define _BOOTAPI_H_

#include "aeplatform.h"



/* 0
参数 1 int_id: 中断编号 0~87 在BOOT层重新定义的中断ID号，和系统优先级一致
参数 2 isr: 相应中断的调用入口
示例:BSP_IntVectSet(INTID_USART3,Boot_UsartHdr) 设置USART3的中断入口为函数 Boot_UsartHdr
返回: 无
*/
void    BSP_IntVectSet(uint16 int_id, void *isr);

void    Boot_SysReset(void);

void    Boot_ResetUsart(uint32 newbaud);

void    Boot_UsartSend(unsigned char *outptr,unsigned int len);

//获取串口数据,底层使用循环缓冲区实现.len获取数据的长度 timeout 超时时间毫秒
int16   Boot_UsartGet(uint8 *buff,uint32 len,uint32 timeout);

void    Boot_UsartClrBuf(void);

int32   Boot_EraseUsrFlash(uint32 addr);

int32   Boot_ProgramUsrFlash(uint8 * buff, uint32 addr, uint32 len);

void    Boot_UpdateProgram(uint32  Binlen);

void    debug_led_on(void);

void    debug_led_off(void);

void    app_enroll_tick_hdl(void *isr,unsigned char index);

void    Get_ParaFrmBoot(uint16 iLevel,uint16 version,uint8 *addr);

void    boot_get_ver(unsigned char *buf);

void    TA_GetCommand(unsigned char *cmd);

void    Command_Process(void);

 //获取串口接收数据标志位.串口缓冲区有效数据长度 0无数据
uint8   get_usart_interrupt_flg(void); 


#endif


