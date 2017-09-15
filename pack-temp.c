#include <stdio.h>
#include <stdint.h>

typedef union {
  unsigned short s;
  unsigned char buf[2];
} temp_t;

static uint8_t _pack_temp(uint8_t *sp)
{
  uint8_t v;

  if (*(sp + 1) & 0x80) {
    *(sp + 1) ^= 0xff;
    *sp ^= 0xff;
    (*sp)++;
    if (!*sp)
      (*(sp + 1))++;
    return (*(sp + 1) << 5) | (*sp >> 3);
  }
  return 0;
}

int main(void)
{
  temp_t temp;
  uint8_t rv;

  temp.s = 0xff00;

  printf("%x %x\n", temp.buf[1], temp.buf[0]);
  rv = _pack_temp(&(temp.buf[0]));
  printf("%hhu\n", rv);
}
