#define NVIC_INT_CTRL		0xE000Ed04		//中断控制及状态寄存器
#define NVIC_PENDSVSET		0x10000000		//写1可以悬起PendSV
#define NVIC_SYSPRI2		0xE000ED22		//系统优先级寄存器
#define NVIC_PENDSV_PRI		0x000000FF		//配置优先级为最低

#define MEM32(addr)			*(volatile unsigned long *)(addr)
#define MEM8(addr)			*(volatile unsigned char *)(addr)	

void triggerPendSVC (void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

typedef struct _BlockType_t
{
	unsigned long* stackPtr;
}BlockType_t;

BlockType_t* blockPtr;


void delay(int count)
{
	while(--count > 0);
}

int flag;

unsigned long stackBuffer[1024];
BlockType_t block;

int main()
{
	block.stackPtr = &stackBuffer[1024];//满降栈
	blockPtr = &block;
	
	for(;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
		
		triggerPendSVC();
	}		
}
