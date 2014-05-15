#ifndef __MALLOC_H
#define __MALLOC_H

#include <stdlib.h>

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void *calloc(int size, size_t cnt);
void free(void *ptr);

#endif /* __MALLOC_H */
