#include <stdio.h>

struct lookuptable_float {
  float i;
  float o;
};

int main(int argc, char **argv)
{
  const struct lookuptable_float lup1[] =
  {
    { 180100, -30 },
    { 32770, 0 },
    { 19930, 10 },
    { 12500, 20 },
    { 10000, 25 },
    { 8055, 30 },
    { 5323, 40 },
    { 3599, 50 },
    { 1753, 70 },
    { 1258, 80 },
    { 821, 100 },
    { 513, 110 },
    { 0, 0}
  };
  
  struct lookuptable_float *p;
  
  float r1 = 4700;
  float r2 = 4700;
  
  for (p = lup1; p->i; p++) {
    float o = (255 * p->i) / (p->i + r1 + r2);
    printf("%f %i\n", o, (int)(2 * (p->o)) + 56);
  }
  
  return 0;
}
