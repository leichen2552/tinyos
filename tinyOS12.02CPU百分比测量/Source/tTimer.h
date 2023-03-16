#ifndef __TTIMER_H_
#define __TTIMER_H_
#include "tEvent.h"

typedef enum _tTimerState
{
	tTimerCreated,
	tTimerStarted,
	tTimerRunning,
	tTimerStopped,
	tTimerDestroyed
}tTimerState;

typedef struct _tTimer
{
	//链表节点
	tNode linkNode;
	
	//初次启动延时后的ticks
	uint32_t startDelayTicks;
	
	//周期定时的周期tick数
	uint32_t durationTicks;

	//当前定时递减计数值
	uint32_t delayTicks;

	//定时回调函数
	void (*timerFunc)(void* arg);
	
	void* arg;
	
	//定时器配置
	uint32_t config;

	//定时器状态
	tTimerState state;
}tTimer;

typedef struct _tTimerInfo
{
	uint32_t startDelayTicks;
	uint32_t durationTicks;
	void (*timerFunc) (void * arg);
	void * arg;
	uint32_t config;
	tTimerState state;
}tTimerInfo;

//在中断服务中去处理的定时器
#define TIMER_CONFIG_TYPE_HARD		(1 << 0)

//在任务中去处理的定时器
#define TIMER_CONFIG_TYPE_SOFT		(0 << 0)

void tTimerInit(tTimer* timer, uint32_t delayTicks, uint32_t durationTicks, 
				void (*timerFunc)(void* arg), void* arg, uint32_t config);
void tTimerModuleInit(void);
void tTimerStart(tTimer* timer);
void tTimerStop (tTimer * timer);
void tTimerModuleTickNotify(void);
void tTimerModuleInit(void);
void tTimerDestroy (tTimer * timer);
void tTimerGetInfo (tTimer* timer, tTimerInfo* info);
void tTimerTaskInit(void);
#endif
