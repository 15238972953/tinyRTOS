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

typedef struct _tNode
{
	struct _tNode* preNode;
	struct _tNode* nextNode;
}tNode;

typedef struct _tList
{
	tNode headNode;
	uint32_t nodeCount;
}tList;

#define tNodeParent(node, parent, name) (parent*)((uint32_t)node - (uint32_t)&((parent *)0)->name)

void tNodeInit (tNode * node);//�ڵ��ʼ��
void tListInit (tList * list);//�����ʼ��
tNode * tListFirst (tList * list);//���������һ�����
tNode *tListFast (tList * list);//�����������һ���ڵ�
tNode * tListPre (tList * list, tNode * node);//����ָ���ڵ��ǰһ���ڵ�
tNode * tListNext (tList * list, tNode * node);//����ָ���ڵ����һ���ڵ�
void tListRemoveAll (tList * list);//������������нڵ�
void tListAddFirst (tList * list, tNode * node);//������ͷ������ڵ�
void tListAddLast (tList * list, tNode * node);//������β������ڵ�
tNode * tListRemoveFirst (tList * list);//������ͷ��ɾ���ڵ�
void tListInsetAfter (tList * list, tNode * nodeAfter, tNode * nodeToInset);//��ָ���ڵ�����ڵ�
void tListRemove (tList * list, tNode * node);//ɾ��ָ���ڵ�

#endif