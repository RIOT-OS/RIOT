/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
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
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef LOG_H
#define LOG_H

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
 * @brief Dummy function to discard log messages
 */
#define LOG_VOID                (void)(0)

/**
 * @brief logging convenience defines
 * @{
 */
#if LOG_LEVEL >= LOG_ERROR
    #define LOG_ERROR(...)      log_write(LOG_ERROR, __VA_ARGS__)
#endif
#ifndef LOG_ERROR
    #define LOG_ERROR(...)      LOG_VOID
#endif

#if LOG_LEVEL >= LOG_WARNING
    #define LOG_WARNING(...)    log_write(LOG_WARNING, __VA_ARGS__)
#endif

#ifndef LOG_WARNING
    #define LOG_WARNING(...)    LOG_VOID
#endif

#if LOG_LEVEL >= LOG_INFO
    #define LOG_INFO(...)       log_write(LOG_INFO, __VA_ARGS__)
#endif
#ifndef LOG_INFO
    #define LOG_INFO(...)       LOG_VOID
#endif

#if LOG_LEVEL >= LOG_DEBUG
    #define LOG_DEBUG(...)      log_write(LOG_DEBUG, __VA_ARGS__)
#endif
#ifndef LOG_DEBUG
    #define LOG_DEBUG(...)      LOG_VOID
#endif
/** @} */

/**
 * @brief Log message if level <= LOG_LEVEL
 */
#define LOG(level, ...) switch(level) {         \
    case LOG_ERROR: LOG_ERROR(__VA_ARGS__); break;      \
    case LOG_WARNING: LOG_WARNING(__VA_ARGS__); break;  \
    case LOG_INFO: LOG_INFO(__VA_ARGS__); break;        \
    case LOG_DEBUG: LOG_DEBUG(__VA_ARGS__); break;      \
    default: LOG_VOID;}

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
