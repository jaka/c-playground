#include <stdio.h>
#include <stdint.h>

#define ZERO_POINT 56

static uint8_t _pack_temp(uint8_t *scratchpad)
{
  uint8_t v;

  if (*(scratchpad + 1) & 0x80) {
    *(scratchpad + 1) ^= 0xff;
    *scratchpad ^= 0xff;

    (*scratchpad)++;
    if (!*scratchpad)
      (*(scratchpad + 1))++;

    v = ZERO_POINT;
    v -= (*(scratchpad + 1) << 5) | (*scratchpad >> 3);
    if (v > ZERO_POINT)
      return 0;
  }
  else {
    v = (*(scratchpad + 1) << 5) | (*scratchpad >> 3);
    if (v > (255 - ZERO_POINT))
      return 255;
    v += ZERO_POINT;
  }
  return v;
}

void assert(uint8_t a, uint8_t b)
{
  if (a == b) 
    printf("OK %d\n", a);
  else
    printf("FAIL %d != %d\n", a, b);
}

int main(void)
{
  union {
    char scratchpad[2];
    uint16_t value;
  } temp;

  uint8_t response;

  temp.value = 0x07d0;
  response = _pack_temp(temp.scratchpad);
  assert(response, 255);

  temp.value = 0x0550;
  response = _pack_temp(temp.scratchpad);
  assert(response, ZERO_POINT + 2 * 85);

  temp.value = 0x0191;
  response = _pack_temp(temp.scratchpad);
  assert(response, ZERO_POINT + 2 * 25);

  temp.value = 0x0008;
  response = _pack_temp(temp.scratchpad);
  assert(response, ZERO_POINT + 1);

  temp.value = 0xfff8;
  response = _pack_temp(temp.scratchpad);
  assert(response, ZERO_POINT - 1);

  temp.value = 0xff5e;
  response = _pack_temp(temp.scratchpad);
  assert(response, ZERO_POINT - 10 * 2);

  temp.value = 0xfe6f;
  response = _pack_temp(temp.scratchpad);
  assert(response, ZERO_POINT - 25 * 2);

  temp.value = 0xfc90;
  response = _pack_temp(temp.scratchpad);
  assert(response, 0);
}
