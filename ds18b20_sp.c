#include <stdio.h>
#include <stdint.h>

int main(int argc, char **argv)
{
  unsigned char sp_sensor[2];
  unsigned short uT;
  short T;
  float temp;

  if (argc == 3) {
    if (sscanf(argv[1], "%02hhx", &sp_sensor[1]) != 1)
      return 1;
    if (sscanf(argv[2], "%02hhx", &sp_sensor[0]) != 1)
      return 1;
  }
  else {
    setvbuf(stdin, NULL, _IONBF, 0);
    if (fscanf(stdin, "%02hhx %02hhx", &sp_sensor[1], &sp_sensor[0]) != 2)
      return 1;
  }
//  sp_sensor[1] = 0x07;
//  sp_sensor[0] = 0xd0;
//  sp_sensor[1] = 0xff;
//  sp_sensor[0] = 0xf8;

  uT = (sp_sensor[1] << 8) | (sp_sensor[0] & 0xff);
  T = (short)uT; /* Force using leading bit as a sign. */
  temp = (float)T / 16;

  printf("T: %.3f (%02x%02x)\n", temp, sp_sensor[1], sp_sensor[0]);
}