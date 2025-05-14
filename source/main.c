/********头文件声明区********/



#include "tinyOS.h"
#include "ARMCM3.h"
tTask * currentTask;//当前任务
tTask * nextTask;//下个任务
tTask * idleTask;//空闲任务
tTask * taskTable[2];//任务栈

uint8_t schedLockCount;//调度锁



/********函数区********/


void tTaskSched ();
void delay (int count);//软延时函数

void tTaskInit (tTask *task, void (*entry) (void *), void *param, tTaskStack *stack)
{
	*(--stack) = (unsigned long) (1 << 24);
	*(--stack) = (unsigned long) entry;
	*(--stack) = (unsigned long) 0x14;
	*(--stack) = (unsigned long) 0x12;
	*(--stack) = (unsigned long) 0x3;
	*(--stack) = (unsigned long) 0x2;
	*(--stack) = (unsigned long) 0x1;
	*(--stack) = (unsigned long) param;
	
	*(--stack) = (unsigned long) 0x11;
	*(--stack) = (unsigned long) 0x10;
	*(--stack) = (unsigned long) 0x9;
	*(--stack) = (unsigned long) 0x8;
	*(--stack) = (unsigned long) 0x7;
	*(--stack) = (unsigned long) 0x6;
	*(--stack) = (unsigned long) 0x5;
	*(--stack) = (unsigned long) 0x4;
	
	task->stack = stack;
	task->delayTicks = 0;
}

void tTaskSchedInit ()//初始化调度锁
{
	schedLockCount = 0;
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

void tTaskSched ()//任务切换调度函数
{
	uint32_t status = tTaskEnterCritical();
	
	if(schedLockCount > 0)
	{
		tTaskExitCritical(status);
	}
	
	if(currentTask == idleTask)
	{
		if(taskTable[0]->delayTicks == 0)
		{
			nextTask = taskTable[0];
		}
		else if(taskTable[1]->delayTicks == 0)
		{
			nextTask = taskTable[1];
		}
		else
		{
			tTaskExitCritical(status);
			return;
		}
	} 
	else 
	{
		if(currentTask == taskTable[0])
		{
			if(taskTable[1]->delayTicks == 0)
			{
				nextTask = taskTable[1];
			}
			else if(currentTask->delayTicks !=0)
			{
				nextTask = idleTask;
			}
			else
			{
				tTaskExitCritical(status);
				return;
			}
		}
		else if(currentTask == taskTable[1])
		{
			if(taskTable[1]->delayTicks == 0)
			{
				nextTask = taskTable[0];
			}
			else if(currentTask->delayTicks != 0)
			{
				nextTask = idleTask;
			}
			else
			{
				tTaskExitCritical(status);
				return;
			}
		}
	}
	
	tTaskSwitch();
	
	tTaskExitCritical(status);
}

void tTaskSystemTickHandler ()//递减各任务的延时计数器
{
	int i;
	
	uint32_t status = tTaskEnterCritical();
	for(i = 0; i < 2; i++)
	{
		if (taskTable[i]->delayTicks > 0)
		{
			taskTable[i]->delayTicks--;
		}
	}
	tTaskExitCritical(status);
	tTaskSched();
}

void tTaskDelay (uint32_t delay)//延时&切换函数
{
	uint32_t status = tTaskEnterCritical();
	currentTask->delayTicks = delay;
	tTaskExitCritical(status);
	tTaskSched();
}

void tSetSysTickPeriod (uint32_t ms)//SysTick的初始化函数，参数为毫秒
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;//初始化SysTick的寄存器
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;//递减寄存器
	SysTick->CTRL = //控制寄存器
		SysTick_CTRL_CLKSOURCE_Msk |
		SysTick_CTRL_TICKINT_Msk |
		SysTick_CTRL_ENABLE_Msk ;
}

void SysTick_Handler ()//SysTick的中断处理函数
{
	tTaskSystemTickHandler();
}





/********任务区********/



tTask tTask1;//任务1
tTaskStack task1Env[1024];//任务1赋值

tTask tTask2;//任务2
tTaskStack task2Env[1024];//任务2赋值

tTask tTaskIdle;//空闲任务
tTaskStack idleTaskEnv[1024];//空闲任务赋值

//空闲任务模块
void idleTaskEntry (void * param)
{
	for(;;)
	{
		
	}
}


//任务1模块
int task1Flag;//任务1的标志位
int firstset;//第一个插入的位图标志位
void task1Entry (void * param) 
{
	int i;
	
	tBitmap bitmap;
	
	
	tBitmapInit(&bitmap);
	  
	
	for(i = tBitmapPosCount() - 1; i >= 0; i--)//测试获取第一位
	{
		tBitmapSet(&bitmap, i);
		
		firstset = tBitmapGetFirstSet(&bitmap);
	}
	
	for(i = 0; i < tBitmapPosCount(); i++)
	{
		tBitmapClear(&bitmap, i);
		firstset = tBitmapGetFirstSet(&bitmap);
	}
	tSetSysTickPeriod(10);
	for(;;)
	{
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
	}
}

//任务2模块
int task2Flag;//任务2的标志位
void task2Entry (void * param) {
	for(;;)
	{
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
		
	}
}

void delay (int count)
{
	while (--count > 0);
}



int main ()//主函数
{
	tTaskInit(&tTask1, task1Entry, (void*)0x11111111, &task1Env[1024]);
	tTaskInit(&tTask2, task2Entry, (void*)0x22222222, &task2Env[1024]);
	
	
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	
	nextTask = taskTable[0];
	
	tTaskInit(&tTaskIdle, idleTaskEntry, (void*)0, &idleTaskEnv[1024]);
	idleTask = &tTaskIdle;
	
	tTaskRunFirst();
	
	return 0;
}
