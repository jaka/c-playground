
/* Released under the 2-Clause BSD License. */

#ifndef LU_SCHEDULER_H
#define LU_SCHEDULER_H

#include <sys/time.h>

enum fd_opts {
  LS_NONE = 0x0,
  LS_READ = 0x1,
  LS_WRITE = 0x2,
  LS_BOTH = 0x3
};

typedef struct lu_tskn lu_tskn_t;

typedef struct {
  void (*cb)(lu_tskn_t *);
  void *ctx;
} lu_tsk_t;

typedef struct lu_tskn {
  struct timeval time;
  lu_tsk_t task;
} lu_tskn_t;

typedef struct lu_fdn lu_fdn_t;

typedef struct {
  void (*recv)(lu_fdn_t *);
  void (*send)(lu_fdn_t *);
  void *ctx;
} lu_fd_t;

typedef struct lu_fdn {
  int fd;
  unsigned int options;
  lu_fd_t data;
  struct lu_fdn *next;
  struct lu_fdn *prev;
} lu_fdn_t;

int lu_init(int);
int lu_loop(void);

int lu_task_insert(lu_tsk_t, time_t);
int lu_task_insert2(void (*)(lu_tskn_t *), void *, time_t);
int lu_task_remove(void *);

int lu_fd_add(lu_fd_t, int, int);
int lu_fd_del(lu_fdn_t *);

#endif /* LU_SCHEDULER_H */
