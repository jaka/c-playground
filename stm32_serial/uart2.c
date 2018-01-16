#include "uart2.h"
#include "cmsis/stm32f072xb.h"

#define BUFSIZE 64
#include "cirbuf.h"

static cirbuf_t tx;

void isr_usart2(void)
{
  uint8_t c;
  if (USART2->ISR & USART_ISR_TXE) {
    if (!_cirbuf_pop(&tx, &c))
      USART2->TDR = c;
    else 
      USART2->CR1 &= ~USART_CR1_TXEIE;
  } 
}

void uart2_init(void)
{
  _cirbuf_init(&tx);

  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  USART2->CR3 = USART_CR3_OVRDIS;
  USART2->BRR = 0x0341;
  USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

  NVIC_EnableIRQ(USART2_IRQn);
}

void uart2_send_char(uint8_t c)
{
  _cirbuf_push(&tx, c);
  USART2->CR1 |= USART_CR1_TXEIE;
}

void uart2_print(uint8_t *s)
{
  uint8_t *p = s;
  while (*p)
    _cirbuf_push(&tx, *p++);
  USART2->CR1 |= USART_CR1_TXEIE;
}

/*
void uart2_int(uint8_t i)
{
uint8_t v;
v = 0;
while (i > 100) {
  v++;
  i -= 100;
}
uart2_send('0' + v);
v = 0;
while (i > 10) {
  v++;
  i -= 10;
}
uart2_send('0' + v);
uart2_send('0' + i);
}
*/
