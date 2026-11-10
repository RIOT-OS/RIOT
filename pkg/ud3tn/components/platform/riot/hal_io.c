// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
/*
 * hal_io.c
 *
 * Description: contains the POSIX implementation of the hardware
 * abstraction layer interface for input/output functionality
 *
 */

#include "platform/hal_time.h"
#include "platform/hal_semaphore.h"

#include "ud3tn/result.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

static Semaphore_t log_io_semph;

enum ud3tn_result hal_io_init(void)
{
    log_io_semph = hal_semaphore_init_binary();
    hal_semaphore_release(log_io_semph);
    return UD3TN_OK;
}

int hal_io_message_printf(const char *format, ...)
{
    int rc;
    va_list v;

    va_start(v, format);
    rc = vfprintf(stderr, format, v);
    va_end(v);
    return rc;
}

static inline const char *get_log_level_name(const uint8_t level)
{
    switch (level) {
    case 1:
        return "ERROR";
    case 2:
        return "WARNING";
    case 3:
        return "INFO";
    default:
        return "DEBUG";
    }
}

int hal_io_log_printf(const int level, const char *const file, const int line,
                      const char *const format, ...)
{
    int rc;
    va_list v;

    hal_semaphore_take_blocking(log_io_semph);
    hal_time_print_log_time_string();
    fprintf(stderr, "[%s] ", get_log_level_name(level));
    va_start(v, format);
    rc = vfprintf(stderr, format, v);
    va_end(v);
    fprintf(stderr, " [%s:%d]\n", file, line);
    fflush(stderr);
    hal_semaphore_release(log_io_semph);
    return rc;
}

void hal_io_log_perror(int level, const char *component, const char *file,
                       int line, const char *message, int error)
{
    hal_semaphore_take_blocking(log_io_semph);
    hal_time_print_log_time_string();
    fprintf(
        stderr,
        "[%s] System error reported in %s - %s: %s [%s:%d]\n",
        get_log_level_name(level),
        component,
        message,
        strerror(error),
        file,
        line
        );
    fflush(stderr);
    hal_semaphore_release(log_io_semph);
}
