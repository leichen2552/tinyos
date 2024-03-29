#include "tinyOS.h"
#include "ARMCM3.h"


tTask* currentTask;
tTask* nextTask;
tTask* idleTask;

tBitmap taskPrioBitmap;
tTask* taskTable[TINYOS_PRO_COUNT];

uint8_t schedLockCount;
tList tTaskDelayedList;

void tTaskInit(tTask* task, void (*entry)(void *), void* param, uint32_t prio, tTaskStack* stack)
{
	*(--stack) = (unsigned long)(1 << 24);	//xPSR寄存器//必须置为1，否则进入ARM模式，运行异常
	*(--stack) = (unsigned long)entry;		//pc寄存器 //任务的入口函数
	*(--stack) = (unsigned long)0x14;				//未用，填入便于识别的数值，对应寄存器号
	*(--stack) = (unsigned long)0x12;				//未用，填入便于识别的数值，对应寄存器号
	*(--stack) = (unsigned long)0x03;				//未用，填入便于识别的数值，对应寄存器号
	*(--stack) = (unsigned long)0x02;				//未用，填入便于识别的数值，对应寄存器号
	*(--stack) = (unsigned long)0x01;				//未用，填入便于识别的数值，对应寄存器号
	*(--stack) = (unsigned long)param;		//任务的入口参数会传给R0
											//以上是硬件自动保存		
	
	*(--stack) = (unsigned long)0x11;				//未用，填入便于识别的数值，对应寄存器号
	*(--stack) = (unsigned long)0x10;				//未用，填入便于识别的数值，对应寄存器号
	*(--stack) = (unsigned long)0x09;				//未用，填入便于识别的数值，对应寄存器号
	*(--stack) = (unsigned long)0x08;				//未用，填入便于识别的数值，对应寄存器号
	*(--stack) = (unsigned long)0x07;				//未用，填入便于识别的数值，对应寄存器号
	*(--stack) = (unsigned long)0x06;				//未用，填入便于识别的数值，对应寄存器号
	*(--stack) = (unsigned long)0x05;				//未用，填入便于识别的数值，对应寄存器号
	*(--stack) = (unsigned long)0x04;				//未用，填入便于识别的数值，对应寄存器号
	
	task->stack = stack;
	task->delayTicks = 0;
	task->prio = prio;
	task->state = TINYOS_TASK_STATE_RDY;
	
	tNodeInit(&(task->delayNode));
	
	taskTable[prio] = task;
	tBitmapSet(&taskPrioBitmap, prio);
}

tTask * tTaskHighestReady()
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	return taskTable[highestPrio];
}

void tTaskSchedInit(void)
{
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);
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
	taskTable[task->prio] = task;
	tBitmapSet(&taskPrioBitmap, task->prio);
}

void tTaskSchedUnRdy(tTask* task)
{
	taskTable[task->prio] = (tTask*)0;
	tBitmapClear(&taskPrioBitmap, task->prio);
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

void tTaskSystemTickHandler()
{
	tNode* node;
	uint32_t status = tTaskEnterCritical();
	
	for(node = tTaskDelayedList.headNode.nextNode; node != &(tTaskDelayedList.headNode); node = node->nextNode)
	{
		tTask* task = tNodeParent(node, tTask, delayNode);
		if(--task->delayTicks == 0)
		{
			tTimeTaskWakeUp(task);
			
			tTaskSchedRdy(task);
		}
		
	}
		
	tTaskExitCritical(status);
	
	tTaskSched();
}

void tTaskDelay(uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	
	tTimeTaskWait(currentTask, delay);
	tTaskSchedUnRdy(currentTask);
	tTaskExitCritical(status);
	tTaskSched();
}


void tSetSysTickPeriod(uint32_t ms)
{
	SysTick->LOAD  = ms * SystemCoreClock / 1000 - 1; 
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	SysTick->VAL   = 0;                           
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
					 SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk; 
}

void SysTick_Handler () 
{
    tTaskSystemTickHandler();
}

int task1Flag;


void task1Entry(void * param)
{
	tSetSysTickPeriod(10);
	
	for(;;)
	{
		task1Flag = 0;
		tTaskDelay(1);	
		task1Flag = 1;
		tTaskDelay(1);
	}
}

int task2Flag;
void task2Entry(void * param)
{
    for(;;)
	{		
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
	}
}


tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

tTask tTaskIdle;
tTaskStack idleTaskEnv[1024];

void idleTaskEntry(void *param)
{
	for(;;){}
}

int main()
{
	tTaskSchedInit();
	tTaskDelayedInit();
	tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Env[1024]);
	tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);
	
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	
	tTaskInit(&tTaskIdle, idleTaskEntry, (void *)0, TINYOS_PRO_COUNT -1,&idleTaskEnv[1024]);
	idleTask = &tTaskIdle;
	nextTask = tTaskHighestReady();
	
	
	tTaskRunFirst();
	
	return 0;	
}
