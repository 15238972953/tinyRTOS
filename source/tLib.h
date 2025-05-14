#ifndef TLIB_H
#define TLIB_H


#include <stdint.h>

typedef struct
{
	uint32_t bitmap;
}tBitmap;

void tBitmapInit (tBitmap * bitmap);//初始化位图
uint32_t tBitmapPosCount (void);//标志位数量
void tBitmapSet (tBitmap * bitmap, uint32_t pos);
void tBitmapClear (tBitmap * bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet (tBitmap * bitmap);//获取第一个进入的位图



#endif