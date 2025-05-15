#include "tinyOS.h"
#include "ARMCM3.h"

//SysTick的初始化函数，参数为毫秒
void tSetSysTickPeriod (uint32_t ms)
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
