#ifndef _UTILS_
#define _UTILS_

#include <stdio.h>

/*
  Zwiększa pamięć zaalokowaną na napis o ustalony mnożnik.
*/
int increase_mem(size_t* string_size, char** string, size_t appendix);

#endif