#ifndef __THOOKS_H_
#define __THOOKS_H_

#include "tinyOS.h"


void tHookCpuIdle(void);
void tHookSysTick(void);
void tHookTaskSwitch(tTask* from, tTask* to);
void tHookTaskInit(tTask* task);


#endif

