#ifndef __TTASK_H_
#define __TTASK_H_


#define TINYOS_TASK_STATE_RDY			0
#define TINYOS_TASK_STATE_DESTORYED		(1<<1)
#define TINYOS_TASK_STATE_DELAYED		(1<<2)
#define TINYOS_TASK_STATE_SUSPEND		(1<<3)

#define TINYOS_TASK_WAIT_MASK			(0xFF << 16)

struct _tEvent;

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
	
	struct _tEvent* waitEvent;
	void* eventMsg;
	uint32_t waitEventResult;
	
	uint32_t waitFlagsType;//�ȴ���־����
	uint32_t eventFlags;//�¼���־
}tTask;


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







#endif


