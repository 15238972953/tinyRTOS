#ifndef THOOKS_H
#define THOOKS_H

#include "tinyOS.h"

// 用于处理CPU空闲时调用
void tHooksCpuIdle (void);
// 用于处理系统时钟中断时调用
void tHooksSysTick (void);
// 用于处理任务切换时调用
void tHooksTaskSwitch (tTask * from, tTask * to);
// 用于初始化任务时调用
void tHooksTaskInit (tTask * task);

#endif