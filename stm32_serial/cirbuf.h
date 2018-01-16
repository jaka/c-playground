#ifndef CIRBUF_H
#define CIRBUF_H

typedef struct {
  volatile uint8_t buffer[BUFSIZE];
  volatile uint8_t head;
  volatile uint8_t tail;
  uint8_t size;
} cirbuf_t;

static inline int8_t _cirbuf_push(cirbuf_t *c, uint8_t data)
{
  int next = (c->head + 1) & c->size;
  if (next == c->tail)
    return -1;
  c->buffer[c->head] = data;
  c->head = next;
  return 0;
}

static inline int8_t _cirbuf_pop(cirbuf_t *c, uint8_t *data)
{
  if (c->head == c->tail)
    return -1;
  *data = c->buffer[c->tail];
  c->tail = (c->tail + 1) & c->size;
  return 0;
}

#define _CIRBUF_MASK (BUFSIZE - 1)

static inline void _cirbuf_init(cirbuf_t *c)
{
  c->head = 0;
  c->tail = 0;
  c->size = _CIRBUF_MASK;
}

#endif
