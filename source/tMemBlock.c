#include "tinyOS.h"
#include "tMemBlock.h"

#if TINYOS_ENABLE_MEMBLOCK == 1
void tMemBlockInit(tMemBlock * memBlock, uint8_t * memStart, uint32_t blockSize, uint32_t blockCnt)
{
    uint8_t *  memblockStart = (uint8_t *)memStart;
    uint8_t * memblockEnd = memblockStart + blockSize * blockCnt;

    if (blockSize < sizeof(tNode))
    {
        return;
    }

    tEventInit(&memBlock->event, tEventTypeMemBlock);
    memBlock->memStart = memStart;
    memBlock->blockSize = blockSize;
    memBlock->maxCount = blockCnt;

    tListInit(&memBlock->blockList);
    while(memblockStart < memblockEnd)
    {
        tNodeInit((tNode *)memblockStart);
        tListAddLast(&memBlock->blockList, (tNode *)memblockStart);

        memblockStart += blockSize;
    }
}

uint32_t tMemBlockWait(tMemBlock * memBlock, uint8_t ** mem, uint32_t waitTicks)
{
    uint32_t status = tTaskEnterCritical();

    if(tListCount(&memBlock->blockList) > 0)
    {
        *mem = (uint8_t *)tListRemoveFirst(&memBlock->blockList);
        tTaskExitCritical(status);
        return tErrorNoError;
    }
    else
    {
        tEventWait(&memBlock->event, currentTask, (void *)0, tEventTypeMemBlock, waitTicks);
        tTaskExitCritical(status);

        tTaskSched();

        *mem = currentTask->eventMsg;
        return currentTask->waitEventResult;
    }
}

uint32_t tMemBlockNoWaitGet(tMemBlock * memBlock, void ** mem)
{
    uint32_t status = tTaskEnterCritical();

    if(tListCount(&memBlock->blockList) > 0)
    {
        *mem = (uint8_t *)tListRemoveFirst(&memBlock->blockList);
        tTaskExitCritical(status);
        return tErrorNoError;
    }
    else
    {
        tTaskExitCritical(status);
        return tErrorResourseUnavailable;
    }
}

void tMemBlockNotify(tMemBlock * memBlock, uint8_t * mem)
{
    uint32_t status = tTaskEnterCritical();

    if(tEventWaitCount(&memBlock->event) > 0)
    {
        tTask * task = tEventWakeUp(&memBlock->event, (void *)mem, tErrorNoError);
        if(task->prio < currentTask->prio)
        {
            tTaskSched;
        }
    }
    else
    {
        tListAddLast(&memBlock->blockList, (tNode *)mem);
    }

    tTaskExitCritical(status);
}

//�洢���״̬��ѯ
void tMemBlockGetInfo(tMemBlock * memBlock, tMemBlockInfo * info)
{
    uint32_t status = tTaskEnterCritical();

    info->count = tListCount(&memBlock->blockList);
    info->maxCount = memBlock->maxCount;
    info->blockSize = memBlock->blockSize;
    info->taskCount = tEventWaitCount(&memBlock->event);

    tTaskExitCritical(status);
}

//�洢���ɾ��
uint32_t tMemBlockDestroy(tMemBlock * memBlock)
{
    uint32_t status = tTaskEnterCritical();

    uint32_t count = tEventRemoveAll(&memBlock->event, (void *)0, tErrorDel);


    tTaskExitCritical(status);

    if(count > 0)
    {
        tTaskSched();
    }
    return count;
}
#endif
