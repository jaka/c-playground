#include <stdio.h>
#include <stdint.h>

#include "assert.c"

#define ZERO_POINT 56

struct {
  uint8_t popravek_A;
  uint8_t popravek_B;
} config;

void _apply_offset(uint8_t j, uint8_t *temp)
{
  uint8_t d;
  uint8_t offset = j == 0 ? config.popravek_A : config.popravek_B;
  if (offset < ZERO_POINT) {
    d = ZERO_POINT - offset;
    if (*temp >= d)
      *temp -= d;
    else
      *temp = 0;
  }
  else {
    d = offset - ZERO_POINT;
    if (*temp <= (255 - d))
      *temp += d;
    else
      *temp = 255;
  }
}

int main(void)
{
  uint8_t temp;

  temp = 100;
  config.popravek_A = ZERO_POINT;
  config.popravek_B = ZERO_POINT;
  _apply_offset(0, &temp);
  assert(temp, 100);

  config.popravek_A = ZERO_POINT - 2;
  _apply_offset(0, &temp);
  assert(temp, 98);

  config.popravek_B = ZERO_POINT + 1;
  _apply_offset(1, &temp);
  assert(temp, 99);

  temp = 253;
  config.popravek_B = ZERO_POINT + 1;
  _apply_offset(1, &temp);
  assert(temp, 254);
  _apply_offset(1, &temp);
  assert(temp, 255);
  _apply_offset(1, &temp);
  assert(temp, 255);

  temp = 5;
  config.popravek_A = ZERO_POINT - 3;
  _apply_offset(0, &temp);
  assert(temp, 2);
  _apply_offset(0, &temp);
  assert(temp, 0);

  return 0;
}
