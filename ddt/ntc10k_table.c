#include <stdio.h>
#include <stdint.h>

struct lookuptable {
  uint8_t i;
  uint8_t o;
};

const struct lookuptable ntk10k[] =
{
  { 0, 255},
  { 19, 255},
  { 23, 235},
  { 30, 216},
  { 40, 196},
   { 47, 186 },
  { 54, 175 },
   { 62, 166 },
  { 71, 155},
   { 82, 145 },
  { 93, 135},
   { 105, 126 },
  { 118, 116},
  { 132, 105}, /* 25 */
  { 146, 95}, /* 20 */
   { 160, 86 },
  { 174, 75},
   { 186, 66 },
  { 199, 56}, /* 0 °C */
   { 209, 47 },
  { 219, 37},
   { 226, 28 },
  { 234, 16},
  { 241, 0},
  { 255, 0}
};

uint8_t get_temp(uint8_t c)
{
  uint8_t m;
  struct lookuptable *lt1, *lt2;

  lt2 = (struct lookuptable *)ntk10k;
  while (c > lt2->i)
    lt2++;
  lt1 = lt2 - 1;
  m = lt1->o - lt2->o;
  c -= lt1->i;
  return lt1->o - (c * m)/(lt2->i - lt1->i);
}

int main(void)
{
  uint8_t i;
  for (i = 1; i < 255; i++)
    printf("%d: %d\n", i, get_temp(i));
  return 0;
}
