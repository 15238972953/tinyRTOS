#include "tinyOS.h"
#include "tMemBlock.h"

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
        tTask * task = tEventWakeUp(&memBlock->event, (void *)0, tErrorNoError);
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

