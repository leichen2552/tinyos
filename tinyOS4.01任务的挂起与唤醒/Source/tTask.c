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
	
	
 	tNodeInit(&(task->delayNode));//延时队列节点
	tNodeInit(&(task->linkNode));//连接节点
	tTaskSchedRdy(task);
}

void tTaskSuspend(tTask* task)
{
	uint32_t status = tTaskEnterCritical();
	
	//不允许对已经对进入延时的状态的任务挂起
	if(!(task->state & TINYOS_TASK_STATE_DELAYED))
	{
		//增加挂起次数，仅当该任务被执行第一次挂起操作时，
		//才考虑是否要执行任务切换操作
		if(++task->suspendCount <=1)
		{
			//设置挂起标志
			task->state |= TINYOS_TASK_STATE_SUSPEND;//suspend 悬挂、挂起 的意思
			//挂起方式很简单，就是将其从就绪队列中移除，这样调度器就不会发现他
			//也不会切换到该任务运行
			tTaskSchedUnRdy(task);
			
			//当然这个任务也有可能是自己，那么就切换到其他任务
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
	
	//检查任务是否处于挂起状态
	if(task->state & TINYOS_TASK_STATE_SUSPEND)
	{
		//递减挂起次数，如果为0，则清除挂起标志
		if(--task->suspendCount == 0)
		{
			//清除挂起标志
			task->state &= ~TINYOS_TASK_STATE_SUSPEND;
			
			//同时将任务放回就绪队列中
			tTaskSchedRdy(task);
			
			//唤醒过程中，可能会有更高优先级的任务就绪，执行一次任务调度
			tTaskSched();
		}
	}
	
	tTaskExitCritical(status);
}


