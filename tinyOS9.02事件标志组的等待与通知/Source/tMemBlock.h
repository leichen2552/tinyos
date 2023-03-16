#ifndef __TMEMBLOCK_H_
#define __TMEMBLOCK_H_

#include "tEvent.h"

typedef struct _tMemBlock
{
	tEvent event;
	void* memStart;
	uint32_t blockSize;
	uint32_t maxCount;
	tList blockList;
}tMemBlock;

typedef struct _tMemBlockInfo
{
	uint32_t count;
	uint32_t mamCount;
	uint32_t blockSize;
	uint32_t taskCount;//等待存储块的任务数量
}tMemBlockInfo;



void tMemBlockInit(tMemBlock* memBlock, uint8_t* memStart, uint32_t blockSize, uint32_t blockCnt); 
uint32_t tMemBlockWait(tMemBlock* memBlock, uint8_t **mem, uint32_t waitTicks);
uint32_t tMemBlockNoWaitGet(tMemBlock* memBlock, void** mem);
void tMemBlockNotify(tMemBlock* memBlock, uint8_t* mem);
void tMemBlockGetInfo(tMemBlock* memBlock, tMemBlockInfo* info);
uint32_t tMemBlockDestroy(tMemBlock* memBlock);

#endif
