#ifndef THOOKS_H
#define THOOKS_H

#include "tinyOS.h"

// ���ڴ���CPU����ʱ����
void tHooksCpuIdle (void);
// ���ڴ���ϵͳʱ���ж�ʱ����
void tHooksSysTick (void);
// ���ڴ��������л�ʱ����
void tHooksTaskSwitch (tTask * from, tTask * to);
// ���ڳ�ʼ������ʱ����
void tHooksTaskInit (tTask * task);

#endif