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

typedef struct _tMutexInfo
{
	uint32_t taskCount;			//�ȴ�����������
	uint32_t ownerPrio;			//ӵ���ߵ����ȼ�
	uint32_t inheritedPrio;		//�̳��ߵ����ȼ�
	tTask* owner;				//�ź�����������
	uint32_t lockedCount; 		//��������
}tMutexInfo;

void tMutexInit(tMutex* mutex);
uint32_t tMutexWait (tMutex * mutex, uint32_t waitTicks);
uint32_t tMutexNoWaitGet (tMutex * mutex);
uint32_t tMutexNotify (tMutex * mutex);
uint32_t tMutexDestroy(tMutex* mutex);
void tMutexGetInfo(tMutex* mutex, tMutexInfo* info);



#endif
