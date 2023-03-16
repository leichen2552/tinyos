#ifndef __TMBOX_H_
#define __TMBOX_H_

#include "tEvent.h"

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

#endif

