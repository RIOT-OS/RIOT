/*
 * Copyright (C) 2015 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_log  Logging
 * @ingroup     core
 * @brief       Logging API
 *
 * ## Logging Modules
 *
 * The logging API can be implemented by several modules. Applications can
 * choose a module that best suits their purpose.
 *
 * If no module is selected explicitly, the built-in default
 * "log_printf" is used. It is a macro based implementation which
 * replaces logging calls with printf expressions.
 *
 * ## Severity Levels
 *
 * To differentiate between importance of log messages, there are three
 * severity levels:
 * - info
 * - warning
 * - error
 *
 * ### Disabling Severity Levels
 *
 * Logging implementations may be configured to discard log messages of any
 * kind.
 *
 * To discard logging levels explicitly, three macros may be set to `1`:
 * - LOG_DISCARD_INFO
 * - LOG_DISCARD_WARNING
 * - LOG_DISCARD_ERROR
 *
 * ## Implementation Details
 *
 * To save memory, implementations are free to only process the first
 * parameter.
 *
 * Implementations are not required to process every log message.
 * A networked remote logging implementation for example could decide
 * not to buffer messages that are logged while the network connection
 * is unavailable.
 *
 * All implementations are required to document how they process
 * messages if they deviate from expected behavior.
 *
 * ## Message Formatting
 *
 * All modules must prepend a severity level indicator to messages, so log
 * readers may differentiate between them.
 * The prefix is a single capital letter for the severity level followed by
 * a colon and a space character:
 * - info messages will be prefixed "I: "
 * - warning messages will be prefixed "W: "
 * - error messages will be prefixed "E: "
 *
 * @{
 *
 * @file
 * @brief       Declaration of the logging API.
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief macro controlling whether to discard info log messages
 *
 * When set to 0, info log messages will be processed, they will
 * be discarded otherwise.
 */
#ifndef LOG_DISCARD_INFO
#define LOG_DISCARD_INFO 0
#endif

/**
 * @brief macro controlling whether to discard warning log messages
 *
 * When set to 0, warning log messages will be processed, they will
 * be discarded otherwise.
 */
#ifndef LOG_DISCARD_WARNING
#define LOG_DISCARD_WARNING 0
#endif

/**
 * @brief macro controlling whether to discard error log messages
 *
 * When set to 0, error log messages will be processed, they will
 * be discarded otherwise.
 */
#ifndef LOG_DISCARD_ERROR
#define LOG_DISCARD_ERROR 0
#endif

/**
 * @brief log a string with informative but unimportant content
 *
 * Examples of messages which should be logged as info include the reception
 * of a packet, successful execution of some function, or activation of a
 * sleep mode
 *
 * @param[in] format    format string in printf syntax
 */
#ifndef MODULE_LOG_PRINTF
void log_info(const char *format, ...);
#else
#if LOG_DISCARD_INFO
#define log_info(...)
#else
#define log_info(...) printf("I: " __VA_ARGS__)
#endif
#endif

/**
 * @brief log a string indicating an exceptional event which could lead to
 * an error
 *
 * Examples of messages which should be logged as warning include dropping
 * of a network packet due to full transceiver queue, detection of an
 * invalid packet payload, or the discovery that the battery is empty
 *
 * @param[in] format    format string in printf syntax
 */
#ifndef MODULE_LOG_PRINTF
void log_warning(const char *format, ...);
#else
#if LOG_DISCARD_WARNING
#define log_warning(...)
#else
#define log_warning(...) printf("W: " __VA_ARGS__)
#endif
#endif

/**
 * @brief log a string describing an error
 *
 * Examples of messages which should be logged as error include unhandled
 * errors of a function execution, detection of a hardware failure,
 * inability to perform hardware initialization, or a stack overflow
 *
 * @param[in] format    format string in printf syntax
 */
#ifndef MODULE_LOG_PRINTF
void log_error(const char *format, ...);
#else
#if LOG_DISCARD_ERROR
#define log_error(...)
#else
#define log_error(...) printf("E: " __VA_ARGS__)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* LOG_H_ */
/** @} */
