/********ͷ�ļ�������********/



#include "tinyOS.h"
#include "ARMCM3.h"
tTask * currentTask;//��ǰ����
tTask * nextTask;//�¸�����
tTask * idleTask;//��������
tTask * taskTable[2];//����ջ

uint8_t schedLockCount;//������



/********������********/


void tTaskSched ();
void delay (int count);//����ʱ����

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

void tTaskSchedInit ()//��ʼ��������
{
	schedLockCount = 0;
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

void tTaskSched ()//�����л����Ⱥ���
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

void tTaskSystemTickHandler ()//�ݼ����������ʱ������
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

void tTaskDelay (uint32_t delay)//��ʱ&�л�����
{
	uint32_t status = tTaskEnterCritical();
	currentTask->delayTicks = delay;
	tTaskExitCritical(status);
	tTaskSched();
}

void tSetSysTickPeriod (uint32_t ms)//SysTick�ĳ�ʼ������������Ϊ����
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;//��ʼ��SysTick�ļĴ���
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;//�ݼ��Ĵ���
	SysTick->CTRL = //���ƼĴ���
		SysTick_CTRL_CLKSOURCE_Msk |
		SysTick_CTRL_TICKINT_Msk |
		SysTick_CTRL_ENABLE_Msk ;
}

void SysTick_Handler ()//SysTick���жϴ�����
{
	tTaskSystemTickHandler();
}





/********������********/



tTask tTask1;//����1
tTaskStack task1Env[1024];//����1��ֵ

tTask tTask2;//����2
tTaskStack task2Env[1024];//����2��ֵ

tTask tTaskIdle;//��������
tTaskStack idleTaskEnv[1024];//��������ֵ

//��������ģ��
void idleTaskEntry (void * param)
{
	for(;;)
	{
		
	}
}


//����1ģ��
int task1Flag;//����1�ı�־λ
int firstset;//��һ�������λͼ��־λ
void task1Entry (void * param) 
{
	int i;
	
	tBitmap bitmap;
	
	
	tBitmapInit(&bitmap);
	  
	
	for(i = tBitmapPosCount() - 1; i >= 0; i--)//���Ի�ȡ��һλ
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

//����2ģ��
int task2Flag;//����2�ı�־λ
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



int main ()//������
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
