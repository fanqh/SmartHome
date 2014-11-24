#ifndef _BOOTAPI_H_
#define _BOOTAPI_H_

#include "aeplatform.h"



/* 0
���� 1 int_id: �жϱ�� 0~87 ��BOOT�����¶�����ж�ID�ţ���ϵͳ���ȼ�һ��
���� 2 isr: ��Ӧ�жϵĵ������
ʾ��:BSP_IntVectSet(INTID_USART3,Boot_UsartHdr) ����USART3���ж����Ϊ���� Boot_UsartHdr
����: ��
*/
void    BSP_IntVectSet(uint16 int_id, void *isr);

void    Boot_SysReset(void);

void    Boot_ResetUsart(uint32 newbaud);

void    Boot_UsartSend(unsigned char *outptr,unsigned int len);

//��ȡ��������,�ײ�ʹ��ѭ��������ʵ��.len��ȡ���ݵĳ��� timeout ��ʱʱ�����
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

 //��ȡ���ڽ������ݱ�־λ.���ڻ�������Ч���ݳ��� 0������
uint8   get_usart_interrupt_flg(void); 


#endif


