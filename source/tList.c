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

//���ؽڵ��ǰһ���ڵ�
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

//���ؽڵ�ĺ�һ���ڵ�
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

//ɾ������������Ԫ��
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

//��ͷ������ڵ�
void tListAddFirst(tList* list, tNode* node)
{
	node->preNode = list->firstNode->preNode;
	node->nextNode = list->firstNode;
	list->firstNode->preNode = node;
	list->firstNode = node;
	list->nodeCount++;
}

//��β������ڵ�
void tListAddLast(tList* list, tNode* node)
{
	node->nextNode = &(list->headNode);
	node->preNode = list->lastNode;
	
	list->lastNode->nextNode = node;
	list->lastNode = node;
	list->nodeCount++;
}

//������ͷ��ɾ���ڵ�
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

//��ָ���ڵ�����ڵ�
void tListInsetAfter (tList * list, tNode * nodeAfter, tNode * nodeToInset)
{//nodeAfter��������Ľڵ�   nodeToInset��������Ľڵ�
	nodeToInset ->preNode = nodeAfter;
	nodeToInset->nextNode = nodeAfter->nextNode;
	
	nodeAfter->nextNode->preNode = nodeToInset;
	nodeAfter->nextNode = nodeToInset;
	
	list->nodeCount++;
}

//ɾ��ָ���ڵ�
void tListRemove (tList * list, tNode * node)
{
	node->preNode->nextNode = node->nextNode;
	node->nextNode->preNode = node->preNode;
	list->nodeCount--; 
}
