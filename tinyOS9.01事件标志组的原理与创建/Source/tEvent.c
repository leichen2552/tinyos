#include "tinyOS.h"


void tEventInit(tEvent* event, tEventType type)
{
	event->type = tEventTypeUnknow;
	tListInit(&event->waitList);
}

void tEventWait(tEvent* event, tTask* task, void* msg, uint32_t state, uint32_t timeout)
{
	uint32_t status = tTaskEnterCritical();
	
	task->state |= state;
	task->waitEvent = event;
	task->eventMsg = msg;
	task->waitEventResult = tErrorNoError;
	
	tTaskSchedUnRdy(task);
	
	tListAddLast(&event->waitList, &task->linkNode);
	
	if(timeout)
	{
		tTimeTaskWait(task, timeout);
	}
	
	tTaskExitCritical(status);
}

tTask* tEventWakeUp(tEvent* event, void* msg, uint32_t result)
{
	tNode* node;
	tTask* task = (tTask*)0;
	uint32_t status = tTaskEnterCritical();
	
	if((node = tListRemoveFirst(&event->waitList)) != (tNode*)0)
	{
		task = (tTask*)tNodeParent(node, tTask, linkNode);
		task->waitEvent = (tEvent*)0;
        task->eventMsg = msg;
		task->waitEventResult = result;
		task->state &= ~TINYOS_TASK_WAIT_MASK;
		
		if(task->delayTicks != 0)
		{
			tTimeTaskWakeUp(task);
		}
		
		tTaskSchedRdy(task);
	}
	
	
	tTaskExitCritical(status);
	
	return task;
}


void tEventRemoveTask(tTask* task, void* msg, uint32_t result)
{
	uint32_t status = tTaskEnterCritical();
	
	tListRemove(&task->waitEvent->waitList, &task->linkNode);
	task->waitEvent = (tEvent*)0;
	task->eventMsg = msg;
	task->waitEventResult = result;
	task->state &= ~TINYOS_TASK_WAIT_MASK;
	
	tTaskExitCritical(status);
}

uint32_t tEventRemoveAll(tEvent* event, void* msg, uint32_t result)
{
	tNode* node;
	uint32_t count = 0;
	
	//进入临界区
	uint32_t status = tTaskEnterCritical();
	
	//获取等待中的任务数量
	count = tListCount(&event->waitList);
	
	//遍历列表中的所有等待任务
	while((node = tListRemoveFirst(&event->waitList)) != (tNode*)0)
	{
		//转换为相应的任务结构
		tTask* task = (tTask *)tNodeParent(node,tTask,linkNode);
		
		//设置状态位，设置收到的消息、结构、清除相应的等待标志位
		task->waitEvent = (tEvent*)0;
		task->eventMsg = msg;
		task->waitEventResult = result;
		task->state &= ~TINYOS_TASK_WAIT_MASK;
		
		//任务申请了超时等待，这里检查下，将其从任务列表中移除
		if(task->delayTicks != 0)
		{
			tTimeTaskWakeUp(task);
		}
		
		//将任务加入就绪队列
		tTaskSchedRdy(task);
	}
	
	//退出临界区
	tTaskExitCritical(status);
	
	return count;
}

uint32_t tEventWaitCount(tEvent* event)
{
	uint32_t count = 0;
	
	uint32_t status = tTaskEnterCritical();
	
	count = tListCount(&event->waitList);
	
	tTaskExitCritical(status);
	
	return count;
}


