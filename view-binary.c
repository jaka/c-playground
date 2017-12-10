#include <stdio.h>
#include <stdlib.h>
#include <string.h>     /* strcat */

void byte_to_binary(unsigned char x, char *b)
{
  unsigned char z;
  for (z = 128; z > 0; z >>= 1) {
    strcat(b, ((x & z) == z) ? "1" : "0");
  }
}

int main(int argc, char *argv[])
{
  FILE* fp;
  char code;
  size_t n = 0;
  static char b[9];
  char c, pc;

  if (argc > 1) {
    printf("Filename %s\n", argv[1]);
    fp = fopen(argv[1], "r");

    if (fp == NULL) {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
    }

    while ((c = fgetc(fp)) != EOF)
    {
      b[0] = '\0';
      byte_to_binary((unsigned char)c, b);
      pc = c;
      if (pc < 32)
        pc = '.';
      printf("C: %c: %03d: %s\n", pc, c, b);
    }
    fclose(fp);
  }
}
