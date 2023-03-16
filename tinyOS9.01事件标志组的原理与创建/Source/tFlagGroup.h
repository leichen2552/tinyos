#ifndef __TFLAGGROUP_H_
#define __TFLAGGROUP_H_

#include "tEvent.h"

typedef struct _tFlagGroup
{
	tEvent event;
	uint32_t flag;//事件标志集合，最多可以表示32个位的事件标志
}tFlagGroup;


void tFlagGroupInit(tFlagGroup* flagGroup, uint32_t flags);

#endif
