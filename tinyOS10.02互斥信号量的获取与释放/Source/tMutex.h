#ifndef __TMUTEX_H_
#define __TMUTEX_H_

#include "tEvent.h"

typedef struct _tMutex
{
	tEvent event;
	uint32_t lockedCount;		//锁定计数器
	tTask* owner;				//当前互斥信号量的所有者
	uint32_t ownerOriginalPrio;	//所有者的原始优先级
}tMutex;

void tMutexInit(tMutex* mutex);
uint32_t tMutexWait (tMutex * mutex, uint32_t waitTicks);
uint32_t tMutexNoWaitGet (tMutex * mutex);
uint32_t tMutexNotify (tMutex * mutex);




#endif
