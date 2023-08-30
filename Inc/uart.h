
#ifndef UART_H_
#define UART_H_
#include <stdint.h>

#include "stm32f0xx.h"

char uart2_read(void);
void uart2_rxtx_init(void);
void uart2_rxtx_interrupt_init(void);


#endif /* UART_H_ */
