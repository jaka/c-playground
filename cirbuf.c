#include <stdio.h>

typedef struct {
  char *buffer;
  unsigned int head;
  unsigned int tail;
  unsigned int size;
} lu_cirbuf_t;

int cirbuf_push(lu_cirbuf_t *c, char data) {

  int next = (c->head + 1) & (c->size - 1);
  if (next == c->tail)
    return -1;
  c->buffer[c->head] = data;
  c->head = next;
  return 0;
}

int cirbuf_pop(lu_cirbuf_t *c, char* data) {

  if (c->head == c->tail)
    return -1;
  *data = c->buffer[c->tail];
  c->tail = (c->tail + 1) & (c->size - 1);
  return 0;
}

int cirbuf_init(lu_cirbuf_t *c, char *buffer, const unsigned int size) {

#ifdef CIRBUF_CHECK_SIZE
  unsigned int s = size;
  while (s % 2 == 0) s /= 2;
  printf("%d\n", s);
  if (s != 1)
    return -1;
#endif

  c->buffer = buffer;
  c->head = 0;
  c->tail = 0;
  c->size = size;

  return 0;
}

#define BUFFER_SIZE 16

void cirbuf_demo(void) {

  int rv;
  char p;

  /* Create memory segment */
  char buffer[BUFFER_SIZE];

  /* Create a circular buffer */
  lu_cirbuf_t demo_cirbuf;

  /* Initialize our circular buffer */
  rv = cirbuf_init(&demo_cirbuf, buffer, BUFFER_SIZE);
  if (rv < 0)
    return;

  /* Push a letter A */
  cirbuf_push(&demo_cirbuf, 'A');

  /* Pop a character */
  cirbuf_pop(&demo_cirbuf, &p);

  printf("Got %c\n", p);

}

int main(int argc, char **argv) {

  cirbuf_demo();
  return 0;
}
