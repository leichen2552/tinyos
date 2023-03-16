#ifndef __TMUTEX_H_
#define __TMUTEX_H_

#include "tEvent.h"

typedef struct _tMutex
{
	tEvent event;
	uint32_t lockedCount;		//����������
	tTask* owner;				//��ǰ�����ź�����������
	uint32_t ownerOriginalPrio;	//�����ߵ�ԭʼ���ȼ�
}tMutex;

void tMutexInit(tMutex* mutex);
uint32_t tMutexWait (tMutex * mutex, uint32_t waitTicks);
uint32_t tMutexNoWaitGet (tMutex * mutex);
uint32_t tMutexNotify (tMutex * mutex);




#endif
