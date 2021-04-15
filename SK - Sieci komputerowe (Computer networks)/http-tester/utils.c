#include <stdlib.h>

#include "err.h"
#include "utils.h"

int increase_mem(size_t* string_size, char** string, size_t appendix) {
  *string_size = 1 + appendix + (*string_size) * 3 / 2;

  *string = (char*)realloc(*string, *string_size);
  if (*string == NULL)
    return MEM_ERROR;

  return SUCCESS;
}