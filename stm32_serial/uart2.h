#ifndef _STM32F0_UART2_H
#define _STM32F0_UART2_H

#include <stdint.h>

void uart2_init(void);

void uart2_send_char(uint8_t);
void uart2_print(uint8_t *);

#endif
