#include <unistd.h>
#include <stdlib.h>

#include "heap.h"

struct heap_s *heap_create(int size) {

  struct heap_s *heap;

  heap = malloc(sizeof(struct heap_element_s));
  heap->elements = malloc(sizeof(struct heap_element_s) * size);
  heap->size = size;
  heap->last_index = -1;

  return heap;
}

void heap_destroy(struct heap_s *heap) {

  if (heap && heap->elements) {
    free(heap->elements);
    heap->elements = NULL;
  }
  if (heap) {
    free(heap);
    heap = NULL;
  }
}

static int heap_promote_up(struct heap_s *heap, int index) {

  int prev;
  struct heap_element_s element;

  element = *(heap->elements + index);
  while (index > 0) {
    prev = (index - 1) / 2;
    if ((heap->elements + prev)->value < element.value)
      break;
    (heap->elements + index)->value = (heap->elements + prev)->value;
    (heap->elements + index)->data = (heap->elements + prev)->data;
    index = prev;  
  }
  *(heap->elements + index) = element;

  return index;
}

static int heap_promote_down(struct heap_s *heap, int index) {

  int next;
  struct heap_element_s element;

  element = *(heap->elements + index);
  while (2 * index < heap->last_index) {
    next = 2 * index + 1;
    if (next < heap->last_index && (heap->elements + next + 1)->value < (heap->elements + next)->value)
      next++;
    if ((heap->elements + next)->value > element.value)
      break;
    (heap->elements + index)->value = (heap->elements + next)->value;
    (heap->elements + index)->data = (heap->elements + next)->data;
    index = next;
  }
  *(heap->elements + index) = element;

  return index;
}

int heap_insert(struct heap_s *heap, int value, void *data) {

  if (heap->last_index == heap->size)
    return -1;
  /* Append to the end of array. */
  heap->last_index++;
  (heap->elements + heap->last_index)->value = value;
  (heap->elements + heap->last_index)->data = data; 

  return heap_promote_up(heap, heap->last_index);
}

int heap_remove(struct heap_s *heap) {

  if (heap->last_index == -1)
    return -1;
  heap->elements->value = (heap->elements + heap->last_index)->value;
  heap->elements->data = (heap->elements + heap->last_index)->data;
  heap->last_index--;

  return heap_promote_down(heap, 0);
}

void *heap_minimal(struct heap_s *heap) {

  if ( heap->last_index < 0 )
    return NULL;
  return (heap->elements)->data;
}
