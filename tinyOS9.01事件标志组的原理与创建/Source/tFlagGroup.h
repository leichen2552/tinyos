#ifndef __TFLAGGROUP_H_
#define __TFLAGGROUP_H_

#include "tEvent.h"

typedef struct _tFlagGroup
{
	tEvent event;
	uint32_t flag;//�¼���־���ϣ������Ա�ʾ32��λ���¼���־
}tFlagGroup;


void tFlagGroupInit(tFlagGroup* flagGroup, uint32_t flags);

#endif
