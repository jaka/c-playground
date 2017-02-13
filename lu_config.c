
/* Released under the 2-Clause BSD License. */

#include <string.h>
#include "lu_config.h"

static char *cfgarg = NULL;

int lu_parseopt(const lu_args *args, const lu_cfgopt *cfgopts) {

  const char *p;
  int i, j = 0, error = 0;
  const lu_cfgopt *cfgopt = NULL;

  for (i = 1; i < args->argc; i++) {
    p = *(args->argv + i);
    if (*p != '-') {
      j++;
      continue;
    }
    p++;
    if (!*p)
      continue;
    for (cfgopt = cfgopts; cfgopt->c; cfgopt++) {
      if (cfgopt->c == *p) {
        p++;
        if (*p)
          cfgopt->cb(*(args->argv + i) + 2);
        else if ((i + 1) < args->argc) {
          i++;
          cfgopt->cb(*(args->argv + i));
        }
        else
          error = -1;
        break;
      }
      if (cfgopt->c > *p)
        break;
    }
  }
  if ( error == 0 )
    error = j;

  return error;
}

char lu_getopt(const lu_args *args, const char *opts) {

  char *p, *q, *r;
  static int i = 1;

  if (i == args->argc) {
    i = 1;
    return EOF;
  }

  cfgarg = NULL;

  p = *(args->argv + i);
  i++;
  if (*p != '-') {
    cfgarg = p;
    return '_';
  }
  p++;
  if (!*p)
    return '?';

  q = p;
  r = strchr(opts, *q);
  if (!r || *q == ':')
    return '?';

  r++;
  if (*r == ':') {
    p++;
    if (*p)
      cfgarg = p;
    else if (i < args->argc) {
      cfgarg = *(args->argv + i);
      i++;
    }
    else
      return ':';
  }

  return *q;
}

const char *lu_getarg(void) {

  return cfgarg;
}
