#ifndef __TINYOS_H_
#define __TINYOS_H_

#include <stdint.h>

#include "tLib.h"

typedef uint32_t tTaskStack;

typedef struct _tTask
{
	tTaskStack * stack;
	uint32_t delayTicks;
}tTask;

extern tTask* currentTask;
extern tTask* nextTask;

uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t status);

void tTaskRunFirst(void);
void tTaskSwitch(void);

void tTaskSchedInit(void);
void tTaskSchedDisable(void);
void tTaskSchedEnable(void);
void tTaskSched(void);



#endif


