#ifndef __TINYOS_H_
#define __TINYOS_H_

#include <stdint.h>

#include "tLib.h"
#include "tConfig.h"

#define TINYOS_TASK_STATE_RDY			0
#define TINYOS_TASK_STATE_DELAYED		(1<<1)
#define TINYOS_TASK_STATE_SUSPEND		(1<<2)

typedef uint32_t tTaskStack;

typedef struct _tTask
{
	// 任务所用堆栈的当前堆栈指针。每个任务都有他自己的堆栈，用于在运行过程中存储临时变量等一些环境参数
	// 在tinyOS运行该任务前，会从stack指向的位置处，会读取堆栈中的环境参数恢复到CPU寄存器中，然后开始运行
	// 在切换至其它任务时，会将当前CPU寄存器值保存到堆栈中，等待下一次运行该任务时再恢复。
	// stack保存了最后保存环境参数的地址位置，用于后续恢复	
	tTaskStack * stack;
	tNode linkNode;			// 连接结点
	uint32_t delayTicks;	// 任务延时计数器
	tNode delayNode;		// 延时结点：通过delayNode就可以将tTask放置到延时队列中
	uint32_t prio;			// 任务的优先级
	uint32_t state;			// 任务当前状态
	uint32_t slice;			// 当前剩余的时间片
	uint32_t suspendCount;	// 被挂起的次数
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
void tTaskSchedRdy(tTask* task);
void tTaskSchedUnRdy(tTask* task);
void tTaskSched(void);
void tTimeTaskWait(tTask* task, uint32_t ticks);
void tTimeTaskWakeUp(tTask* task);
void tTaskSystemTickHandler(void);
void tTaskDelay(uint32_t delay);
void tTaskInit(tTask* task, void (*entry)(void *), void* param, uint32_t prio, tTaskStack* stack);
void tSetSysTickPeriod(uint32_t ms);

void tInitApp(void);

void tTaskSuspend(tTask* task);
void tTaskWakeUp(tTask* task);
#endif


