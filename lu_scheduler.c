
/* Released under the 2-Clause BSD License. */

#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include "lu_scheduler.h"

/* Struct:   lu_tskn */
/* Variable: tskn    */
/* Struct:   lu_fdn */
/* Variable: fdn    */

typedef struct {
  lu_tskn_t *tasks;
  int size;
  int last_index; /* Index to the last occupied cell. */
} lu_tskh_t;

/* ***************** */

lu_tskh_t *tsk_heap = NULL;
lu_fdn_t *fdn_first = NULL;
lu_fdn_t *fdn_last = NULL;

/* ***************** */

static inline int lu_tcmp(struct timeval *a, struct timeval *b) {
  /* Compare two timevals and return 1 whether a before b otherwise 0. */

  return (a->tv_sec < b->tv_sec || (a->tv_sec == b->tv_sec && a->tv_usec < b->tv_usec));
}

static int lu_tsk_up(int index) {

  int prev;
  lu_tskn_t task;

  for (task = *(tsk_heap->tasks + index); index > 0; index = prev) {
    prev = (index - 1) / 2;
    if (lu_tcmp(&(tsk_heap->tasks + prev)->time, &task.time) == 1)
      break;
    (tsk_heap->tasks + index)->time = (tsk_heap->tasks + prev)->time;
    (tsk_heap->tasks + index)->task = (tsk_heap->tasks + prev)->task;  
  }
  *(tsk_heap->tasks + index) = task;

  return index;
}

static int lu_tsk_down(int index) {

  int next;
  lu_tskn_t task;

  for (task = *(tsk_heap->tasks + index); 2 * index < tsk_heap->last_index; index = next) {
    next = 2 * index + 1;
    if (next < tsk_heap->last_index && 
        lu_tcmp(&(tsk_heap->tasks + next + 1)->time, &(tsk_heap->tasks + next)->time) == 1)
      next++;
    if (lu_tcmp(&task.time, &(tsk_heap->tasks + next)->time) == 1)
      break;
    (tsk_heap->tasks + index)->time = (tsk_heap->tasks + next)->time;
    (tsk_heap->tasks + index)->task = (tsk_heap->tasks + next)->task;
  }
  *(tsk_heap->tasks + index) = task;

  return index;
}

static int lu_tsk_push(struct timeval time, lu_tsk_t task) {

  if (tsk_heap->last_index == tsk_heap->size)
    return -1;
  /* Append to the end of array. */
  tsk_heap->last_index++;
  (tsk_heap->tasks + tsk_heap->last_index)->time = time;
  (tsk_heap->tasks + tsk_heap->last_index)->task = task; 

  return lu_tsk_up(tsk_heap->last_index);
}

static int lu_tsk_del(int i) {

  if (i > tsk_heap->last_index)
    return -1;

  (tsk_heap->tasks + i)->time = (tsk_heap->tasks + tsk_heap->last_index)->time;
  (tsk_heap->tasks + i)->task = (tsk_heap->tasks + tsk_heap->last_index)->task;
  tsk_heap->last_index--;

  return lu_tsk_down(i);
}

static int lu_timeout(struct timeval *timeout) {

  int rv;
  struct timeval a, b, c;
  struct timespec cur_time;

  if (tsk_heap->last_index < 0)
    return -1;

  a = tsk_heap->tasks->time;

  if (clock_gettime(CLOCK_MONOTONIC, &cur_time) < 0)
    return -1;
  b.tv_sec = cur_time.tv_sec;
  b.tv_usec = (cur_time.tv_nsec / 1000) + 1;

  /* Set rv if a is in the past (before b), otherwise calculate the difference c = a - b. */

  rv = 0;

  if (a.tv_sec < b.tv_sec)
    rv = 1;
  else if (a.tv_sec == b.tv_sec) {
    if (a.tv_usec <= b.tv_usec)
      rv = 1;
    else {
      c.tv_sec = 0;
      c.tv_usec = a.tv_usec - b.tv_usec;
    }
  }
  else {
    c.tv_sec = a.tv_sec - b.tv_sec; /* This is always more than 1. */
    if (a.tv_usec < b.tv_usec) {
      c.tv_sec--;
      c.tv_usec = 1000000 + a.tv_usec - b.tv_usec;
    }
    else
      c.tv_usec = a.tv_usec - b.tv_usec;
  }

  *timeout = c;
  return rv;
}

static int lu_tskrun(void) {

  lu_tskn_t tskn;

  if (tsk_heap->last_index < 0) {
    /* This should never happen. */
    return -1;
  }

  tskn = *tsk_heap->tasks; 
  lu_tsk_del(0);
  tskn.task.cb(&tskn);

  return 0;
}

