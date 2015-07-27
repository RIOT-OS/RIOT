/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
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
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef __LOG_H
#define __LOG_H

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
#define LOG_LEVEL_NONE    (0)    /**< Lowest log level, will output nothing */
#define LOG_LEVEL_KERNEL  (1)    /**< Kernel log level, will print only kernel messages */
#define LOG_LEVEL_ERROR   (2)    /**< Error log level, will print only critical,
                                      non-recoverable errors like hardware initialization
                                      failures */
#define LOG_LEVEL_WARNING (3)    /**< Warning log level, will print warning messages for
                                      temporary errors */
#define LOG_LEVEL_INFO    (4)    /**< Informational log level, will print purely
                                      informational messages like successful system bootup,
                                      network link state, ...*/
#define LOG_LEVEL_DEBUG   (5)    /**< Debug log level, printing developer stuff considered
                                      too verbose for production use */
#define LOG_LEVEL_ALL     (6)    /**< print everything */

#ifndef LOG_LEVEL
/**
 * @brief Default log level define
 */
#ifdef DEVELHELP
#   define LOG_LEVEL    LOG_LEVEL_INFO
#else
#   define LOG_LEVEL    LOG_LEVEL_KERNEL
#endif
#endif

/**
 * @brief Log message if level <= LOG_LEVEL
 */
#define LOG(level, ...) if (level <= LOG_LEVEL) log_write(level, __VA_ARGS__)

/**
 * @brief logging convenience defines
 * @{
 */
#define LOG_KERNEL(...) LOG(LOG_LEVEL_KERNEL, __VA_ARGS__)
#define LOG_ERROR(...) LOG(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_WARNING(...) LOG(LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_INFO(...) LOG(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(LOG_LEVEL_DEBUG, __VA_ARGS__)
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

#endif /* __LOG_H */
/** @} */
