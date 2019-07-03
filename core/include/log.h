/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       System logging header
 *
 * This header offers a bunch of "LOG_*" functions that, with the default
 * implementation, just use printf, but honour a verbosity level.
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

#ifndef LOG_H
#define LOG_H

#include <stdint.h>

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
    LOG_NONE = -1,
    LOG_EMERG,      /**< Emergency log level, when system gets unusable */
    LOG_ALERT,      /**< Emergency log level, action must be taken immediately */
    LOG_CRIT,       /**< Emergency log level, for emergency errors which are not recoverable */
    LOG_ERROR,      /**< Error log level, will print only critical, possibly recoverable */
    LOG_WARNING,    /**< Warning log level, will print warning messages for
                         temporary errors */
    LOG_NOTICE,     /**< Notice log level, normal, but significant, conditions */
    LOG_INFO,       /**< Informational log level, will print purely
                         informational messages like successful system bootup,
                         network link state, ...*/
    LOG_DEBUG,      /**< Debug log level, printing developer stuff considered
                         too verbose for production use */
    LOG_ALL         /**< print everything */
};

/**
 * @brief LOG_ERR Wrapper used for backwards compatibility.
 *
 * Syslog uses LOG_ERROR while RIOT uses LOG_ERR.
 */
#define LOG_ERR (LOG_ERROR)

/**
 * @brief Possible LOG facilities as used in the glibc syslog implementation
 */
#define LOG_KERN      (0<<3)
#define LOG_USER      (1<<3)
#define LOG_MAIL      (2<<3)
#define LOG_DAEMON    (3<<3)
#define LOG_AUTH      (4<<3)
#define LOG_SYSLOG    (5<<3)
#define LOG_LPR       (6<<3)
#define LOG_NEWS      (7<<3)
#define LOG_UUCP      (8<<3)
#define LOG_CRON      (9<<3)
#define LOG_AUTHPRIV  (10<<3)
#define LOG_FTP       (11<<3)
#define LOG_LOCAL0    (16<<3)
#define LOG_LOCAL1    (17<<3)
#define LOG_LOCAL2    (18<<3)
#define LOG_LOCAL3    (19<<3)
#define LOG_LOCAL4    (20<<3)
#define LOG_LOCAL5    (21<<3)
#define LOG_LOCAL6    (22<<3)
#define LOG_LOCAL7    (23<<3)
#define	LOG_NFACILITIES	(20)

#ifndef LOG_LEVEL
/**
 * @brief Default log level define
 */
#define LOG_LEVEL LOG_INFO
#endif

/**
 * @brief Number of Bits in (facility|priority) used for the log level (priority)
 */
#define LOG_LEVEL_BITS (3)

/**
 * @brief Returns priority from a given (facility|priority)
 */
#define GET_LOG_LEVEL(x) ((uint32_t)x & (((uint32_t)1 << LOG_LEVEL_BITS) - 1))

/**
 * @brief Returns facility from a given (facility|priority)
 */
#define GET_FACILITY_LEVEL(x) ((uint32_t)x & (uint32_t)(INT_MAX-((1<<3)-1)))

/**
 * @brief Check for a given (facility|priority) has priority <= LOG_LEVEL
 */
#define CHECK_LOG_LEVEL(x) ((int)GET_LOG_LEVEL(x) <= (LOG_LEVEL))

/**
 * @brief Log message if level <= LOG_LEVEL
 */
#ifdef __clang__    /* following pragmas required for clang 3.8.0 */
#define LOG(level, ...) do { \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wtautological-compare\"") \
    if (CHECK_LOG_LEVEL(level)) log_write((level), __VA_ARGS__); } while (0U) \
    _Pragma("clang diagnostic pop")
#else
#define LOG(level, ...) do { \
    if (CHECK_LOG_LEVEL(level)) log_write((level), __VA_ARGS__); } while (0U)
#endif /* __clang__ */

/**
 * @name Logging convenience defines
 * @{
 */

#define LOG_EMERG(...) LOG(LOG_EMERG, __VA_ARGS__)
#define LOG_ALERT(...) LOG(LOG_ALERT, __VA_ARGS__)
#define LOG_CRIT(...) LOG(LOG_CRIT, __VA_ARGS__)
#define LOG_ERROR(...) LOG(LOG_ERROR, __VA_ARGS__)
#define LOG_WARNING(...) LOG(LOG_WARNING, __VA_ARGS__)
#define LOG_NOTICE(...) LOG(LOG_NOTICE, __VA_ARGS__)
#define LOG_INFO(...) LOG(LOG_INFO, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(LOG_DEBUG, __VA_ARGS__)
/** @} */

#ifdef MODULE_LOG
#include "log_module.h"
#else
#include <stdio.h>

/**
 * @brief Default log_write function, just maps to printf
 */
#define log_write(level, ...) printf(__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* LOG_H */
/** @} */
