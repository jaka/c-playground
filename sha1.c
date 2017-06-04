#include <stdio.h>
#include <stdint.h>
#include <string.h>

static void memdump(const char *s, const int len)
{
  int i, j = len, k, l = 0;
  char c;

  while (j) {
    j /= 16;
    l++;
  }
  
  printf("Displaying: %d bytes\n\n", len);
  
  printf("%*s  ", l, "");
  for (i = 0; i < 16;) {
    printf("%2X", i++);
   if (i % 4 == 0) 
     printf(" ");
  }
  for (i = 0; i < len;) {
    printf("\n%0*X: ", l, i);
    for (j = 0; j < 16; i++, j++) {
      if (i < len)
         printf("%02hhx", s[i]);
      else
         printf("  ");
      if (j % 4 == 3) 
        printf(" ");
    }
    i -= j;
    for (k = 0; k < j; k++) {
      c = s[i++];
      if (k % 8 == 0)
        printf(" ");
      if (c < 32 || c > 126) c = '.';  
        printf("%c", c);
    }
  }
  printf("\n");
}

#define SHA1_K0 0x5a827999
#define SHA1_K20 0x6ed9eba1
#define SHA1_K40 0x8f1bbcdc
#define SHA1_K60 0xca62c1d6

#define SHA1_A 0x67452301
#define SHA1_B 0xefcdab89
#define SHA1_C 0x98badcfe
#define SHA1_D 0x10325476
#define SHA1_E 0xc3d2e1f0

typedef struct {
  uint32_t a;
  uint32_t b;
  uint32_t c;
  uint32_t d;
  uint32_t e;
  uint32_t t;
} sha1_state_t;

typedef union {
  unsigned char c[64];
  uint32_t w[16];
} sha1_msg_t;

static uint32_t swap_uint32(uint32_t val)
{
  val = ((val << 8) & 0xff00ff00) | ((val >> 8) & 0xff00ff); 
  return (val << 16) | (val >> 16);
}

static uint64_t swap_uint64(uint64_t val)
{
  val = ((val << 8) & 0xff00ff00ff00ff00ULL) | ((val >> 8) & 0x00ff00ff00ff00ffULL);
  val = ((val << 16) & 0xffff0000ffff0000ULL) | ((val >> 16) & 0x0000ffff0000ffffULL);
  return (val << 32) | (val >> 32);
}

static uint32_t rotl(uint32_t i, uint32_t bits)
{
  return (i << bits) | (i >> (32 - bits));
}

int main(int argc, char **argv)
{
  unsigned int i;
  sha1_msg_t msg;
  sha1_state_t s;
  uint32_t t;
  size_t len;
  
  uint64_t BE_len;
    
  char *input = "abc";
  
  len = strlen(input);
  memcpy(msg.c, input, len);
  memset(msg.c + len, 0, 56 - len);
  *(msg.c + len) = 0x80;

  BE_len = swap_uint64(8 * len);
  memcpy(msg.c + 56, (unsigned char *)&BE_len, 8);

  memdump(msg.c, sizeof(msg.c));
  
  s.a = SHA1_A;
  s.b = SHA1_B;
  s.c = SHA1_C;
  s.d = SHA1_D;
  s.e = SHA1_E;

  for (i = 0; i < 80; i++) {

    if (i >= 16) {
      t = msg.w[(i+13) & 0xf] ^ msg.w[(i+8) & 0xf] ^ msg.w[(i+2) & 0xf] ^ msg.w[i & 0xf];
      msg.w[i & 0xf] = rotl(t, 1);
    }
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    else {
      msg.w[i] = swap_uint32(msg.w[i]);
    }
#endif
    
    if (i < 20)
      t = ((s.b & s.c) | ((~s.b) & s.d)) + SHA1_K0;
    else if (i < 40)
      t = (s.b ^ s.c ^ s.d) + SHA1_K20;
    else if (i < 60)
      t = ((s.b & s.c) | (s.d & (s.b | s.c))) + SHA1_K40;
    else
      t = (s.b ^ s.c ^ s.d) + SHA1_K60;

    t += rotl(s.a, 5) + s.e + msg.w[i & 0xf];

    s.e = s.d;
    s.d = s.c;
    s.c = rotl(s.b, 30);
    s.b = s.a;
    s.a = t;

    /*
    printf("%u A %08x B %08x C %08x D %08x E %08x\n", i, s.a, s.b, s.c, s.d, s.e);
    */
  }

  s.a += SHA1_A;
  s.b += SHA1_B;
  s.c += SHA1_C;
  s.d += SHA1_D;
  s.e += SHA1_E;
  
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  for (i = 0; i < 5; i++) {
    *((uint32_t *)&s + i) = swap_uint32(*((uint32_t *)&s + i));
  }
#endif

  for (i = 0; i < 20; i++) {
    printf("%02x", *((unsigned char *)&s + i));
  }
  printf("\n");
	return 0;
}
