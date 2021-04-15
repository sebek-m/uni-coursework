#ifndef _MSG_BUILDER_
#define _MSG_BUILDER_

#include <stdio.h>

#include "parser.h"

/*
  Na podstawie Wypełnia napis, na który wskazuje message, żądaniem HTTP.
*/
int build_message(arg_info* info, char** message, size_t* message_length);

#endif