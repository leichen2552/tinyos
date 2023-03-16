#include "tinyOS.h"

tTask tTask1;
tTask tTask2;
tTask tTask3;
tTask tTask4;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
tTaskStack task3Env[1024];
tTaskStack task4Env[1024];

//定义事件控制块
tEvent eventWaitTimeout;
tEvent eventWaitNormal;

int task1Flag;
void task1Entry(void * param)
{	
	tSetSysTickPeriod(10);
	
	tEventInit(&eventWaitTimeout,tEventTypeUnknow);//对超时控制块进行初始化
	
	for(;;)
	{
		tEventWait(&eventWaitTimeout, currentTask, (void* )0, 0, 5);//设置超时时间5个Ticks
		tTaskSched();
		
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
		tEventWait(&eventWaitNormal, currentTask, (void*)0,0,0);
		tTaskSched();
		
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);	
	}
}

int task3Flag;
void task3Entry(void * param)
{	
	tEventInit(&eventWaitNormal,tEventTypeUnknow);//对另一个时间控制块初始化
    for(;;)
	{	
		tEventWait(&eventWaitNormal, currentTask, (void*)0,0,0);
		tTaskSched();
		
		task3Flag = 0;
		tTaskDelay(1);
		task3Flag = 1;
		tTaskDelay(1);
	
	}
}

int task4Flag;
void task4Entry(void * param)
{
    for(;;)
	{	
		tTask* rdyTask = tEventWakeUp(&eventWaitNormal, (void*)0, 0); 
		tTaskSched();
		
		task4Flag = 0;
		tTaskDelay(1);
		task4Flag = 1;
		tTaskDelay(1);
	}
}



void tInitApp()
{
	tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Env[1024]);
	tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);
	tTaskInit(&tTask3, task3Entry, (void *)0x22222222, 0, &task3Env[1024]);
	tTaskInit(&tTask4, task4Entry, (void *)0x44444444, 1, &task4Env[1024]);
}



