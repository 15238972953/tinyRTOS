#include "tinyOS.h"


tTask tTask1;//任务1
tTaskStack task1Env[1024];//任务1赋值

tTask tTask2;//任务2
tTaskStack task2Env[1024];//任务2赋值

tTask tTask3;//任务3
tTaskStack task3Env[1024];//任务3赋值

tTask tTask4;//任务4
tTaskStack task4Env[1024];//任务4赋值

tEvent eventWaitTimeout;
tEvent eventWaitNormal;

//任务1模块
int task1Flag;//任务1的标志位
void task1Entry (void * param) 
{
	tSetSysTickPeriod(10);
	
	tEventInit(&eventWaitTimeout, tEventTypeUnknow);

	for(;;)
	{
		tEventWait(&eventWaitTimeout,currentTask,(void*)0,0,5);
		tTaskSched();
		
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
	}
}

//任务2模块
int task2Flag;//任务2的标志位
void task2Entry (void * param) 
{
	for(;;)
	{
		tEventWait(&eventWaitNormal,currentTask,(void*)0,0,0);
		tTaskSched();
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
	}
}

//任务3模块
int task3Flag;//任务3的标志位
void task3Entry (void * param) 
{
	tEventInit(&eventWaitNormal, tEventTypeUnknow);
	for(;;)
	{
		tEventWait(&eventWaitNormal,currentTask,(void*)0,0,0);
		tTaskSched();
		task3Flag = 0;
		tTaskDelay(1);
		task3Flag = 1;
		tTaskDelay(1);
	}
}

//任务4模块
int task4Flag;//任务4的标志位
void task4Entry (void * param) 
{
	for(;;)
	{
		tTask* rdyTask = tEventWakeUp(&eventWaitNormal,(void*)0,0);
		tTaskSched();
		task4Flag = 0;
		tTaskDelay(1);
		task4Flag = 1;
		tTaskDelay(1);
	}
}

void tInitApp (void)
{
	tTaskInit(&tTask1, task1Entry, (void*)0x11111111, 0, &task1Env[1024]);
	tTaskInit(&tTask2, task2Entry, (void*)0x22222222, 1, &task2Env[1024]);
	tTaskInit(&tTask3, task3Entry, (void*)0x22222222, 0, &task3Env[1024]);
	tTaskInit(&tTask4, task4Entry, (void*)0x44444444, 1, &task4Env[1024]);
	
}