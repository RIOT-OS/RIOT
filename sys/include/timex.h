#ifndef __TIMEX_H
#define __TIMEX_H 

#include <stdint.h>

typedef struct timex_t {
    uint32_t seconds;
    uint32_t nanoseconds;
} timex_t;

/* a+b */
timex_t timex_add(const timex_t a, const timex_t b); 

/* a-b*/
timex_t timex_sub(const timex_t a, const timex_t b);

timex_t timex_set(uint32_t seconds, uint32_t nanoseconds);

#endif /* __TIMEX_H */
