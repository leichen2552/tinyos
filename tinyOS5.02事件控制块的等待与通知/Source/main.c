#include "tinyOS.h"
#include "ARMCM3.h"


tTask* currentTask;
tTask* nextTask;
tTask* idleTask;

tBitmap taskPrioBitmap;
tList taskTable[TINYOS_PRO_COUNT];

uint8_t schedLockCount;
tList tTaskDelayedList;



tTask * tTaskHighestReady(void)
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	tNode* node = tListFirst(&taskTable[highestPrio]);
	return tNodeParent(node, tTask, linkNode);
}

void tTaskSchedInit(void)
{
	int i;
	
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);
	for(i=0; i<TINYOS_PRO_COUNT; i++)
	{
		tListInit(&taskTable[i]);
	}
}

void tTaskSchedDisable(void)
{
	uint32_t status = tTaskEnterCritical();
	
	if(schedLockCount < 255)
	{
		schedLockCount++;
	}
	
	tTaskExitCritical(status);
}

void tTaskSchedEnable(void)
{
	uint32_t status = tTaskEnterCritical();
	
	if(schedLockCount > 0)
	{
		if(--schedLockCount == 0)
		{
			tTaskSched();
		}
	}
	tTaskExitCritical(status);
}

void tTaskSchedRdy(tTask* task)
{
	tListAddFirst(&(taskTable[task->prio]),&(task->linkNode));
	tBitmapSet(&taskPrioBitmap, task->prio);
}

void tTaskSchedUnRdy(tTask* task)
{ 
	tListRemove(&taskTable[task->prio],&(task->linkNode));
	if(tListCount(&taskTable[task->prio]) == 0)
	{
		tBitmapClear(&taskPrioBitmap, task->prio);
	}
}

void tTaskSchedRemove(tTask* task)
{
	tListRemove(&taskTable[task->prio],&(task->linkNode));
	if(tListCount(&taskTable[task->prio]) == 0)
	{
		tBitmapClear(&taskPrioBitmap, task->prio);
	}
}

void tTaskSched()
{
	tTask* tempTask;
	uint32_t status = tTaskEnterCritical();
	
	if(schedLockCount > 0)
	{
		tTaskExitCritical(status);
		return;
	}
	
	tempTask = tTaskHighestReady();
	if(tempTask != currentTask)
	{
		nextTask = tempTask;
		tTaskSwitch();
	}
	
	tTaskExitCritical(status);
}

void tTaskDelayedInit(void)
{
	tListInit(&tTaskDelayedList);
}

void tTimeTaskWait(tTask* task, uint32_t ticks)
{
	task->delayTicks = ticks;
	tListAddLast(&tTaskDelayedList, &(task->delayNode));
	task->state |= TINYOS_TASK_STATE_DELAYED;
}

void tTimeTaskWakeUp(tTask* task)
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));
	task->state &= ~TINYOS_TASK_STATE_DELAYED;
}
 
void tTimeTaskRemove(tTask* task)//延时队列中移除
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));
}

void tTaskSystemTickHandler()
{
	tNode* node;
	
	//进入临界区，以保护任务在整个任务调度与切换期间，不会因为发生中断导致currentTask和nextTask可能更改
	uint32_t status = tTaskEnterCritical();
	
	//检查所有任务的delayTicks数，如果不为0的话，减1
	for(node = tTaskDelayedList.headNode.nextNode; node != &(tTaskDelayedList.headNode); node = node->nextNode)
	{
		tTask* task = tNodeParent(node, tTask, delayNode);
		
		//减减Ticks数，检查是否有任务的延时完毕，如果完毕移出延时队列并解除延时态，进入就绪态
		if(--task->delayTicks == 0)
		{
			//如果任务还处于等待时间的状态，则将其从事件等待队列中唤醒
			if(task->waitEvent)
			{
				//此时，消息为空，等待结果为超时
				tEventRemoveTask(task, (void*)0, tErrorTimeout);
			}
			
			tTimeTaskWakeUp(task);
			
			tTaskSchedRdy(task);
		}
		
	}
		
	if(--currentTask->slice == 0)
	{
		if(tListCount(&taskTable[currentTask->prio]) > 0)
		{
			tListRemoveFirst(&taskTable[currentTask->prio]);
			tListAddLast(&taskTable[currentTask->prio], &(currentTask->linkNode));
			
			currentTask->slice = TINYOS_SLICE_MAX;
		}
	}
	
	tTaskExitCritical(status);
	
	tTaskSched();
}


tTask tTaskIdle;
tTaskStack idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE];

void idleTaskEntry(void *param)
{
	for(;;){}
}

int main()
{
	tTaskSchedInit();
	tTaskDelayedInit();
	
	tInitApp();
	
	tTaskInit(&tTaskIdle, idleTaskEntry, (void *)0, TINYOS_PRO_COUNT -1,&idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE]);
	idleTask = &tTaskIdle;
	nextTask = tTaskHighestReady();
	
	
	tTaskRunFirst();
	
	return 0;	
}
