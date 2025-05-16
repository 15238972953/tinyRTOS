#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>

#include "tLib.h"
#include "tconfig.h"
#include "tEvent.h"
#include "tTask.h"
#include "tSem.h"
#include "tMbox.h"
#include "tMemBlock.h"

typedef enum _tError{
	tErrorNoError = 0,
	tErrorTimeout,
	tErrorResourseUnavailable,
	tErrorDel,
	tErrorResourseFull,
}tError;


extern tTask * currentTask;
extern tTask * nextTask;


void tTaskSchedInit ();//初始化调度锁

void tTaskSchedDisable ();//调度锁上锁
void tTaskSchedEnable ();//调度锁解锁
uint32_t tTaskEnterCritical (void);
void tTaskExitCritical (uint32_t status);

void tTaskRunFirst(void);
void tTaskSwitch(void);

void tTaskSchedRdy (tTask * task);
void tTaskSchedUnRdy (tTask * task);

void tTimeTaskWait (tTask * task, uint32_t ticks);
void tTimeTaskWakeUp (tTask * task);

void tTaskSystemTickHandler ();
void tTaskDelay (uint32_t delay);

void tSetSysTickPeriod (uint32_t ms);
void tInitApp (void);
void tTaskSched ();
void tTimeTaskRemove(tTask* task);
void tTaskSchedRemove(tTask* task);




#endif