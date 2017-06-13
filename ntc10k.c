#include <stdio.h>
#include <math.h>

struct lookuptable {
  unsigned char i;
  unsigned char o;
};

struct f_lookuptable {
  float i;
  float o;
};

const struct lookuptable ntk10k[] =
{
  { 0, 255},
  { 19, 255},
  { 23, 235},
  { 30, 216},
  { 40, 196},
  { 54, 175},
  { 71, 155},
  { 93, 135},
  { 118, 116},
  { 132, 105},
  { 146, 96},
  { 174, 75},
  { 199, 56}, /* 0 °C */
  { 219, 37},
  { 234, 16},
  { 241, 0},
  { 255, 0},
};

const struct f_lookuptable r_ntc10k[] = {
 { -40, 2000000 }, /* FAKE */
 { -30, 180100 },
 { -20, 98320 },
 { -10, 55790 },
 { 0, 32770 },
 { 10, 19930 },
 { 20, 12500 },
 { 25, 10000 },
 { 30, 8055 },
 { 40, 5323 },
 { 50, 3599 },
 { 60, 2486 },
 { 70, 1753 },
 { 80, 1258 },
 { 90, 919 },
 { 100, 682 },
 { 110, 513 },
 { 120, 392 },
 { 130, 303 },
 { 300, 0 }, /* FAKE */
};

static unsigned int f_transform(void)
{
  struct f_lookuptable *lt, *lt1, *lt2;
  double adc, adc4, o, t, k;
  const double r = 2 * 4700;
  int a, u;
  
  lt = (struct f_lookuptable *)&r_ntc10k[0];
  while (lt->o) {
    adc = 1024 * lt->o / (lt->o + r);
    adc4 = 4 * round(adc / 4);
    o = (adc4 * r) / (1024 - adc4);
    lt2 = (struct f_lookuptable *)&r_ntc10k[0];
    while (lt2->o > o)
      lt2++;
    lt1 = lt2 - 1;
      k = (lt1->i - lt2->i) / (lt1->o - lt2->o);
      t = lt2->i + k * (o - lt2->o);
      //printf("%f %f %f %f %f %f\n", lt1->o, o, lt2->o, lt1->i, lt2->i, t);
      a = (int)round(adc / 4); 
      u = (int)(t * 2) + 56;
      printf("%i, %f -> %f, %i\n", a, o, t, u);
    lt++;
   }

  return 0;
}

int main(int argc, char **argv)
{
  unsigned char adc_1 = 191;
  unsigned char adc_2 = 63;
  unsigned char c, n, s;
  struct lookuptable *lt1, *lt2;
  double d;
  
  f_transform();

  c = adc_1 - adc_2;
  lt2 = (struct lookuptable *)&ntk10k[0];
  while (c > lt2->i)
    lt2++;
  lt1 = lt2 - 1;
  n = lt1->o - lt2->o;
  c -= lt1->i;
  s = lt1->o - (c * n)/(lt2->i - lt1->i);
  
  d = (s - 56) / 2.0;
  printf("%hhu %f\n", s, d);

  return 0;
}
