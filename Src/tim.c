#include "stm32f0xx.h"

#define TIM3EN 	   (1U << 1)
#define CEN 	   (1U << 0)
#define OC_TOGGLE  ((1U << 4) | (1U << 5))
#define CCER_CC1E  (1U << 0)
#define GPIOAEN    (1U << 17)
#define UIE 	   (1U << 0)

void tim3_1Hz_interrupt(void)
{
	//Enable clock access to timer3
	RCC -> APB1ENR |= TIM3EN;

	//Set PSC Value
	TIM3 -> PSC = 800 - 1;

	//Set ARR Value
	TIM3 -> ARR = 10000 - 1;

	//Clear CNT
	TIM3 -> CNT = 0;

	//Enable the timer
	TIM3 -> CR1 |= CEN;

	/*Enable TIM Interrupt*/
	TIM3 -> DIER |= UIE;

	/*Enable TIM Interrupt in NVIC */
	NVIC_EnableIRQ(TIM3_IRQn);
}


void tim3_1Hz_init(void)
{
	//Enable clock access to timer3
	RCC -> APB1ENR |= TIM3EN;

	//Set PSC Value
	TIM3 -> PSC = 800 - 1;

	//Set ARR Value
	TIM3 -> ARR = 10000 - 1;

	//Clear CNT
	TIM3 -> CNT = 0;

	//Enable the timer
	TIM3 -> CR1 |= CEN;
}

void tim3_pa6_Output_Compare(void)
{
	//Enable clk access to gpioa
	RCC -> AHBENR |= GPIOAEN;
	//Config pa6
	GPIOA -> MODER &=! (1U << 12);
	GPIOA -> MODER |= (1U << 13);

	//Set AF
	GPIOA -> AFR[0] |= (1U << 24);


	//Enable clock access to timer3
	RCC -> APB1ENR |= TIM3EN;

	//Set PSC Value
	TIM3 -> PSC = 800 - 1;

	//Set ARR Value
	TIM3 -> ARR = 10000 - 1;

	//Set output copmpare toggle
	TIM3 -> CCMR1 = OC_TOGGLE;

	//Enable tim3 ch1 in compare mode
	TIM3 -> CCER |= CCER_CC1E;

	//Clear CNT
	TIM3 -> CNT = 0;

	//Enable the timer
	TIM3 -> CR1 |= CEN;
}
