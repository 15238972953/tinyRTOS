#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>

#include "tLib.h"

typedef uint32_t tTaskStack;

typedef struct _tTask {
	tTaskStack * stack;
	uint32_t delayTicks;//软延时器计数器
}tTask;

extern tTask * currentTask;
extern tTask * nextTask;


void tTaskSchedInit ();//初始化调度锁

void tTaskSchedDisable ();//调度锁上锁
void tTaskSchedEnable ();//调度锁解锁
uint32_t tTaskEnterCritical (void);
void tTaskExitCritical (uint32_t status);

void tTaskRunFirst(void);
void tTaskSwitch(void);
#endif