/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_log_color Colored log module
 * @ingroup     sys
 * @brief       This module implements a logging module with colored output
 * @{
 *
 * @file
 * @brief       log_module header
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LOG_MODULE_H
#define LOG_MODULE_H

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default ANSI color escape code for error logs
 *
 * Default is bold red
 */
#ifndef LOG_ERROR_ANSI_COLOR_CODE
#define LOG_ERROR_ANSI_COLOR_CODE       ("\033[1;31m")
#endif

/**
 * @brief   Default ANSI color escape code for warning logs
 *
 * Default is bold yellow
 */
#ifndef LOG_WARNING_ANSI_COLOR_CODE
#define LOG_WARNING_ANSI_COLOR_CODE     ("\033[1;33m")
#endif

/**
 * @brief   Default ANSI color escape code for info logs
 *
 * Default is bold white
 */
#ifndef LOG_INFO_ANSI_COLOR_CODE
#define LOG_INFO_ANSI_COLOR_CODE        ("\033[1m")
#endif

/**
 * @brief   Default ANSI color escape code for debug logs
 *
 * Default is green
 */
#ifndef LOG_DEBUG_ANSI_COLOR_CODE
#define LOG_DEBUG_ANSI_COLOR_CODE       ("\033[0;32m")
#endif

/**
 * @brief   ANSI color escape code used for resetting color
 */
#define LOG_RESET_ANSI_COLOR_CODE       ("\033[0m")

/**
 * @brief   ANSI color escape codes array
 *
 * Internal use only
 */
static const char * const _ansi_codes[] =
{
    [LOG_ERROR] = LOG_ERROR_ANSI_COLOR_CODE,
    [LOG_WARNING] = LOG_WARNING_ANSI_COLOR_CODE,
    [LOG_INFO] = LOG_INFO_ANSI_COLOR_CODE,
    [LOG_DEBUG] = LOG_DEBUG_ANSI_COLOR_CODE,
};

/**
 * @brief log_write overridden function for colored output
 *
 * @param[in] level  Logging level
 * @param[in] format String format to print
 */
static inline void log_write(unsigned level, const char *format, ...)
{
    assert(level > 0);

    printf("%s", _ansi_codes[level]);
    va_list args;
    va_start(args, format);
    /* Temporarily disable clang format-nonliteral warning */
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif /* clang */
    vprintf(format, args);
#ifdef __clang__
#pragma clang diagnostic pop
#endif /* clang */
    va_end(args);
    printf(LOG_RESET_ANSI_COLOR_CODE);

#if defined(MODULE_NEWLIB) || defined(MODULE_PICOLIBC)
    /* no fflush on msp430 */
    fflush(stdout);
#endif
}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* LOG_MODULE_H */
