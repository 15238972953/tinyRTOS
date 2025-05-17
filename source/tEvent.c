#include "tinyOS.h"

void tEventInit(tEvent * event, tEventType type)
{
	event->type = tEventTypeUnknow;
    tListInit(&event->waitList);
}

void tEventWait (tEvent * event, tTask * task, void * msg, uint32_t state, uint32_t timeout)
{
    uint32_t status = tTaskEnterCritical();

    task->state |= state;
    task->waitEvent = event;
    task->eventMsg = msg;
    task->waitEventResult = tErrorNoError;

    tTaskSchedUnRdy(task);
    tListAddLast(&event->waitList, &task->linkNode);

    if(timeout)
    {
        tTimeTaskWait(task, timeout);
    }
    tTaskExitCritical(status);
}

//唤醒事件控制块队列中头部的事件
tTask* tEventWakeUp(tEvent * event, void * msg, uint32_t result)
{
    tNode * node;
    tTask * task = (tTask *)0;
    uint32_t status = tTaskEnterCritical();

    if((node =  tListRemoveFirst(&event->waitList)) != (tNode *)0)
    {
        task = (tTask *)tNodeParent(node, tTask, linkNode);
        task->waitEvent = (tEvent *)0;
        task->eventMsg = msg;
        task->waitEventResult = result;
        task->state &= ~TINYOS_TASK_WAIT_MASK;

        if(task->delayTicks != 0)
        {
            tTimeTaskWakeUp(task);
        }
        tTaskSchedRdy(task);
    }

    tTaskExitCritical(status);
		
	return task;
}

//唤醒事件控制块队列中头部的事件
tTask * tEventWakeUpTask (tEvent * event, tTask * task, void * msg, uint32_t result)
{
    uint32_t status = tTaskEnterCritical();

    tListRemove(&event->waitList, &task->linkNode);
    task->waitEvent = (tEvent *)0;
    task->eventMsg = msg;
    task->waitEventResult = result;
    task->state &= ~TINYOS_TASK_WAIT_MASK;

    if(task->delayTicks != 0)
    {
        tTimeTaskWakeUp(task);
    }
    tTaskSchedRdy(task);

    tTaskExitCritical(status);
		
	return task;
}

//将某一任务从事件控制块的等待队列中移除
void tEventRemoveTask (tTask * task, void * msg, uint32_t result)
{
    uint32_t status = tTaskEnterCritical();
    tListRemove(&task->waitEvent->waitList, &task->linkNode);
    task->waitEvent = (tEvent *)0;
    task->eventMsg = msg;
    task->waitEventResult = result;
    task->state &= ~TINYOS_TASK_WAIT_MASK;

    tTaskExitCritical(status);
}

//删除事件控制块队列中的所有事件
uint32_t tEventRemoveAll (tEvent * event, void * msg, uint32_t result)
{
    tNode * node;
    uint32_t count = 0;
    uint32_t status = tTaskEnterCritical();

    count = tListCount(&event->waitList);

    while((node = tListRemoveFirst(&event->waitList)) != (tNode *)0)
    {
        tTask * task = (tTask *)tNodeParent(node, tTask, linkNode);
        task->waitEvent = (tEvent *)0;
        task->eventMsg = msg;
        task->waitEventResult = result;
        task->state &= ~TINYOS_TASK_WAIT_MASK;

        if(task->delayTicks != 0)
        {
            tTimeTaskWakeUp(task);
        }
        tTaskSchedRdy(task);
    }

    tTaskExitCritical(status);
    return count;
}

//获取事件控制块队列中等待的事件数量
uint32_t tEventWaitCount(tEvent* event)
{
    uint32_t count = 0;
    uint32_t status = tTaskEnterCritical();

    count = tListCount(&event->waitList);

    tTaskExitCritical(status);
    return count;
}
