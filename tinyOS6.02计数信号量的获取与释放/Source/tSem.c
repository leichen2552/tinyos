#include "tSem.h"
#include "tinyOS.h"

void tSemInit(tSem* sem, uint32_t startCount, uint32_t maxCount)
{
	tEventInit(&sem->event, tEventTypeSem);
	
	sem->maxCount = maxCount;
	if(maxCount == 0)
	{
		sem->count = startCount;
	}
	else
	{
		sem->count = (startCount > maxCount) ? maxCount : startCount;
	}
}

//信号量的等待
uint32_t tSemWait(tSem* sem, uint32_t waitTicks)
{
	uint32_t status = tTaskEnterCritical();
	
	//首先检查信号量计数是否大于0
	if(sem->count > 0)
	{
		//如果大于0的话，消耗掉一个，然后正常退出
		--sem->count;
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else
	{
		//如果信号量不大于0，就将任务插入事件队列中
		tEventWait(&sem->event, currentTask, (void*)0, tEventTypeSem, waitTicks);
		tTaskExitCritical(status);
		
		//最后再执行一次事件调度，以便于切换到其他任务
		tTaskSched();
		
		//但凡有与等待超时或者计数可用时，执行会返回到这里，然后取出等待结构
		return currentTask->waitEventResult;
	}
}

//信号量的无等待获取
uint32_t tSemNoWaitGet(tSem* sem)
{
	uint32_t status = tTaskEnterCritical();
	
	//首先检查信号量计数是否大于0
	if(sem->count > 0)
	{
		//如果大于0的话，就消耗掉一个，然后正常退出
		--sem->count;
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else
	{
		//否则返回资源不可用
		tTaskExitCritical(status);
		return tErrorResourceUnavailable;
	}
}

//信号量的通知
void tSemNotify(tSem* sem)
{
	uint32_t status = tTaskEnterCritical();
	
	//检查是否有任务等待
	if(tEventWaitCount(&sem->event) > 0)
	{
		//如果有的话，直接唤醒位于队列首部（最先等待）的任务
		tTask* task = tEventWakeUp(&sem->event, (void*)0, tErrorNoError);
		//如果这个任务优先级更高，就执行调度，切换过去
		if(task->prio < currentTask->prio)
		{
			tTaskSched();
		}
	}
	else
	{
		//如果没有任务等待的话，增加计数
		++sem->count;////////////////////////这个地方是有一点小bug，自己以后改
		
		//如果这个计数超过了最大允许的计数，则递减
		if((sem->maxCount != 0) && (sem->count > sem->maxCount))
		{
			sem->count = sem->maxCount;
		}
	}
	
	tTaskExitCritical(status);
}










