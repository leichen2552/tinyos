#ifndef __TMBOX_H_
#define __TMBOX_H_

#include "tEvent.h"

#define tMBOXSendNormal		0x00
#define tMBOXSendFront		0x01

typedef struct _tMbox
{
	tEvent event;//�¼����ƿ�
	uint32_t count;//�����е���Ϣ����
	uint32_t read;//������
	uint32_t write;//д����
	uint32_t maxCount;//�����Ϣ����
	void** msgBuffer;//������
}tMbox;

void tMboxInit(tMbox* mbox, void **msgBuffer, uint32_t maxCount);
uint32_t tMboxWait(tMbox* mbox, void **msg, uint32_t waitTicks);
uint32_t tMboxNoWaitGet(tMbox* mbox, void **msg);
uint32_t tMboxNotify(tMbox* mbox, void* msg, uint32_t notifyOption);
void tMboxFlush(tMbox* mbox);
uint32_t tMboxDestroy(tMbox* mbox);
#endif

