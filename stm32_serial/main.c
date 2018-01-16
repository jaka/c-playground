#include <stdint.h>
#include "uart2.h"
#include "cmsis/stm32f072xb.h"

void on(void) {
  *((uint32_t *)0x48000018) = 0x20;
}

void off(void) {
  *((uint32_t *)0x48000018) = 0x200000;
}

#define DELAY 0x3ffff

int main(void)
{
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  volatile uint32_t i;
  uint8_t string[5] = "jaka";

  /* UART */
  GPIOA->AFR[0] |= (0x01 << GPIO_AFRL_AFRL3_Pos) | (0x01 << GPIO_AFRL_AFRL2_Pos);
  GPIOA->MODER |= GPIO_MODER_MODER3_1 | GPIO_MODER_MODER2_1;
  uart2_init();

  /* GPIO */
  *((uint32_t *)0x40021014) |= 0x20000;
  *((uint32_t *)0x48000000) |= 0x400;

  while (1) {

    off();
    for (i = DELAY; i; i--);

    for (i = '0'; i < '9'; i++)
      uart2_send_char(i);

    uart2_print((uint8_t *)"\r\n");

    for (i = 0; i < 4; i++)
      uart2_print(string);

    on();
    for (i = DELAY; i; i--);

  }
  
}
