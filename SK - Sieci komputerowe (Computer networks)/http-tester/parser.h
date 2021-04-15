#ifndef _PARSER_
#define _PARSER_

#include <stdio.h>

typedef struct struct_arg_info {
  char* connection_port;
  char* connection_address;

  size_t cookies_len;
  char* cookies;

  size_t http_addr_len;
  char* http_address;
  size_t http_rsc_len;
  char* http_resource;
} arg_info;

/*
  Inicjalizuje strukturę arg_info.
*/
void initialize(arg_info* info);

/*
  Wypełnia strukturę info danymi przeczytanymi z argumentów wywołania programu
*/
int read_cmd(arg_info* info, int argc, char* argv[]);

/*
  Czyści pamięć zaalokowaną na elementy struktury w trakcie parsowania
*/
void clear_info(arg_info* info);

#endif