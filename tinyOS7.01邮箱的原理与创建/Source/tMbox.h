#ifndef __TMBOX_H_
#define __TMBOX_H_

#include "tEvent.h"

typedef struct _tMbox
{
	tEvent event;//事件控制块
	uint32_t count;//邮箱中的消息数量
	uint32_t read;//读索引
	uint32_t write;//写索引
	uint32_t maxCount;//最大消息数量
	void** msgBuffer;//缓冲区
}tMbox;

void tMboxInit(tMbox* mbox, void **msgBuffer, uint32_t maxCount);

#endif

