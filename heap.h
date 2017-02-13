#ifndef HEAP_H
#define HEAP_H

struct heap_element_s {
  int value;
  void *data;
};

struct heap_s {
  struct heap_element_s *elements;
  int size;
  int last_index;
};

struct heap_s *heap_create(int);
void heap_destroy(struct heap_s *);

int heap_insert(struct heap_s *, int, void *);
int heap_remove(struct heap_s *);
void *heap_minimal(struct heap_s *);

#endif
