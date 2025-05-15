#include "tinyOS.h"


tTask tTask1;//����1
tTaskStack task1Env[1024];//����1��ֵ

tTask tTask2;//����2
tTaskStack task2Env[1024];//����2��ֵ

tTask tTask3;//����3
tTaskStack task3Env[1024];//����3��ֵ

tTask tTask4;//����4
tTaskStack task4Env[1024];//����4��ֵ

tEvent eventWaitTimeout;
tEvent eventWaitNormal;

//����1ģ��
int task1Flag;//����1�ı�־λ
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

//����2ģ��
int task2Flag;//����2�ı�־λ
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

//����3ģ��
int task3Flag;//����3�ı�־λ
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

//����4ģ��
int task4Flag;//����4�ı�־λ
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