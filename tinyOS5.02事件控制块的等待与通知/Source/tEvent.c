#include "tinyOS.h"


void tEventInit(tEvent* event, tEventType type)
{
	event->type = tEventTypeUnknow;
	tListInit(&event->waitList);
}

void tEventWait(tEvent* event, tTask* task, void* msg, uint32_t state, uint32_t timeout)
{
	//�����ٽ���
	uint32_t status = tTaskEnterCritical();
	
	task->state |= state;						//���������ĳ���¼��ĵȴ�״̬
	task->waitEvent = event;					//���õȴ������¼��Ľṹ
	task->eventMsg = msg;						//��������ȴ�ʱ�����Ϣ�洢λ�ã���Ϊ�е�ʱ����н�����Ϣ ��������Ҫ������
 	task->waitEventResult = tErrorNoError;		//����¼��ȴ����
	
	tTaskSchedUnRdy(task);//�������ʱ��������Ƴ�
	
	tListAddLast(&event->waitList, &task->linkNode);//�������������¼����ƿ�ĵȴ�������
	
	//����з������ó�ʱ����ͬʱ���뵽��ʱ������
	//��ʱ�䵽��ʱ������ʱ������Ƹ����������ʱ�����Ƴ���ͬʱ��ʱ���б��Ƴ�
	if(timeout)
	{
		tTimeTaskWait(task, timeout);
	}
	
	//�˳��ٽ���
	tTaskExitCritical(status);
}

tTask* tEventWakeUp(tEvent* event, void* msg, uint32_t result)
{
	tNode* node;
	tTask* task = (tTask*)0;
	
	//�����ٽ���
	uint32_t status = tTaskEnterCritical();
	
	//����һ��������¼����ƿ��л��ѣ�����ʵ�ֵĹ���ܼ򵥣��Ӷ����е�ͷ��ȥ���ѣ��ȵȴ��������Ȼ��ѣ���ȴ����������
	if((node = tListRemoveFirst(&event->waitList)) != (tNode*)0)
	{
		//����һ����ز��������������յ�����Ϣ���ṹ�������Ӧ�ĵȴ���־λ
		task = (tTask*)tNodeParent(node, tTask, linkNode);
		task->waitEvent = (tEvent*)0;
		task->waitEventResult = result;
		task->state &= TINYOS_TASK_WAIT_MASK;
		
		//���������˳�ʱ�ȴ����������£��������ʱ�������Ƴ�
		if(task->delayTicks != 0)
		{
			tTimeTaskWakeUp(task);
		}
		
		//����������������
		tTaskSchedRdy(task);
	}
	
	
	tTaskExitCritical(status);
	
	return task;
}

//ǿ�ư�������¼����ƿ���ɾ��
void tEventRemoveTask(tTask* task, void* msg, uint32_t result)
{
	uint32_t status = tTaskEnterCritical();
	
	//����������ڵĵȴ��������Ƴ�
	//ע�⣬����û�м��waitevent�Ƿ�Ϊ�գ���Ȼ�Ǵ��¼����Ƴ�����ô����Ϊ������Ϊ��
	tListRemove(&task->waitEvent->waitList, &task->linkNode);
	
	//������صĲ����������յ�����Ϣ���ṹ�������Ӧ�ı�־λ��
	task->waitEvent = (tEvent*)0;
	task->eventMsg = msg;
	task->waitEventResult = result;
	task->state &= TINYOS_TASK_WAIT_MASK;
	
	tTaskExitCritical(status);
}

