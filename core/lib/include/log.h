/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       System logging header
 * @anchor      core_util_log
 *
 * This header offers a bunch of "LOG_*" functions that, with the default
 * implementation, just use printf, but honour a verbosity level.
 *
 * If you want a logging unit name to be prefixed to the logs, define LOG_UNIT
 * in the source file before including this header.
 *
 * If desired, it is possible to implement a log module which then will be used
 * instead the default printf-based implementation.  In order to do so, the log
 * module has to
 *
 * 1. provide "log_module.h"
 * 2. have a name starting with "log_" *or* depend on the pseudo-module LOG,
 * 3. implement log_write()
 *
 * See "sys/log/log_printfnoformat" for an example.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief defined log levels
 *
 * These are the logging levels a user can choose.
 * The idea is to set LOG_LEVEL to one of these values in the application's Makefile.
 * That will restrict output of log statements to those with equal or lower log level.
 *
 * The default log level is LOG_INFO, which will print every message.
 *
 * The log function calls of filtered messages will be optimized out at compile
 * time, so a lower log level might result in smaller code size.
 */
enum {
    LOG_NONE,       /**< Lowest log level, will output nothing */
    LOG_WTF,        /**< What a Terrible Failure log level, will print
                         only critical, non-recoverable errors that should never
                         ever happen. Turns the system off. */
    LOG_ERROR,      /**< Error log level, will print only critical,
                         non-recoverable errors like hardware initialization
                         failures */
    LOG_WARNING,    /**< Warning log level, will print warning messages for
                         temporary errors */
    LOG_INFO,       /**< Informational log level, will print purely
                         informational messages like successful system bootup,
                         network link state, ...*/
    LOG_DEBUG,      /**< Debug log level, printing developer stuff considered
                         too verbose for production use */
    LOG_ALL         /**< print everything */
};

#ifndef LOG_LEVEL
/**
 * @brief Default log level define
 */
#define LOG_LEVEL LOG_INFO
#endif

/**
 * @brief Log with/without unit.
 *
 * @note Internal use only, use @ref LOG() instead.
 */
#ifdef LOG_UNIT
#  define LOG_WRITE(level, fmt, ...) log_write((level), "%s: "fmt, LOG_UNIT, ##__VA_ARGS__)
#else
#  define LOG_WRITE(level, ...) log_write((level), __VA_ARGS__)
#endif

/**
 * @brief Log message if level <= LOG_LEVEL
 */
#ifdef __clang__    /* following pragmas required for clang 3.8.0 */
#define LOG(level, ...) do { \
        _Pragma("clang diagnostic push") \
        _Pragma("clang diagnostic ignored \"-Wtautological-compare\"") \
        if ((level) <= LOG_LEVEL) LOG_WRITE((level), __VA_ARGS__); } while (0U) \
        _Pragma("clang diagnostic pop")
#else
#define LOG(level, ...) do { \
        if ((level) <= LOG_LEVEL) LOG_WRITE((level), __VA_ARGS__); } while (0U)
#endif /* __clang__ */

/**
 * @name Logging convenience defines
 * @{
 */
#define LOG_WTF(...) LOG(LOG_WTF, __VA_ARGS__)          /**< log a terrible failure */
#define LOG_ERROR(...) LOG(LOG_ERROR, __VA_ARGS__)      /**< log an error */
#define LOG_WARNING(...) LOG(LOG_WARNING, __VA_ARGS__)  /**< log a warning */
#define LOG_INFO(...) LOG(LOG_INFO, __VA_ARGS__)        /**< for the curious */
/**
 * @brief   Print a log message, if `LOG_LEVEL` is defined to be at least
 *          `LOG_DEBUG`
 *
 * @note    This looks similar to the @ref DEBUG function. However, it is
 *          enabled *globally*. Prefer @ref DEBUG (which can be enabled on a
 *          per-file granularity) for debug output relevant for debugging
 *          a module in RIOT. Prefer this for debug output relevant for
 *          application developers using your module (e.g. to hint potentially
 *          incorrect / inefficient use of your library).
 * @details If a variable is only accessed by `LOG_DEBUG()`, the compiler will
 *          not warn about unused variables even when the log level is lower
 *          than `LOG_DEBUG`.
 */
#define LOG_DEBUG(...) LOG(LOG_DEBUG, __VA_ARGS__)
/** @} */

#ifdef MODULE_LOG
#include "log_module.h"
#else
#include <stdio.h>

#include "periph/pm.h"

/**
 * @brief Default log_write function, just maps to printf
 */
#define log_write(level, ...) do { printf(__VA_ARGS__); if (level == LOG_WTF) pm_off(); } while (0U)
#endif

#ifdef __cplusplus
}
#endif

/** @} */
