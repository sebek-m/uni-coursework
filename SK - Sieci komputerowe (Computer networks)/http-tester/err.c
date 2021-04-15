#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <netdb.h>
#include "err.h"

void syserr(const char *fmt, ...)
{
  va_list fmt_args;
  int err;

  fprintf(stderr, "ERROR: ");
  err = errno;

  va_start(fmt_args, fmt);
  vfprintf(stderr, fmt, fmt_args);
  va_end(fmt_args);
  fprintf(stderr, " (%d; %s)\n", err, strerror(err));
  exit(EXIT_FAILURE);
}

void fatal(const char *fmt, ...)
{
  va_list fmt_args;

  fprintf(stderr, "ERROR: ");
  va_start(fmt_args, fmt);
  vfprintf(stderr, fmt, fmt_args);
  va_end(fmt_args);
  fprintf(stderr, "\n");
  exit(EXIT_FAILURE);
}

void resolve_error(int err) {
  switch (err) {
    case MEM_ERROR:
      syserr("memory allocation");
      break;
    case INVALID_ARGS_ERROR:
      fatal("Invalid args. Usage: ./testhttp_raw connection_address:port cookie_file http_server");
      break;
    case FILE_OPEN_ERROR:
      syserr("opening file");
      break;
    case FILE_READ_ERROR:
      syserr("reading file");
      break;
    case FILE_CLOSE_ERROR:
      syserr("closing file");
      break;
    case SOCKET_ERROR:
      syserr("socket");
      break;
    case CONNECT_ERROR:
      syserr("connect");
      break;
    case WRITE_ERROR:
      syserr("write");
      break;
    case CLOSE_ERROR:
      syserr("closing socket");
      break;
    case READ_ERROR:
      syserr("read");
      break;
    case INVALID_RESPONSE:
      fatal("invalid response");
      break;
    default:
      // Errory z gettaddrinfo
      if (err > 0) {
        err -= GAI_ERROR_BASE;
        syserr("getaddrinfo: %s", gai_strerror(err));
      }
      break;
  }
}