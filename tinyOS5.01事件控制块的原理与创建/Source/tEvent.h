#ifndef __TEVENT_H_
#define __TEVENT_H_

#include "tLib.h"

//Event类型
typedef enum _tEventType
{
	tEventTypeUnknow,			//未知类型
}tEventType;

//Event控制结构
typedef struct _tEvent
{
	tEventType type;			//Event类型
	tList waitList;				//任务等待列表
}tEvent;



void tEventInit(tEvent* event, tEventType type);





#endif
