#ifndef __TLIB_H_
#define __TLIB_H_

#include <stdint.h>

typedef struct
{
	uint32_t bitmap;
}tBitmap;

void tBitmapInit(tBitmap* bitmap);
uint32_t tBitmapPosCount(void);
void tBitmapSet(tBitmap* bitmap, uint32_t pos);
void tBitmapClear(tBitmap* bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet(tBitmap* bitmap);



#endif 





