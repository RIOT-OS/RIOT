#ifndef SLEEPMEM_H
#define SLEEPMEM_H

#include <unistd.h>

extern size_t sleepmem_size;

void sleepmem_init(void);
size_t sleepmem_put(void *src, size_t n);
size_t sleepmem_get(void *target, size_t n);

#endif /* SLEEPMEM_H */
