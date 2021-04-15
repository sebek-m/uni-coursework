#ifndef _CONNECTOR_
#define _CONNECTOR_

#include "parser.h"

/*
  Łączy się z podanym przy wywołaniu serwerem, wysyła do niego żądanie HTTP (message),
  a następnie zapisuje otrzymaną odpowiedź w napisie, na który wskazuje response.
*/
int send_and_receive_msg(arg_info* info, size_t message_len, char* message,
                         size_t* response_len, char** response);

#endif