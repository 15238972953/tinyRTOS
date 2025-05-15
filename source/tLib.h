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

void tNodeInit (tNode * node);//节点初始化
void tListInit (tList * list);//链表初始化
tNode * tListFirst (tList * list);//返回链表第一个结点
tNode *tListFast (tList * list);//返回链表最后一个节点
tNode * tListPre (tList * list, tNode * node);//返回指定节点的前一个节点
tNode * tListNext (tList * list, tNode * node);//返回指定节点的下一个节点
void tListRemoveAll (tList * list);//清空链表中所有节点
void tListAddFirst (tList * list, tNode * node);//从链表头部插入节点
void tListAddLast (tList * list, tNode * node);//从链表尾部插入节点
tNode * tListRemoveFirst (tList * list);//从链表头部删除节点
void tListInsetAfter (tList * list, tNode * nodeAfter, tNode * nodeToInset);//在指定节点后插入节点
void tListRemove (tList * list, tNode * node);//删除指定节点

#endif