/* ***************** */

int lu_init(int size) {

  if (size < 0)
    return -1;

  if (tsk_heap != NULL)
    return -1;

  tsk_heap = malloc(sizeof(lu_tskh_t));
  if (tsk_heap == NULL)
    return -1;

  tsk_heap->tasks = malloc(sizeof(lu_tskn_t) * size);
  if (tsk_heap->tasks == NULL) {
    free(tsk_heap);
    return -1;
  }

  tsk_heap->size = size;
  tsk_heap->last_index = -1;

  return 0;
}

int lu_task_insert(lu_tsk_t task, time_t s) {

  struct timeval time;
  struct timespec cur_time;

  if (clock_gettime(CLOCK_MONOTONIC, &cur_time) < 0)
    return -1;
  time.tv_sec = cur_time.tv_sec + s;
  time.tv_usec = (cur_time.tv_nsec / 1000) + 1;

  return lu_tsk_push(time, task);
}

int lu_task_insert2(void (*cb)(lu_tskn_t *), void *ctx, time_t s) {

  lu_tsk_t task;

  task.cb = cb;
  task.ctx = ctx;

  return lu_task_insert(task, s);
}

int lu_task_remove(void *ctx) {

  int i;
  
  if (tsk_heap->last_index == -1)
    return -1;

  if ((tsk_heap->tasks + tsk_heap->last_index)->task.ctx == ctx) {
    tsk_heap->last_index--;
    return 0;
  }

  for (i = 0; i < tsk_heap->last_index; i++) {
    if ((tsk_heap->tasks + i)->task.ctx == ctx)
      return lu_tsk_del(i);
  }

  return -1;
}

int lu_fd_add(lu_fd_t fdd, int fd, int options) {

  lu_fdn_t *new_fdn;

  new_fdn = malloc(sizeof(lu_fdn_t));
  if (!new_fdn)
    return -1;

  new_fdn->fd = fd;
  new_fdn->data = fdd;
  new_fdn->options = options;

  /* Put the node to the start. */
  new_fdn->prev = NULL;
  new_fdn->next = fdn_first;
  if (fdn_first)
    fdn_first->prev = new_fdn;
  else
    fdn_last = new_fdn;
  fdn_first = new_fdn;

  return 0;
}

int lu_fd_del(lu_fdn_t *fdn) {

  if (!fdn)
    return -1;

  if ( fdn->next )
    fdn->next->prev = fdn->prev;
  if ( fdn->prev )
    fdn->prev->next = fdn->next;
  if ( fdn_first == fdn )
    fdn_first = fdn->next;
  if ( fdn_last == fdn )
    fdn_last = fdn->prev;

  free(fdn);

  return 0;
}

int lu_loop(void) {

  struct timeval task_timeout;
  struct timeval *timeout;
  fd_set readfds;
  fd_set writefds;
  lu_fdn_t *fdn;
  int max_fd;
  int select_rv;
  int timeout_rv;

  if (!tsk_heap && !fdn_first)
    return -1;

  for (;;) {

    timeout_rv = lu_timeout(&task_timeout);
    switch (timeout_rv) {
      case -1:
        timeout = NULL;
        break;
      case 0:
        timeout = &task_timeout;
        break;
      case 1:
        lu_tskrun();
        continue;
    }
    
    max_fd = 0;    
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

#define SET_MAX_FD(fd) {if (max_fd < fd) max_fd = fd;}

    for (fdn = fdn_first; fdn != NULL; fdn = fdn->next) {
      if (fdn->options & LS_WRITE) {
        FD_SET(fdn->fd, &writefds);
        SET_MAX_FD(fdn->fd)
      }
      if (fdn->options & LS_READ) {
        FD_SET(fdn->fd, &readfds);
        SET_MAX_FD(fdn->fd)
      }
    }

#undef SET_MAX_FD

    errno = 0;
    select_rv = select(max_fd + 1, &readfds, &writefds, NULL, timeout);

    if (select_rv < -1) {
      if (errno == EINTR)
        continue;
    }
    else if (select_rv == 0) {
      /* Timeout detected, run scheduled task. */
      lu_tskrun();
      continue;
    }
    
    /* Find available fds. */
    for ( fdn = fdn_first; fdn != NULL; fdn = fdn->next ) {
      if (FD_ISSET(fdn->fd, &readfds)) {
        fdn->data.recv(fdn);
        select_rv--;
      }
      if (FD_ISSET(fdn->fd, &writefds)) {
        fdn->data.send(fdn);
        select_rv--;
      }
    }

  }

  return 0;
}
