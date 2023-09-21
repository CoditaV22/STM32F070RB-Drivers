#include "stm32f4xx.h"

#define I2C1EN		   			(1U << 21)
#define GPIOBEN		    		(1U << 1)

#define CR1_SWRST 			(1U << 15)
#define CR1_PE				(1U << 0)
#define CR1_START			(1U << 8)
#define CR1_STOP			(1U << 9)
#define CR1_ACK				(1U << 10)

#define SR2_BUSY			(1U << 1)
#define SR1_SB			    (1U << 0)
#define SR1_ADDR			(1U << 1)
#define SR1_BTF				(1U << 2)
#define SR1_RXNE	        (1U << 6)
#define SR1_TXE		        (1U << 7)
#define CCR_100KHz 			80
#define SD_MODE_MAX_RISE_TIME	17

void I2C1_init(void)
{
	/*Enable Clk Access to GPIOB*/
	RCC -> AHB1ENR |= GPIOBEN;

	/*Set PB8 and PB9 mode to alternate function*/
	GPIOB -> MODER &=~ (1U << 16);
	GPIOB -> MODER |=  (1U << 17);

	GPIOB -> MODER &=~ (1U << 18);
	GPIOB -> MODER |=  (1U << 19);

	/*Select AF type*/
	GPIOB -> AFR[1] &=~ (1U << 0);
	GPIOB -> AFR[1] &=~ (1U << 1);
	GPIOB -> AFR[1] |=  (1U << 2);
	GPIOB -> AFR[1] &=~ (1U << 3);

	GPIOB -> AFR[1] &=~ (1U << 4);
	GPIOB -> AFR[1] &=~ (1U << 5);
	GPIOB -> AFR[1] |=  (1U << 6);
	GPIOB -> AFR[1] &=~ (1U << 7);

	/*Set output type to open-drain*/
	GPIOB -> OTYPER |= (1U << 8);
	GPIOB -> OTYPER |= (1U << 9);

	/*Enable Pull-up for PB8 and PB9*/
	GPIOB -> PUPDR |=  (1U << 16);
	GPIOB -> PUPDR &=~ (1U << 17);

	GPIOB -> PUPDR |=  (1U << 18);
	GPIOB -> PUPDR &=~ (1U << 19);

	/*Enable Clk Access to I2C1*/
	RCC -> APB1ENR |= I2C1EN;

	/*Enter Reset Mode*/
	I2C1 -> CR1 |= CR1_SWRST;

	/*Leave Reset Mode*/
	I2C1 -> CR1 &=~ CR1_SWRST;

	/*Set periph clock value*/
	I2C1 -> CR2 |= (1 << 4); //16 MHz

	/*Set I2C to standard mode , 100KHz Clk*/
	I2C1 -> CCR = CCR_100KHz;

	/*Set rise time*/
	I2C1 -> TRISE = SD_MODE_MAX_RISE_TIME;

	/*Enable I2C*/
	I2C1 -> CR1 |= CR1_PE;

}

void I2C1_byteRead(char saddr , char maddr , char* data )
{
	volatile int tmp;

	/*Wait for I2C not to be busy*/
	while((I2C1 -> SR2 & SR2_BUSY)){}

	/*Generate Start Condition*/
	I2C1 -> CR1 |= CR1_START;

	/*Wait until start flag to be set*/
	while(!(I2C1 -> SR1 & SR1_SB)){}

	/*Send slave address and write*/
	I2C1 -> DR = saddr << 1;

	/*Wait for addr flag*/
	while(!(I2C1 -> SR1 & SR1_ADDR)){}

	/*Clear addr flag*/
	tmp = I2C1 -> SR2;

	/*Send mem address*/
	I2C1 -> DR = maddr;

	/*Wait until tx empty*/
	while(!(I2C1 -> SR1 & SR1_TXE)){}

	/*Generate restart*/
	I2C1 -> CR1 |= CR1_START;

	/*Wait until start flag to be set*/
	while(!(I2C1 -> SR1 & SR1_SB)){}

	/*Send slave address and read*/
	I2C1 -> DR = saddr << 1 | 1 ;

	/*Disable acknowledge*/
	I2C1 -> CR1 &=~ CR1_ACK;

	/*Clear addr flag*/
	tmp = I2C1 -> SR2;

	/*Generate stop*/
	I2C1 -> CR1 |= CR1_STOP;


	/*Wait until rx empty*/
	while(!(I2C1 -> SR1 & SR1_RXNE)){}

	/*Store*/
	*data++ = I2C1 -> DR;
}

