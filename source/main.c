/********ͷ�ļ�������********/

#include "tinyOS.h"
#include "ARMCM3.h"


tTask * currentTask;//��ǰ����
tTask * nextTask;//�¸�����
tTask * idleTask;//��������
tBitmap taskPrioBitmap;
tList taskTable[TINYOS_PRO_COUNT];//����ջ

uint8_t schedLockCount;//������
tList tTaskDelayedList;



/********������********/

//��ȡ�����б������ȼ���ߵ�����
tTask* tTaskHighestReady(void)
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	tNode* node = tListFirst(&taskTable[highestPrio]);
	return tNodeParent(node, tTask, linkNode);
}

void tTaskSchedInit ()//��ʼ��������
{
	int i;
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);
	for(i=0; i<TINYOS_PRO_COUNT; ++i)
	{
		tListInit(&taskTable[i]);
	}
}

void tTaskSchedDisable ()//����������
{
	uint32_t status = tTaskEnterCritical();
	
	if(schedLockCount < 255)
	{
		schedLockCount++;
	}
	
	tTaskExitCritical(status);

}

void tTaskSchedEnable ()//����������
{
	uint32_t status = tTaskEnterCritical();
	
	if(schedLockCount > 0)
	{
		if(--schedLockCount == 0)
		{
			tTaskSched ();
		}
	}
}

//������Ĳ���
void tTaskSchedRdy (tTask * task)
{
	tListAddFirst(&(taskTable[task->prio]),&(task->linkNode));
	tBitmapSet(&taskPrioBitmap, task->prio);
}

//�������ɾ��
void tTaskSchedUnRdy (tTask * task)
{
	tListRemove(&(taskTable[task->prio]),&(task->linkNode));
	if(tListCount(&taskTable[task->prio]) == 0)
	{
		tBitmapClear(&taskPrioBitmap, task->prio);
	} 
}

void tTaskSchedRemove(tTask* task)
{
	tListRemove(&(taskTable[task->prio]),&(task->linkNode));
	if(tListCount(&taskTable[task->prio]) == 0)
	{
		tBitmapClear(&taskPrioBitmap, task->prio);
	}
}

void tTaskSched ()//�����л����Ⱥ���
{
	tTask *tempTask;
	uint32_t status = tTaskEnterCritical();
	
	if(schedLockCount > 0)
	{
		tTaskExitCritical(status);
	}
	
	tempTask = tTaskHighestReady();
	if(tempTask != currentTask)
	{
		nextTask = tempTask;
		tTaskSwitch();
	}
	
	tTaskExitCritical(status);
}

void tTaskDealyedInit()
{
	tListInit(&tTaskDelayedList);
}

//��ʱ�б�Ĳ���
void tTimeTaskWait (tTask * task, uint32_t ticks)
{
	task->delayTicks = ticks;
	tListAddLast(&tTaskDelayedList, &(task->delayNode));
	task->state |= TINYOS_TASK_STATE_DELAYED;
}

//��ʱ�б��ɾ��
void tTimeTaskWakeUp (tTask * task)
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));
	task->state &= ~TINYOS_TASK_STATE_DELAYED;
}

void tTimeTaskRemove(tTask* task)
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));
}

//�ݼ����������ʱ������
void tTaskSystemTickHandler ()
{
	uint32_t status = tTaskEnterCritical();
	
	tNode* node;
	for(node = tTaskDelayedList.headNode.nextNode; node!=&(tTaskDelayedList.headNode); node = node->nextNode)
	{
		tTask* task = tNodeParent(node, tTask, delayNode);
		if (--task->delayTicks == 0)
		{
			if(task->waitEvent)
			{
				tEventRemoveTask(task, (void*)0,tErrorTimeout);
			}
			tTimeTaskWakeUp(task);
			tTaskSchedRdy(task);
		}
	}
	
	if (--currentTask->slice == 0)
	{
		if (tListCount(&taskTable[currentTask->prio]) > 0)
		{
			tListRemoveFirst(&taskTable[currentTask->prio]);
			tListAddLast(&taskTable[currentTask->prio], &(currentTask->linkNode));
			
			currentTask->slice = TINYOS_SLICE_MAX;
		}
	}

	tTaskExitCritical(status);
	tTaskSched();
}




/********������********/
tTask tTaskIdle;//��������
tTaskStack idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE];//��������ֵ

//��������ģ��
void idleTaskEntry (void * param)
{
	for(;;){}
}


int main ()//������
{
	tTaskSchedInit();
	tTaskDealyedInit();
	
	tInitApp();
	
	tTaskInit(&tTaskIdle, idleTaskEntry, (void*)0, TINYOS_PRO_COUNT-1, &idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE]);
	idleTask = &tTaskIdle;
	
	nextTask = tTaskHighestReady();
	
	tTaskRunFirst();
	
	return 0;
}
