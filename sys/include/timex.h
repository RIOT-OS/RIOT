#ifndef __TIMEX_H
#define __TIMEX_H 

#include <stdint.h>

typedef struct timex_t {
    uint32_t seconds;
    uint32_t nanoseconds;
} timex_t;

timex_t timex_add(const timex_t a, const timex_t b); 

#endif /* __TIMEX_H */
