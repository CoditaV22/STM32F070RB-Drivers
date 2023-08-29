#include "stm32f0xx.h"
#include "adc.h"

#define GPIOAEN 	(1U << 17)
#define ADCEN 		(1U << 9)
#define ADEN		(1U << 0)
#define HSI14ON 	(1U << 0)
#define HSI14RDY 	(1U << 1)
#define ADRDY 		(1U << 0)
#define ADSTP		(1U << 4)
#define ADDIS 		(1U << 1)
#define DMAEN		(1U << 0)
#define ADCAL		(1U << 31)
#define ADSTART 	(1U << 2)
#define CONT		(1U << 13)
#define EOC 		(1U << 2)
#define EOSMP 		(1U << 1)


void pa1_adc_init(void)
{
	RCC -> AHBENR |= GPIOAEN;

	RCC -> APB2ENR |= ADCEN;

	RCC -> CR2 |= HSI14ON;

	while(!(RCC -> CR2 & HSI14RDY)){}

	if(ADC1 -> ISR & ADRDY)
	{
		ADC1 -> CR |= ADSTP;
		while(ADC1 -> CR & ADSTP){}
		ADC1 -> CR |= ADDIS;
	}

	if (((ADC1 -> CR & ADEN)==1)&&(!(ADC1 -> CR & ADDIS)))
	{
			ADC1 -> CR |= ADDIS;
			while((ADC1->CR & ADEN)){}
			ADC1 -> CFGR1 &=~ (DMAEN);
	}

	ADC1 -> CR |= ADCAL;
	while((ADC1 -> CR & ADCAL)){}
		__NOP(); // Wait and Do nothing
		__NOP(); // Wait and Do nothing
		__NOP(); // Wait and Do nothing
		__NOP(); // Wait and Do nothing

	if((ADC1 -> ISR & ADRDY))
	{
		ADC1 -> ISR |= ADRDY;
	}
	ADC1 -> CR |= ADEN;
	while(!(ADC1 ->ISR & ADRDY)){}

	if (!(ADC1 -> CR & ADEN))
	{
		ADC1 -> CR |= ADEN;
	}

}

void adc_config(void)
{
	if(((ADC1 -> ISR & ADRDY)) && (!(ADC1 -> CR & ADSTART)))
	{
		ADC1 -> CFGR2 &= ~ADC_CFGR2_CKMODE;
		ADC1 -> CFGR1 &=~ CONT;

		GPIOA -> MODER |= (1U << 2);
		GPIOA -> MODER |= (1U << 3);

		ADC1-> CHSELR |= (1U << 1);
	}
}

void start_adc_single_conversion_mode(void)
{

	ADC1 -> CR |= ADSTART;
}

int adc_read(void)
{
	while(!(ADC1 -> ISR & EOC));


	ADC1 -> ISR |= EOSMP;

	return(ADC1 -> DR);
}

