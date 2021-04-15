/*
  Usunąłem wywołania exit. Zamiast tego, w radio-proxy ustawiany jest kod wyjścia na 1,
  by program mógł się normalnie zakończyć i zwolnić pamięć.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include "err.h"

extern int sys_nerr;

void syserr(const char *fmt, ...) {
  va_list fmt_args;
  int err;

  fprintf(stderr, "ERROR: ");
  err = errno;

  va_start(fmt_args, fmt);
  if (vfprintf(stderr, fmt, fmt_args) < 0) {
    fprintf(stderr, " (also error in syserr) ");
  }
  va_end(fmt_args);
  fprintf(stderr, " (%d; %s)\n", err, strerror(err));
}

void fatal(const char *fmt, ...) {
  va_list fmt_args;
  
  fprintf(stderr, "ERROR: ");

  va_start(fmt_args, fmt);
  if (vfprintf(stderr, fmt, fmt_args) < 0) {
    fprintf(stderr, " (also error in fatal) ");
  }
  va_end(fmt_args);

  fprintf(stderr, "\n");
}
