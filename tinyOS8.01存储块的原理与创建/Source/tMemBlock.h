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

void tMemBlockInit(tMemBlock* memBlock, uint8_t* memStart, uint32_t blockSize, uint32_t blockCnt); 


#endif