void I2C1_burstRead(char saddr , char maddr , int n , char* data)
{
	volatile int tmp;

	/*Wait for I2C not to be busy*/
	while((I2C1 -> SR2 & SR2_BUSY)){}

	/*Generate Start Condition*/
	I2C1 -> CR1 |= CR1_START;

	/*Wait until start flag to be set*/
	while(!(I2C1 -> SR1 & SR1_SB)){}

	/*Send slave address and write*/
	I2C1 -> DR = saddr << 1;

	/*Wait for addr flag*/
	while(!(I2C1 -> SR1 & SR1_ADDR)){}

	/*Clear addr flag*/
	tmp = I2C1 -> SR2;

	/*Wait until tx empty*/
	while(!(I2C1 -> SR1 & SR1_TXE)){}

	/*Send mem address*/
	I2C1 -> DR = maddr;

	/*Wait until tx empty*/
	while(!(I2C1 -> SR1 & SR1_TXE)){}

	/*Generate restart*/
	I2C1 -> CR1 |= CR1_START;

	/*Wait until start flag to be set*/
	while(!(I2C1 -> SR1 & SR1_SB)){}

	/*Send slave address and read*/
	I2C1 -> DR = saddr << 1 | 1 ;

	/*Wait for addr flag*/
	while(!(I2C1 -> SR1 & SR1_ADDR)){}

	/*Clear addr flag*/
	tmp = I2C1 -> SR2;

	/*Enable acknowledge*/
	I2C1 -> CR1 |= CR1_ACK;

	while(n > 0U)
	{
		/*Check if n == 1*/
		if(n == 1U)
		{
			/*Disable acknowledge*/
			I2C1 -> CR1 &=~ CR1_ACK;

			/*Generate stop*/
			I2C1 -> CR1 |= CR1_STOP;

			/*Wait until rx empty*/
			while(!(I2C1 -> SR1 & SR1_RXNE)){}

			/*Store*/
			*data++ = I2C1 -> DR;

			break;
		}
		else
		{
			while(!(I2C1 -> SR1 & SR1_RXNE)){}
				/*Store*/
				*data++ = I2C1 -> DR;
				n--;
		}
	}
}

void I2C1_burstWrite(char saddr , char maddr , int n , char* data)
{
	volatile int tmp;

	/*Wait for I2C not to be busy*/
	while((I2C1 -> SR2 & SR2_BUSY)){}

	/*Generate Start Condition*/
	I2C1 -> CR1 |= CR1_START;

	/*Wait until start flag to be set*/
	while(!(I2C1 -> SR1 & SR1_SB)){}

	/*Send slave address and write*/
	I2C1 -> DR = saddr << 1;

	/*Wait for addr flag*/
	while(!(I2C1 -> SR1 & SR1_ADDR)){}

	/*Clear addr flag*/
	tmp = I2C1 -> SR2;

	/*Wait until tx empty*/
	while(!(I2C1 -> SR1 & SR1_TXE)){}

	/*Send mem address*/
	I2C1 -> DR = maddr;

	for(int i = 0 ; i < n ; i ++)
	{
		/*Wait until tx empty*/
		while(!(I2C1 -> SR1 & SR1_TXE)){}

		I2C1 -> DR = *data++;
	}

	while(!(I2C1 -> SR1 & SR1_BTF)){}

	I2C1 -> CR1 |= CR1_STOP;


}
