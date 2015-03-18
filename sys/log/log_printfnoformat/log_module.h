#ifndef __LOG_FORMAT_H
#define __LOG_FORMAT_H 

#include <stdio.h>

static inline void _log(unsigned level, const char *format, ...) {
    (void)level;
    puts(format);
}

#endif /* __LOG_FORMAT_H */
