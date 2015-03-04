/*
 * Copyright (C) 2015 Kaspar Schleiser
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file        log.h
 * @brief       System logging header
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __LOG_H
#define __LOG_H

#ifdef __cplusplus
 extern "C" {
#endif

enum {
    LOG_NONE,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG,
    LOG_ALL
};

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_ALL
#endif

/**
 * @brief Log message if level <= LOG_LEVEL
 *
 * ... doc to come ...
 *
 * @{
 */

#define LOG(level, ...) if (level <= LOG_LEVEL) _log(level, __VA_ARGS__)

/** @} */

#ifdef HAVE_LOG_MODULE
#include "log_module.h"
#else

#include <stdio.h>

#define _log(level, ...) printf(__VA_ARGS__)

#define log_error(...) LOG(LOG_ERROR, __VA_ARGS__)
#define log_warning(...) LOG(LOG_WARNING, __VA_ARGS__)
#define log_info(...) LOG(LOG_INFO, __VA_ARGS__)
#define log_debug(...) LOG(LOG_DEBUG, __VA_ARGS__)

#endif

#ifdef __cplusplus
}
#endif

#endif /* __LOG_H */
/** @} */
