#include "tinyOS.h"

//��ʱ&�л�����
void tTaskDelay (uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	
	tTimeTaskWait(currentTask,delay);
	tTaskSchedUnRdy(currentTask);
	
	tTaskExitCritical(status);
	tTaskSched();
}