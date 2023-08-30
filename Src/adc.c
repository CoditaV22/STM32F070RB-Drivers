#include "stm32f0xx.h"
#include "adc.h"


void pa1_adc_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
	RCC->CR2 |= RCC_CR2_HSI14ON;
	while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0)
	{
		__NOP();
	}

	////////// Turn off the ADC ///////////////
	if ((ADC1->ISR & ADC_ISR_ADRDY) != 0) // An ADC conversion is occurring
	{
		ADC1->CR |= ADC_CR_ADSTP; // Stop the conversion
		do
		{
			__NOP(); // Nothing
		}while((ADC1->CR & ADC_CR_ADSTP) != 0); // Wait until conversion has stopped
		ADC1->CR |= ADC_CR_ADDIS; // Disable the ADC
	}

	/* Ensure that ADEN=0 and DMAEN=0 (only if ADC is enabled and there is no pending disable) */
	if (((ADC1->CR & ADC_CR_ADEN)==1)&&((ADC1->CR & ADC_CR_ADDIS)==0))
	{
		ADC1->CR |= ADC_CR_ADDIS;
		while((ADC1->CR & ADC_CR_ADEN) != 0){ //Wait until ADC Enable = 0
			__NOP(); // Wait and Do nothing
		}
		ADC1->CFGR1 &= ~( ADC_CFGR1_DMAEN);
	}

	/* Calibrate the ADC */
	ADC1->CR |= ADC_CR_ADCAL;
	/* Wait for ADC Calibration to finish */
	while((ADC1->CR & ADC_CR_ADCAL)!=0){
		__NOP(); // Wait and Do nothing
	}
	/* Wait 4 clock cycles after calibration	 */
	__NOP(); // Wait and Do nothing
	__NOP(); // Wait and Do nothing
	__NOP(); // Wait and Do nothing
	__NOP(); // Wait and Do nothing

	//////////START SEQUENCE///////////////
	/* Clear ADRDY bit by programming to 1 (Perhaps I need a delay)*/
	if((ADC1->ISR & ADC_ISR_ADRDY) != 0)
	{
	ADC1->ISR |= ADC_ISR_ADRDY;
	__NOP(); // Wait 1 clock cycle (might be unnecessary)
	}

	/* Set ADEN in ADC_CR Register */
	ADC1->CR |= ADC_CR_ADEN;
	while((ADC1->ISR & ADC_ISR_ADRDY)== 0)
	{
		__NOP(); // DO Nothing S
	}
	if ((ADC1->CR & ADC_CR_ADEN)==0)
	{
		ADC1->CR |= ADC_CR_ADEN;
	}
}

void adc_config(void)
{
	///////Configure the ADC GPIO PIN//////
	if(((ADC1->ISR & ADC_ISR_ADRDY)==1) && ((ADC1->CR & ADC_CR_ADSTART)==0))
	{
		/* Use the ADC Asynchronous Clock */
		ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;
		/* Set ADC to single conversion mode */
		ADC1->CFGR1 &= ~ADC_CFGR1_CONT;
		/* Set PA1 to analog mode */
		GPIOA->MODER |= GPIO_MODER_MODER1;

		/* Select ADC Ch1 & Ch16 for conversion */
		ADC1->CHSELR |= ADC_CHSELR_CHSEL1;
	}
}

void start_adc_single_conversion_mode(void)
{
	/* Start the conversion */
	ADC1->CR |= ADC_CR_ADSTART;
}

int adc_read(void)
{
	//////////READ THE CONVERSION///////////
	/* Wait for conversion to be complete */
	do {
		__NOP(); // nothing
	}while((ADC1->ISR & ADC_ISR_EOC)==0); //Check this

	/* Clear End of Sample Flag */
	ADC1->ISR |= ADC_ISR_EOSMP;

	/* Read and return converted result (This action also clears EOC flag) */
	return(ADC1->DR);
}
