#include "tinyOS.h"
#include "ARMCM3.h"

//SysTick�ĳ�ʼ������������Ϊ����
void tSetSysTickPeriod (uint32_t ms)
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
