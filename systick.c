#include "stm32f0xx.h"

#define SYSTICK_LOAD_VAL		8000
#define CTRL_ENABLE 			(1U << 0)
#define CTRL_CLKSRC				(1U << 2)
#define CTRL_COUNTFLAG 			(1U << 16)

void systickDelayMs(int delay)
{
	SysTick -> LOAD =  SYSTICK_LOAD_VAL;

	SysTick -> VAL = 0;

	SysTick -> CTRL = CTRL_ENABLE | CTRL_CLKSRC;

	for(int i = 0; i < delay ; i++)
	{
		while(!(SysTick -> CTRL & CTRL_COUNTFLAG)){}

	}
	SysTick -> CTRL = 0;
}
