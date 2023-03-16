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
	
	
 	tNodeInit(&(task->delayNode));//��ʱ���нڵ�
	tNodeInit(&(task->linkNode));//���ӽڵ�
	tTaskSchedRdy(task);
}

void tTaskSuspend(tTask* task)
{
	uint32_t status = tTaskEnterCritical();
	
	//��������Ѿ��Խ�����ʱ��״̬���������
	if(!(task->state & TINYOS_TASK_STATE_DELAYED))
	{
		//���ӹ������������������ִ�е�һ�ι������ʱ��
		//�ſ����Ƿ�Ҫִ�������л�����
		if(++task->suspendCount <=1)
		{
			//���ù����־
			task->state |= TINYOS_TASK_STATE_SUSPEND;//suspend ���ҡ����� ����˼
			//����ʽ�ܼ򵥣����ǽ���Ӿ����������Ƴ��������������Ͳ��ᷢ����
			//Ҳ�����л�������������
			tTaskSchedUnRdy(task);
			
			//��Ȼ�������Ҳ�п������Լ�����ô���л�����������
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
	
	//��������Ƿ��ڹ���״̬
	if(task->state & TINYOS_TASK_STATE_SUSPEND)
	{
		//�ݼ�������������Ϊ0������������־
		if(--task->suspendCount == 0)
		{
			//��������־
			task->state &= ~TINYOS_TASK_STATE_SUSPEND;
			
			//ͬʱ������Żؾ���������
			tTaskSchedRdy(task);
			
			//���ѹ����У����ܻ��и������ȼ������������ִ��һ���������
			tTaskSched();
		}
	}
	
	tTaskExitCritical(status);
}


