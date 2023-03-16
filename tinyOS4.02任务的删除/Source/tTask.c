#include "tinyOS.h"


void tTaskInit(tTask* task, void (*entry)(void *), void* param, uint32_t prio, tTaskStack* stack)
{
	*(--stack) = (unsigned long)(1 << 24);
	*(--stack) = (unsigned long)entry;
	*(--stack) = (unsigned long)0x14;
	*(--stack) = (unsigned long)0x12;
	*(--stack) = (unsigned long)0x03;
	*(--stack) = (unsigned long)0x02;
	*(--stack) = (unsigned long)0x01;
	*(--stack) = (unsigned long)param;
	
	*(--stack) = (unsigned long)0x11;
	*(--stack) = (unsigned long)0x10;
	*(--stack) = (unsigned long)0x09;
	*(--stack) = (unsigned long)0x08;
	*(--stack) = (unsigned long)0x07;
	*(--stack) = (unsigned long)0x06;
	*(--stack) = (unsigned long)0x05;
	*(--stack) = (unsigned long)0x04;
	
	task->slice = TINYOS_SLICE_MAX;
	task->stack = stack;
	task->delayTicks = 0;
	task->prio = prio;
	task->state = TINYOS_TASK_STATE_RDY;
	task->suspendCount = 0;
	task->clean = (void (*)(void *))0;
	task->cleanParam = (void *)0;
	task->requestDeleteFlag = 0;
	
	tNodeInit(&(task->delayNode));
	tNodeInit(&(task->linkNode));
	tTaskSchedRdy(task);
}

void tTaskSuspend(tTask* task)
{
	uint32_t status = tTaskEnterCritical();
	
	if(!(task->state & TINYOS_TASK_STATE_DELAYED))
	{
		if(++task->suspendCount <=1)
		{
			task->state |= TINYOS_TASK_STATE_SUSPEND;
			tTaskSchedUnRdy(task);
			if(task == currentTask)
			{
				tTaskSched();
			}
		}
	}
	
	tTaskExitCritical(status);
}

void tTaskWakeUp(tTask* task)
{
	uint32_t status = tTaskEnterCritical();
	
	if(task->state & TINYOS_TASK_STATE_SUSPEND)
	{
		if(--task->suspendCount == 0)
		{
			task->state &= ~TINYOS_TASK_STATE_SUSPEND;
			tTaskSchedRdy(task);
			tTaskSched();
		}
	}
	
	tTaskExitCritical(status);
}

void tTaskSetCleanCallFunc(tTask* task, void(*clean)(void* param), void *param)
{
	task->clean = clean;
	task->cleanParam = param;
}

//强制删除函数
void tTaskForceDelete(tTask* task)
{   
	uint32_t status = tTaskEnterCritical();
	
	//如果任务处于延时状态，则从延时列表中删除
	if(task->state & TINYOS_TASK_STATE_DELAYED)
	{
		tTimeTaskRemove(task);
	}
	//如果任务不处于挂起状态，那么就是就绪态，那就从就绪表中删除
	else if(!(task->state)&TINYOS_TASK_STATE_SUSPEND)
	{
		tTaskSchedRemove(task);
	}
	//删除时，如果没有设置清理函数，则调用清理函数
	if(task->clean)
	{
		task->clean(task->cleanParam);
	}
	//如果删除的是自己，那么需要切换到另一个任务，所以执行一次任务调度
	if(currentTask == task)
	{
		tTaskSched();
	}
	
	tTaskExitCritical(status);
}

void tTaskRequestDelete(tTask* task)
{
	uint32_t status = tTaskEnterCritical();
	
	//设置清除删除标记 
	task->requestDeleteFlag = 1;
	
	tTaskExitCritical(status);
}

uint8_t tTaskIsRequestedDeleted(void)
{
	uint8_t delete;
	
	uint32_t status = tTaskEnterCritical();
	
	//获取请求删除标记
	delete = currentTask->requestDeleteFlag;
	
	tTaskExitCritical(status);
	
	return delete;
}

void tTaskDeleteSelf(void)
{
	uint32_t status = tTaskEnterCritical();
	
	//任务在调用该函数时，必须是处于就绪状态或
	tTaskSchedRemove(currentTask);
	
	//删除时，如果没有设置清理函数，则调用清理函数
	if(currentTask->clean)
	{
		currentTask->clean(currentTask->cleanParam);
	}
	
	//接下来肯定是切换到其他任务运行
	tTaskSched();
	
	tTaskExitCritical(status);
}

