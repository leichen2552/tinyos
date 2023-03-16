#include "tinyOS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL		0xE000Ed04
#define NVIC_PENDSVSET		0x10000000
#define NVIC_SYSPRI2		0xE000ED22
#define NVIC_PENDSV_PRI		0x000000FF

#define MEM32(addr)			*(volatile unsigned long *)(addr)
#define MEM8(addr)			*(volatile unsigned char *)(addr)

uint32_t tTaskEnterCritical(void)
{
	uint32_t primask = __get_PRIMASK();//��ȡPRIMASK��ֵ��
	__disable_irq();//���ù��жϵ�ֵ
	
	return primask;
}

void tTaskExitCritical(uint32_t status)
{
	__set_PRIMASK(status);
}

__asm void PendSV_Handler(void)
{
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP				   //�������⹦�ܼĴ�����ͨ�ù��ܼĴ�ֵ
	CBZ R0, PendSVHander_nosave//�ж�R0��ֵ�Ƿ�Ϊ0����0����ת��PendSVHander_nosave��
							   //�ж���tTaskRunFirst��������tTaskSwitch()����
	STMDB R0!, {R4-R11}
	
	LDR R1, =currentTask
	LDR R1, [R1]
	STR R0, [R1]
	
	
PendSVHander_nosave
	LDR R0, =currentTask		//���ص�ǰ����R0
	LDR R1, =nextTask			//�����¸�����R1
	LDR R2, [R1]				//��nextTask�Ĵ���ֵ��R2
	STR R2, [R0]				//��nextTaskֵ��currentTask
	
	LDR R0, [R2]				//��currentTaskȡ����ջ��ַ 
	LDMIA R0!, {R4-R11}			//��������R4-R11�Ĵ���
	
	MSR PSP, R0//�洢ͨ�üĴ�����ֵ�����⹦�ܼĴ������л���currentTask�Ķ�ջ
	ORR LR, LR, #0x04 //�ں˸�λ��Ĭ������Ȩ��ջ�����������Ǵӽ��̶�ջ������ջ���������غ�ʹ��PSP
	BX LR	    		//�˳��쳣
}




void tTaskRunFirst(void)
{
	//����������һ����ǣ�PSP = 0��������tTaskSwitch()���֣�������PEND_SV
	//���жϵ�ǰ�л���tinyOS�������л�����һ�����񣬻��Ƕ������Ѿ���������ִ�е��л�
	
	__set_PSP(0);//���ڱ�ǳ�ʼ����������������Ķ�����PSPջ�϶���ָ���û���ջ��ĳ��λ�ÿ϶���Ϊ0
	
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
	//���Կ������������û�з���
	//������Ϊ��һ������PEND_SV�󣬽�����PendSV���������������л����л�����һ����������
	//�˺�TinyOS������������е��������У���Զ���᷵�ظú�������
	
}

void tTaskSwitch(void)
{
	//��tTaskRunFirst�������������ĳ��������ã�Ȼ�󴥷�PendSV�л�����������
	//֮���ĳ��ʱ�򣬽����ٴ��л������������У���ʱ����ʼ���и��д��룬Ȼ��
	//tTaskSwitch���ô�������������
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}


















