#ifndef MSP430_TYPES_H
#define MSP430_TYPES_H

#include <sys/_types.h>

/** defining signed type for size_t */
typedef _ssize_t ssize_t;

typedef unsigned long time_t;

struct timespec {
    time_t  tv_sec;   /* Seconds */
    long  tv_nsec;    /* Nanoseconds */
};

/* TODO: remove once msp430 libc supports struct timeval */
struct timeval {
    time_t tv_sec;
    time_t tv_usec;
};

#endif /* MSP430_TYPES_H */
