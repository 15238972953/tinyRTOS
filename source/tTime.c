#include "tinyOS.h"

//ÑÓÊ±&ÇÐ»»º¯Êý
void tTaskDelay (uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	
	tTimeTaskWait(currentTask,delay);
	tTaskSchedUnRdy(currentTask);
	
	tTaskExitCritical(status);
	tTaskSched();
}