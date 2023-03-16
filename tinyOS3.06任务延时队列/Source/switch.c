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
	uint32_t primask = __get_PRIMASK();//获取PRIMASK的值，
	__disable_irq();//设置关中断的值
	
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
	
	MRS R0, PSP				   //加载特殊功能寄存器到通用功能寄存值
	CBZ R0, PendSVHander_nosave//判断R0的值是否为0，是0就跳转到PendSVHander_nosave段
							   //判断是tTaskRunFirst触发还是tTaskSwitch()触发
	STMDB R0!, {R4-R11}
	
	LDR R1, =currentTask
	LDR R1, [R1]
	STR R0, [R1]
	
	
PendSVHander_nosave
	LDR R0, =currentTask		//加载当前任务到R0
	LDR R1, =nextTask			//加载下个任务到R1
	LDR R2, [R1]				//把nextTask寄存器值给R2
	STR R2, [R0]				//把nextTask值给currentTask
	
	LDR R0, [R2]				//从currentTask取出堆栈地址 
	LDMIA R0!, {R4-R11}			//批量加载R4-R11寄存器
	
	MSR PSP, R0//存储通用寄存器的值到特殊功能寄存器，切换到currentTask的堆栈
	ORR LR, LR, #0x04 //内核复位后默认是特权级栈，该条语句就是从进程堆栈中作出栈操作，返回后使用PSP
	BX LR	    		//退出异常
}




void tTaskRunFirst(void)
{
	//这里设置了一个标记，PSP = 0，用于与tTaskSwitch()区分，用于在PEND_SV
	//中判断当前切换是tinyOS启东市切换至第一个任务，还是多任务已经跑起来后执行的切换
	
	__set_PSP(0);//用于标记初始任务，如果是跑起来的多任务，PSP栈肯定是指向用户级栈的某个位置肯定不为0
	
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
	//可以看到，这个函数没有返回
	//这是因为，一旦触发PEND_SV后，将会在PendSV后立即进行任务切换，切换至第一个任务运行
	//此后，TinyOS将负责管理所有的任务运行，永远不会返回该函数运行
	
}

void tTaskSwitch(void)
{
	//和tTaskRunFirst，这个函数会在某个任务调用，然后触发PendSV切换至其他任务
	//之后的某个时候，将会再次切换到该任务运行，此时，开始运行该行代码，然后到
	//tTaskSwitch调用处继续往下运行
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}


















