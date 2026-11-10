// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
#ifndef HAL_IO_H_INCLUDED
#define HAL_IO_H_INCLUDED

#include "ud3tn/result.h"

#include <stdint.h>

extern uint8_t LOG_LEVEL;

#define LOGF_GENERIC(level, format, ...) \
do { \
    __typeof__(level) level_ = (level); \
    if (level_ <= LOG_LEVEL) { \
        hal_io_log_printf(level_, __FILE__, (int)__LINE__, format, __VA_ARGS__); \
    } \
} while (0)

#define LOGF_ERROR(...) LOGF_GENERIC(1, __VA_ARGS__)
#define LOGF_WARN(...) LOGF_GENERIC(2, __VA_ARGS__)
#define LOGF_INFO(...) LOGF_GENERIC(3, __VA_ARGS__)

#define UD3TN_LOG_ERROR(message) LOGF_ERROR("%s", message)
#define UD3TN_LOG_WARN(message) LOGF_WARN("%s", message)
#define UD3TN_LOG_INFO(message) LOGF_INFO("%s", message)

#define LOG_ERROR(message) UD3TN_LOG_ERROR(message)
#define LOG_WARN(message) UD3TN_LOG_WARN(message)
#define LOG_INFO(message) UD3TN_LOG_INFO(message)

#ifdef DEBUG
#define LOGF_DEBUG(...) LOGF_GENERIC(4, __VA_ARGS__)
#define UD3TN_LOG_DEBUG(message) LOGF_DEBUG("%s", message)
#else
#define LOGF_DEBUG(...) ((void)0)
#define UD3TN_LOG_DEBUG(message) ((void)0)
#endif
#define LOG_DEBUG(message) UD3TN_LOG_DEBUG(message)

#define LOG_ERRNO_GENERIC(level, component, message, error) \
do { \
    __typeof__(level) level_ = (level); \
    if (level_ <= LOG_LEVEL) { \
        hal_io_log_perror(level_, component, __FILE__, (int)__LINE__, message, error); \
    } \
} while (0)

#define LOG_ERRNO_ERROR(...) LOG_ERRNO_GENERIC(1, __VA_ARGS__)
#define LOG_ERRNO_WARN(...) LOG_ERRNO_GENERIC(2, __VA_ARGS__)
#define LOG_ERRNO_INFO(...) LOG_ERRNO_GENERIC(3, __VA_ARGS__)
#define LOG_ERRNO LOG_ERRNO_WARN

enum ud3tn_result hal_io_init(void);
int hal_io_message_printf(const char *format, ...);
int hal_io_log_printf(int level, const char *file, int line,
                      const char *format, ...);
void hal_io_log_perror(int level, const char *component, const char *file,
                       int line, const char *message, int error);

#endif /* HAL_IO_H_INCLUDED */
