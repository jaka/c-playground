void assert(uint8_t a, uint8_t b)
{
  if (a == b) 
    printf("OK %d\n", a);
  else
    printf("FAIL %d != %d\n", a, b);
}
