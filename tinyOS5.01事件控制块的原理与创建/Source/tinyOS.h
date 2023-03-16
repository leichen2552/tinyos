#ifndef __TINYOS_H_
#define __TINYOS_H_

#include <stdint.h>

#include "tLib.h"
#include "tConfig.h"
#include "tEvent.h"

typedef enum _tError
{
	tErrorNoError = 0,
}tError;


#define TINYOS_TASK_STATE_RDY			0
#define TINYOS_TASK_STATE_DESTORYED		(1<<1)
#define TINYOS_TASK_STATE_DELAYED		(1<<2)
#define TINYOS_TASK_STATE_SUSPEND		(1<<3)

typedef uint32_t tTaskStack;

typedef struct _tTask
{
	// �������ö�ջ�ĵ�ǰ��ջָ�롣ÿ�����������Լ��Ķ�ջ�����������й����д洢��ʱ������һЩ��������
	// ��tinyOS���и�����ǰ�����stackָ���λ�ô������ȡ��ջ�еĻ��������ָ���CPU�Ĵ����У�Ȼ��ʼ����
	// ���л�����������ʱ���Ὣ��ǰCPU�Ĵ���ֵ���浽��ջ�У��ȴ���һ�����и�����ʱ�ٻָ���
	// stack��������󱣴滷�������ĵ�ַλ�ã����ں����ָ�	
	tTaskStack * stack;
	tNode linkNode;			// ���ӽ��
	uint32_t delayTicks;	// ������ʱ������
	tNode delayNode;		// ��ʱ��㣺ͨ��delayNode�Ϳ��Խ�tTask���õ���ʱ������
	uint32_t prio;			// ��������ȼ�
	uint32_t state;			// ����ǰ״̬
	uint32_t slice;			// ��ǰʣ���ʱ��Ƭ
	uint32_t suspendCount;	// ������Ĵ���
	
	void(*clean)(void* param);
	void* cleanParam;
	uint8_t requestDeleteFlag;
	
	tEvent* waitEvent;		//�¼���ȴ�����
	void* eventMsg;			
	uint32_t waitEventResult;
}tTask;
 
extern tTask* currentTask;
extern tTask* nextTask;

uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t status);

void tTaskRunFirst(void);
void tTaskSwitch(void);

void tTaskSchedInit(void);
void tTaskSchedDisable(void);
void tTaskSchedEnable(void);
void tTaskSchedRdy(tTask* task);
void tTaskSchedUnRdy(tTask* task);
void tTaskSchedRemove(tTask* task);
void tTaskSched(void);
void tTimeTaskWait(tTask* task, uint32_t ticks);
void tTimeTaskWakeUp(tTask* task);
void tTimeTaskRemove(tTask* task);
void tTaskSystemTickHandler(void);
void tTaskDelay(uint32_t delay);

typedef struct _tTaskInfo {
	uint32_t delayTicks;
	uint32_t prio;
	uint32_t state;
	uint32_t slice;
	uint32_t suspendCount;
}tTaskInfo;


void tTaskInit(tTask* task, void (*entry)(void *), void* param, uint32_t prio, tTaskStack* stack);
void tTaskSuspend(tTask* task);
void tTaskWakeUp(tTask* task);
void tTaskSetCleanCallFunc(tTask* task, void(*clean)(void* param), void *param);
void tTaskForceDelete(tTask* task);
void tTaskRequestDelete(tTask* task);
uint8_t tTaskIsRequestedDeleted(void);
void tTaskDeleteSelf(void);
void tTaskGetInfo(tTask* task, tTaskInfo* info);

void tSetSysTickPeriod(uint32_t ms);
void tInitApp(void);


#endif


