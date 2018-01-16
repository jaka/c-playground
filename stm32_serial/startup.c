#include <stdint.h>

/* STARTUP */
extern int main(void);

extern uint32_t *__bss_begin, *__bss_end;
extern uint32_t *__data_begin, *__data_end;
extern uint32_t *__datai_begin, *__datai_end;

__attribute__ ((section(".startup"), optimize("-O2"))) void __startup(void) 
{

  uint32_t *bss = __bss_begin;
  uint32_t *data = __data_begin;
  uint32_t *datai = __datai_begin;

  while (bss < __bss_end) {
    *bss = 0;
    bss++;
  }

  while (datai < __datai_end) {
    *data = *datai;
    data++;
    datai++;
  }

  main();
}

/* weak ISR */
__attribute__ ((section(".isr"), optimize("-O3"))) void __nothing(void) {}

void isr_wwdg(void) __attribute__ ((weak, alias ("__nothing")));
void isr_pvd_vddio2(void) __attribute__ ((weak, alias ("__nothing")));
void isr_usart2(void) __attribute__ ((weak, alias ("__nothing")));

/* VECTOR TABLE */

struct __vector_table {
  uint32_t *start_sp;
  void (*sirq[15])(void);
  void (*pirq[32])(void);
};

#define __zero 0x0
extern uint32_t __stack_end;

struct __vector_table vector_table __attribute__ ((section(".vectors"))) =
{
  &__stack_end,
  {
    __startup,
  },
  {
    isr_wwdg,
    isr_pvd_vddio2,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    __zero,
    isr_usart2
  }
};


