#ifndef __TEVENT_H_
#define __TEVENT_H_

#include "tLib.h"

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





#endif
