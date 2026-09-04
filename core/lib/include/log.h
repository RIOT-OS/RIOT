/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include "macros/xtstr.h"
#include <string.h>

/**
 * @defgroup     core_util_log Logging
 * @brief Logging
 * @{
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
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief       System logging header
 *
 * See "sys/log/log_printfnoformat" for an example.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

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
typedef enum {
    /**
     * @brief Lowest log level, will output nothing
     */
    LOG_NONE = 0,
    /**
     * @brief Error log level, will print only critical,
     *        non-recoverable errors like hardware initialization failures
     */
    LOG_ERROR = 1,
    /**
     * @brief Warning log level, will print warning messages for
     *        temporary errors
     */
    LOG_WARNING = 2,
    /**
     * @brief Informational log level, will print purely
     *        informational messages like successful system bootup,
     *        network link state, ...
     */
    LOG_INFO = 3,
    /**
     * @brief Debug log level, printing developer stuff considered
     *        too verbose for production use
     */
    LOG_DEBUG = 4,

    LOG_ALL,
} log_level_t;


#if !defined(LOG_LEVEL) || defined(DOXYGEN)
/**
 * @brief Default log level define
 */
#  define LOG_LEVEL LOG_INFO
#endif

#if !defined(DOXYGEN)

/* If the log unit is not provided, we assume the default created by the build system. */
#  if !defined(LOG_UNIT)
#    if defined(LOG_UNIT_DEFAULT)
#      define LOG_UNIT LOG_UNIT_DEFAULT
#    else
#      define LOG_UNIT ""
#    endif
#  endif

#  define _LOG_LEVEL_MATCHES(level) ((log_level_t)(level) <= (log_level_t)(LOG_LEVEL))

/* If LOG is present in make, switch to new logic: LOG invocations used to always have an effect
 * if the level matched, now they only do if contained in LOG. DEBUG invocations used to only
 * have an effect when ENABLE_DEBUG was turned on, now they do too if contained in LOG */
#  if defined(LOG_UNITS_SELECTIVE)
/*   New behavior:
 *   - LOG() iff level and unit matches
 *   - DEBUG() iff (ENABLE_DEBUG or [level and unit matches, just like LOG])
 */
#    define _CAN_LOG_H(level, unit) (_LOG_LEVEL_MATCHES(level) && _LOG_UNIT_ENABLED(unit))
#    define _CAN_DEBUG_H(level, unit) (IS_ACTIVE(ENABLE_DEBUG) || _CAN_LOG_H(level, unit))

/*   There are three tiers of selective logging: */
#    if defined(LOG_UNITS_SELECTIVE_ALL)
/*     1. LOG="*", i.e., enable all log units */
#      define _LOG_UNIT_ENABLED(unit) true
#    elif defined(LOG_UNITS_SELECTIVE_PATTERNS)
/*     2. LOG="core.irq ztimer", i.e., enable log units by prefix pattern */
#      define _LOG_UNIT_ENABLED(unit) ((strlen(unit) > 0) && ({                                \
            bool forced = false;                                                                   \
            for (unsigned int i = 0; i < ARRAY_SIZE((const char*[]){ LOG_UNITS_SELECTIVE_PATTERNS }); i += 1) {    \
                forced |= strncmp(((const char*[]){ LOG_UNITS_SELECTIVE_PATTERNS })[i], unit,                      \
                           strlen(((const char*[]){ LOG_UNITS_SELECTIVE_PATTERNS })[i])) == 0;                     \
            }                                                                                      \
            forced;                                                                                \
        }))
#    else
/*     3. LOG="", i.e., disable all log units */
#      define _LOG_UNIT_ENABLED(unit) false
#    endif

#  else /* defined(LOG_UNITS_SELECTIVE) */
/*   Old behavior:
 *   - LOG() iff level matches
 *   - DEBUG() iff ENABLE_DEBUG is on. */
#    define _CAN_LOG_H(level, unit) _LOG_LEVEL_MATCHES(level)
#    define _CAN_DEBUG_H(level, unit) IS_ACTIVE(ENABLE_DEBUG)
#  endif /* defined(LOG_UNITS_SELECTIVE) */

#  if defined(__clang__)
#    define __LOG_PROLOGUE \
      _Pragma("clang diagnostic push") \
      _Pragma("clang diagnostic ignored \"-Wtautological-compare\"")
#    define __LOG_EPILOGUE \
      _Pragma("clang diagnostic pop")
#  else /* defined(__clang__) */
#    define __LOG_PROLOGUE
#    define __LOG_EPILOGUE
#  endif /* defined(__clang__) */

#endif /* !defined(DOXYGEN) */

#define LOG_WRITE(level, unit, ...) \
    do { __LOG_PROLOGUE \
        if (_CAN_LOG_H(level, unit)) { \
            log_write(level, (unit), __VA_ARGS__); \
        } \
    } while (0U) __LOG_EPILOGUE

#define LOG_WRITE_CONT(level, unit, ...) \
    do { __LOG_PROLOGUE \
        if (_CAN_LOG_H(level, unit)) { \
            log_write_continue(level, (unit), __VA_ARGS__); \
        } \
    } while (0U) __LOG_EPILOGUE

#define LOG(level, ...)      LOG_WRITE(level, LOG_UNIT, __VA_ARGS__)
#define LOG_CONT(level, ...) LOG_WRITE_CONT(level, LOG_UNIT, __VA_ARGS__)

#define LOG_ERROR(...)   LOG(LOG_ERROR,   __VA_ARGS__)
#define LOG_WARNING(...) LOG(LOG_WARNING, __VA_ARGS__)
#define LOG_INFO(...)    LOG(LOG_INFO,    __VA_ARGS__)
#define LOG_DEBUG(...)   LOG(LOG_DEBUG,   __VA_ARGS__)
/** @} */


#ifdef MODULE_LOG
#  include "log_module.h"
#else
#  include <stdio.h>

/**
 * @brief Default log_write function, just maps to printf
 */
#  define log_write(level, unit, ...) \
    do { \
        if (IS_ACTIVE(CONFIG_LOG_SHOW_UNIT) && (unit) && (unit)[0]) { \
            printf("%s: ", (unit)); \
        } \
        printf(__VA_ARGS__); \
    } while (0)

#  define log_write_continue(level, unit, ...) \
    do { \
        printf(__VA_ARGS__); \
    } while (0)

#endif

#ifdef __cplusplus
}
#endif

/** @} */
