/* SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0 */
/**
 * @file
 * @brief Logging and message-output interface for the uD3TN HAL.
 */
#ifndef HAL_IO_H_INCLUDED
#define HAL_IO_H_INCLUDED

#include "ud3tn/result.h"

#include <stdint.h>

/** @brief Current maximum log level; messages at or below it are emitted. */
extern uint8_t LOG_LEVEL;

/**
 * @brief Emit a formatted log message when its level is enabled.
 * @param level Message severity, from 1 (error) through 4 (debug).
 * @param format printf-style format string.
 * @param ... Values referenced by @p format.
 */
#define LOGF_GENERIC(level, format, ...) \
do { \
    __typeof__(level) level_ = (level); \
    if (level_ <= LOG_LEVEL) { \
        hal_io_log_printf(level_, __FILE__, (int)__LINE__, format, __VA_ARGS__); \
    } \
} while (0)

/** @brief Log an error message. */
#define LOGF_ERROR(...) LOGF_GENERIC(1, __VA_ARGS__)
/** @brief Log a warning message. */
#define LOGF_WARN(...) LOGF_GENERIC(2, __VA_ARGS__)
/** @brief Log an informational message. */
#define LOGF_INFO(...) LOGF_GENERIC(3, __VA_ARGS__)

/** @brief Log a string as an error message. */
#define UD3TN_LOG_ERROR(message) LOGF_ERROR("%s", message)
/** @brief Log a string as a warning message. */
#define UD3TN_LOG_WARN(message) LOGF_WARN("%s", message)
/** @brief Log a string as an informational message. */
#define UD3TN_LOG_INFO(message) LOGF_INFO("%s", message)

/** @brief Compatibility alias for @ref UD3TN_LOG_ERROR. */
#define LOG_ERROR(message) UD3TN_LOG_ERROR(message)
/** @brief Compatibility alias for @ref UD3TN_LOG_WARN. */
#define LOG_WARN(message) UD3TN_LOG_WARN(message)
/** @brief Compatibility alias for @ref UD3TN_LOG_INFO. */
#define LOG_INFO(message) UD3TN_LOG_INFO(message)

#ifdef DEBUG
/** @brief Log a debug message; available when the build enables `DEBUG`. */
#define LOGF_DEBUG(...) LOGF_GENERIC(4, __VA_ARGS__)
/** @brief Log a string as a debug message; available in debug builds. */
#define UD3TN_LOG_DEBUG(message) LOGF_DEBUG("%s", message)
#else
/** @brief No-op when the build does not enable `DEBUG`. */
#define LOGF_DEBUG(...) ((void)0)
/** @brief No-op when the build does not enable `DEBUG`. */
#define UD3TN_LOG_DEBUG(message) ((void)0)
#endif
/** @brief Compatibility alias for @ref UD3TN_LOG_DEBUG. */
#define LOG_DEBUG(message) UD3TN_LOG_DEBUG(message)

/**
 * @brief Log an error with component and system-error context if enabled.
 * @param level Message severity.
 * @param component Name of the reporting component.
 * @param message Description of the failed operation.
 * @param error Error number passed to `strerror`.
 */
#define LOG_ERRNO_GENERIC(level, component, message, error) \
do { \
    __typeof__(level) level_ = (level); \
    if (level_ <= LOG_LEVEL) { \
        hal_io_log_perror(level_, component, __FILE__, (int)__LINE__, message, error); \
    } \
} while (0)

/** @brief Log a system error at error severity. */
#define LOG_ERRNO_ERROR(...) LOG_ERRNO_GENERIC(1, __VA_ARGS__)
/** @brief Log a system error at warning severity. */
#define LOG_ERRNO_WARN(...) LOG_ERRNO_GENERIC(2, __VA_ARGS__)
/** @brief Log a system error at informational severity. */
#define LOG_ERRNO_INFO(...) LOG_ERRNO_GENERIC(3, __VA_ARGS__)
/** @brief Compatibility alias for @ref LOG_ERRNO_WARN. */
#define LOG_ERRNO LOG_ERRNO_WARN

/** @brief Initialize the HAL output facilities. */
enum ud3tn_result hal_io_init(void);

/**
 * @brief Write a formatted, non-log message to the HAL output stream.
 * @param format printf-style format string.
 * @param ... Values referenced by @p format.
 * @return Number of characters written, or a negative value on error.
 */
int hal_io_message_printf(const char *format, ...);

/**
 * @brief Write a formatted log message with source location.
 * @param level Message severity.
 * @param file Source file containing the log call.
 * @param line Source line containing the log call.
 * @param format printf-style format string.
 * @param ... Values referenced by @p format.
 * @return Number of formatted message characters, or a negative value on error.
 */
int hal_io_log_printf(int level, const char *file, int line,
                      const char *format, ...);

/**
 * @brief Write a system error with component and source-location context.
 * @param level Message severity.
 * @param component Name of the reporting component.
 * @param file Source file containing the log call.
 * @param line Source line containing the log call.
 * @param message Description of the failed operation.
 * @param error Error number used to format the system-error description.
 */
void hal_io_log_perror(int level, const char *component, const char *file,
                       int line, const char *message, int error);

#endif /* HAL_IO_H_INCLUDED */
