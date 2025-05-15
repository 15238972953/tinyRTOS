#include "tLib.h"

void tNodeInit(tNode* node)
{
	node->nextNode = node;
	node->preNode = node;
}

#define firstNode headNode.nextNode
#define lastNode headNode.preNode

void tListInit(tList* list)
{
	list->firstNode = &(list->headNode);
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

uint32_t tListCount(tList* list)
{
	return list->nodeCount;
}

tNode* tListFirst(tList* list)
{
	tNode* node = (tNode*)0;
	if(list->nodeCount != 0)
	{
		node = list->firstNode;
	}
	return node;
}

tNode* tListLast(tList* list)
{
	tNode* node = (tNode*)0;
	if(list->nodeCount != 0)
	{
		node = list->lastNode;
	}
	return node;
}

//返回节点的前一个节点
tNode* tListPre(tList* list, tNode* node)
{
	if(node->preNode == node)
	{
		return (tNode*)0;
	}else
	{
		return node->preNode;
	}
}

//返回节点的后一个节点
tNode* tListNext(tList* list, tNode* node)
{
	if(node->nextNode == node)
	{
		return (tNode*)0;
	}else
	{
		return node->nextNode;
	}
}

//删除链表中所有元素
void tListRemoveAll(tList* list)
{
	uint32_t count;
	tNode* nextNode;
	
	nextNode = list->firstNode;
	for(count = list->nodeCount; count != 0; count--)
	{
		tNode* currentNode = nextNode;
		nextNode = nextNode->nextNode;
		
		currentNode->nextNode = currentNode;
		currentNode->preNode = currentNode;
	}
	list->firstNode = &(list->headNode);
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

//从头部插入节点
void tListAddFirst(tList* list, tNode* node)
{
	node->preNode = list->firstNode->preNode;
	node->nextNode = list->firstNode;
	list->firstNode->preNode = node;
	list->firstNode = node;
	list->nodeCount++;
}

//从尾部插入节点
void tListAddLast(tList* list, tNode* node)
{
	node->nextNode = &(list->headNode);
	node->preNode = list->lastNode;
	
	list->lastNode->nextNode = node;
	list->lastNode = node;
	list->nodeCount++;
}

//从链表头部删除节点
tNode * tListRemoveFirst (tList * list)
{
	tNode * node = (tNode *)0;
	
	if(list->nodeCount != 0)
	{
		node = list->firstNode;
		
		node->nextNode->preNode = &(list->headNode);
		list->firstNode = node->nextNode;
		list->nodeCount--;
	}
	return node;
}

//在指定节点后插入节点
void tListInsetAfter (tList * list, tNode * nodeAfter, tNode * nodeToInset)
{//nodeAfter：被插入的节点   nodeToInset：待插入的节点
	nodeToInset ->preNode = nodeAfter;
	nodeToInset->nextNode = nodeAfter->nextNode;
	
	nodeAfter->nextNode->preNode = nodeToInset;
	nodeAfter->nextNode = nodeToInset;
	
	list->nodeCount++;
}

//删除指定节点
void tListRemove (tList * list, tNode * node)
{
	node->preNode->nextNode = node->nextNode;
	node->nextNode->preNode = node->preNode;
	list->nodeCount--; 
}
