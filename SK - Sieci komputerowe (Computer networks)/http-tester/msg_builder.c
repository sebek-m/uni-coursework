#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "msg_builder.h"

int build_message(arg_info* info, char** message, size_t* message_length) {
  size_t cookie_line_len;

  if (info->cookies_len > 0)
    cookie_line_len = strlen("Cookie: ") + info->cookies_len + strlen("\r\n");
  else
    cookie_line_len = 0;
  
  *message_length = strlen("GET ") + info->http_rsc_len + strlen(" HTTP/1.1\r\n") +
                    strlen("Host: ") + info->http_addr_len + strlen("\r\n") +
                    cookie_line_len +
                    strlen("Coonection: close\r\n") +
                    strlen("\r\n");

  *message = (char*)malloc(*message_length + 1);
  if (*message == NULL)
    return MEM_ERROR;

  if (cookie_line_len > 0) {
    sprintf(*message, "GET %s HTTP/1.1\r\n"
                      "Host: %s\r\n"
                      "Cookie: %s\r\n"
                      "Connection: close\r\n"
                      "\r\n",
                      info->http_resource, info->http_address, info->cookies);
  }
  else {
    sprintf(*message, "GET %s HTTP/1.1\r\n"
                      "Host: %s\r\n"
                      "Connection: close\r\n"
                      "\r\n",
                      info->http_resource, info->http_address);
  }

  return SUCCESS;
}