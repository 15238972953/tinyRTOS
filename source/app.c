#include "tinyOS.h"


tTask tTask1;//任务1
tTaskStack task1Env[1024];//任务1赋值

tTask tTask2;//任务2
tTaskStack task2Env[1024];//任务2赋值

tTask tTask3;//任务3
tTaskStack task3Env[1024];//任务3赋值

tTask tTask4;//任务4
tTaskStack task4Env[1024];//任务4赋值

tFlagGroup flagGroup;


int task1Flag;
void task1Entry (void * param) 
{
	tSetSysTickPeriod(10);

	tFlagGroupInit(&flagGroup, 0xFF);
	for(;;)
	{
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
		tFlagGroupNotify(&flagGroup, 0, 0x6);
	}
}

int task2Flag;
void task2Entry (void * param) 
{
	uint32_t resultFlags;
	for(;;)
	{
		tFlagGroupWait(&flagGroup, TFLAGGROUP_CLEAR_ALL, 0x4, &resultFlags, 10);
		tFlagGroupNoWaitGet(&flagGroup, TFLAGGROUP_CLEAR_ALL, 0x3,&resultFlags);
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
	}
}

int task3Flag;
void task3Entry (void * param) 
{
	for(;;)
	{
		task3Flag = 0;
		tTaskDelay(1);
		task3Flag = 1;
		tTaskDelay(1);
		
	}
}

int task4Flag;
void task4Entry (void * param) {
	for(;;)
	{
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
	tTaskInit(&tTask3, task3Entry, (void*)0x22222222, 1, &task3Env[1024]);
	tTaskInit(&tTask4, task4Entry, (void*)0x44444444, 1, &task4Env[1024]);

}