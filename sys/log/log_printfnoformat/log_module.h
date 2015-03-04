#ifndef __LOG_FORMAT_H
#define __LOG_FORMAT_H 

#include <stdio.h>

static void _log(unsigned level, const char *format, ...) {
    (void)level;
    puts(format);
}

#define log_error(...) LOG(LOG_ERROR, "E: " __VA_ARGS__)
#define log_warning(...) LOG(LOG_WARNING, "W: " __VA_ARGS__)
#define log_info(...) LOG(LOG_INFO, "I: " __VA_ARGS__)
#define log_debug(...) LOG(LOG_DEBUG, "D: " __VA_ARGS__)

#endif /* __LOG_FORMAT_H */
