#ifndef __TEVENT_H_
#define __TEVENT_H_

#include "tLib.h"
#include "tTask.h"
//Event����
typedef enum _tEventType
{
	tEventTypeUnknow,			//δ֪����
}tEventType;

//Event���ƽṹ
typedef struct _tEvent
{
	tEventType type;			//Event����
	tList waitList;				//����ȴ��б�
}tEvent;

void tEventInit(tEvent* event, tEventType type);
void tEventWait(tEvent* event, tTask* task, void* msg, uint32_t state, uint32_t timeout);
tTask* tEventWakeUp(tEvent* event, void* msg, uint32_t result);
void tEventRemoveTask(tTask* task, void* msg, uint32_t result);



#endif
