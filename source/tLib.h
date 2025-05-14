#ifndef TLIB_H
#define TLIB_H


#include <stdint.h>

typedef struct
{
	uint32_t bitmap;
}tBitmap;

void tBitmapInit (tBitmap * bitmap);//��ʼ��λͼ
uint32_t tBitmapPosCount (void);//��־λ����
void tBitmapSet (tBitmap * bitmap, uint32_t pos);
void tBitmapClear (tBitmap * bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet (tBitmap * bitmap);//��ȡ��һ�������λͼ



#endif