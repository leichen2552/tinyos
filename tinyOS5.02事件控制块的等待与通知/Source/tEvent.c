#include "tinyOS.h"


void tEventInit(tEvent* event, tEventType type)
{
	event->type = tEventTypeUnknow;
	tListInit(&event->waitList);
}

void tEventWait(tEvent* event, tTask* task, void* msg, uint32_t state, uint32_t timeout)
{
	//进入临界区
	uint32_t status = tTaskEnterCritical();
	
	task->state |= state;						//标记任务处于某种事件的等待状态
	task->waitEvent = event;					//设置等待任务事件的结构
	task->eventMsg = msg;						//设置任务等待时间的消息存储位置，因为有的时候会有接受消息 ，所以需要接受区
 	task->waitEventResult = tErrorNoError;		//清空事件等待结果
	
	tTaskSchedUnRdy(task);//将任务从时间队列中移除
	
	tListAddLast(&event->waitList, &task->linkNode);//把这个任务插入事件控制块的等待队列中
	
	//如果有发现设置超时，在同时插入到延时队列中
	//当时间到达时，由延时处理机制负责将任务从延时队列移除，同时从时件列表移除
	if(timeout)
	{
		tTimeTaskWait(task, timeout);
	}
	
	//退出临界区
	tTaskExitCritical(status);
}

tTask* tEventWakeUp(tEvent* event, void* msg, uint32_t result)
{
	tNode* node;
	tTask* task = (tTask*)0;
	
	//进入临界区
	uint32_t status = tTaskEnterCritical();
	
	//按照一定规则从事件控制块中唤醒，这里实现的规则很简单，从队列中的头部去唤醒，先等待的任务先唤醒，后等待的任务后唤醒
	if((node = tListRemoveFirst(&event->waitList)) != (tNode*)0)
	{
		//设置一下相关参数，比如设置收到的信息、结构、清除相应的等待标志位
		task = (tTask*)tNodeParent(node, tTask, linkNode);
		task->waitEvent = (tEvent*)0;
		task->waitEventResult = result;
		task->state &= TINYOS_TASK_WAIT_MASK;
		
		//任务申请了超时等待，这里检查下，将其从延时队列中移除
		if(task->delayTicks != 0)
		{
			tTimeTaskWakeUp(task);
		}
		
		//将任务加入就绪队列
		tTaskSchedRdy(task);
	}
	
	
	tTaskExitCritical(status);
	
	return task;
}

//强制把任务从事件控制块中删除
void tEventRemoveTask(tTask* task, void* msg, uint32_t result)
{
	uint32_t status = tTaskEnterCritical();
	
	//将任务从所在的等待队列中移除
	//注意，这里没有检查waitevent是否为空，既然是从事件中移除，那么就认为不可能为空
	tListRemove(&task->waitEvent->waitList, &task->linkNode);
	
	//设置相关的参数，设置收到的消息，结构，清除相应的标志位。
	task->waitEvent = (tEvent*)0;
	task->eventMsg = msg;
	task->waitEventResult = result;
	task->state &= TINYOS_TASK_WAIT_MASK;
	
	tTaskExitCritical(status);
}

