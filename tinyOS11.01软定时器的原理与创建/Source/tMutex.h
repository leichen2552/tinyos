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

typedef struct _tMutexInfo
{
	uint32_t taskCount;			//等待的任务数量
	uint32_t ownerPrio;			//拥有者的优先级
	uint32_t inheritedPrio;		//继承者的优先级
	tTask* owner;				//信号量的所有者
	uint32_t lockedCount; 		//锁定次数
}tMutexInfo;

void tMutexInit(tMutex* mutex);
uint32_t tMutexWait (tMutex * mutex, uint32_t waitTicks);
uint32_t tMutexNoWaitGet (tMutex * mutex);
uint32_t tMutexNotify (tMutex * mutex);
uint32_t tMutexDestroy(tMutex* mutex);
void tMutexGetInfo(tMutex* mutex, tMutexInfo* info);



#endif
