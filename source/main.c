/********头文件声明区********/

#include "tinyOS.h"
#include "ARMCM3.h"


tTask * currentTask;//当前任务
tTask * nextTask;//下个任务
tTask * idleTask;//空闲任务
tBitmap taskPrioBitmap;
tList taskTable[TINYOS_PRO_COUNT];//任务栈

uint8_t schedLockCount;//调度锁
tList tTaskDelayedList;

uint32_t tickCount;
uint32_t idleCount;
uint32_t idleMaxCount;

#if TINYOS_ENABLE_CPUUSAGE_STAT == 1
static void initCpuUsageStat();
static void checkCpuUsage(void);
static void cpuUsageSyncWithSysTick(void);
#endif

/********函数区********/

//获取任务列表中优先级最高的任务
tTask* tTaskHighestReady(void)
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	tNode* node = tListFirst(&taskTable[highestPrio]);
	return tNodeParent(node, tTask, linkNode);
}

void tTaskSchedInit ()//初始化调度锁
{
	int i;
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);
	for(i=0; i<TINYOS_PRO_COUNT; ++i)
	{
		tListInit(&taskTable[i]);
	}
}

void tTaskSchedDisable ()//调度锁上锁
{
	uint32_t status = tTaskEnterCritical();
	
	if(schedLockCount < 255)
	{
		schedLockCount++;
	}
	
	tTaskExitCritical(status);

}

void tTaskSchedEnable ()//调度锁解锁
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

//就绪表的插入
void tTaskSchedRdy (tTask * task)
{
	tListAddFirst(&(taskTable[task->prio]),&(task->linkNode));
	tBitmapSet(&taskPrioBitmap, task->prio);
}

//就绪表的删除
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

void tTaskSched ()//任务切换调度函数
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
#if TINYOS_ENABLE_HOOKS == 1
		tHooksTaskSwitch(currentTask, nextTask);
#endif
		tTaskSwitch();
	}
	
	tTaskExitCritical(status);
}

void tTaskDelayedInit()
{
	tListInit(&tTaskDelayedList);
}

//延时列表的插入
void tTimeTaskWait (tTask * task, uint32_t ticks)
{
	task->delayTicks = ticks;
	tListAddLast(&tTaskDelayedList, &(task->delayNode));
	task->state |= TINYOS_TASK_STATE_DELAYED;
}

//延时列表的删除
void tTimeTaskWakeUp (tTask * task)
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));
	task->state &= ~TINYOS_TASK_STATE_DELAYED;
}

void tTimeTaskRemove(tTask* task)
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));
}

void tTimeTickInit(void)
{
	tickCount = 0;
}

//递减各任务的延时计数器
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
	tickCount++;
	
#if TINYOS_ENABLE_CPUUSAGE_STAT == 1
	checkCpuUsage();
#endif
	
	tTaskExitCritical(status);
	
#if TINYOS_ENABLE_TIMER == 1
	tTimerModuleTickNotify();
#endif 
	
#if TINYOS_ENABLE_HOOKS == 1
	tHooksSysTick();
#endif
	tTaskSched();
}
#if TINYOS_ENABLE_CPUUSAGE_STAT == 1
static float cpuUsage;
static uint32_t enableCpuUsageState;

static void initCpuUsageStat(void)
{
	idleCount = 0;
	idleMaxCount = 0;
	cpuUsage = 0.0f;
	enableCpuUsageState = 0;
}

static void checkCpuUsage(void)
{
	if(enableCpuUsageState == 0)
	{
		enableCpuUsageState = 1;
		tickCount = 0;
		return;
	}
	
	if(tickCount == TICK_PER_SEC)
	{
		idleMaxCount = idleCount;
		idleCount = 0;
		
		tTaskSchedEnable();
	}
	else if(tickCount % TICK_PER_SEC == 0)
	{
		cpuUsage = 100 - (idleCount * 100.0 / idleMaxCount);
		idleCount = 0;
	}
}

static void cpuUsageSyncWithSusTick(void)
{
	while(enableCpuUsageState == 0)
	{
		;;
	}
}

float tCpuUsageGet(void)
{
	float usage = 0;
	uint32_t status = tTaskEnterCritical();
	usage = cpuUsage;
	tTaskExitCritical(status);
	return usage;
}
#endif


tTask tTaskIdle;
tTaskStack idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE];


void idleTaskEntry (void * param)
{
	tTaskSchedDisable();
	tInitApp();
	
#if TINYOS_ENABLE_TIMER ==1
	tTimerInitTask();
#endif
	
	tSetSysTickPeriod(TINYOS_SYSTICK_MS);

#if TINYOS_ENABLE_CPUUSAGE_STAT == 1
	cpuUsageSyncWithSusTick();
#endif
	
	for(;;)
	{
		uint32_t status = tTaskEnterCritical();
		idleCount++;
		tTaskExitCritical(status);

#if TINYOS_ENABLE_HOOKS == 1
	tHooksCpuIdle();
#endif
		
	}
}


int main ()//主函数
{
	tTaskSchedInit();
	tTaskDelayedInit();
	
	#if TINYOS_ENABLE_TIMER == 1
		tTimerModuleInit();
	#endif
	
	tTimeTickInit();

#if TINYOS_ENABLE_CPUUSAGE_STAT == 1
	initCpuUsageStat();
#endif
	
	tTaskInit(&tTaskIdle, idleTaskEntry, (void*)0, TINYOS_PRO_COUNT-1, idleTaskEnv, TINYOS_IDLETASK_STACK_SIZE);
	idleTask = &tTaskIdle;
	
	nextTask = tTaskHighestReady();
	
	tTaskRunFirst();
	
	return 0;
}
