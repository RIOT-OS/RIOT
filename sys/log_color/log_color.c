/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  sys_log_color Colored log module
 * @{
 *
 * @file
 * @brief       log_color
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */
#ifdef MODULE_ESP_COMMON
/* ESP_COMMON provides its own log_module implementation see
 * - cpu/esp_common/include/log_module.h
 * - cpu/esp_common/include/esp_common_log.h */

typedef int dont_be_pedantic; /* this c-file is not empty */

#else /*MODULE_ESP_COMMON*/
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "kernel_defines.h"
#include "periph/pm.h"
#include "log.h"

/**
 * @brief   Default ANSI color escape code for WTF logs
 *
 * Default is bold cyan
 */
#ifndef LOG_WTF_ANSI_COLOR_CODE
#define LOG_WTF_ANSI_COLOR_CODE       ("\033[1;36m")
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

static const char * const _ansi_codes[] =
{
    [LOG_WTF] = LOG_WTF_ANSI_COLOR_CODE,
    [LOG_ERROR] = LOG_ERROR_ANSI_COLOR_CODE,
    [LOG_WARNING] = LOG_WARNING_ANSI_COLOR_CODE,
    [LOG_INFO] = LOG_INFO_ANSI_COLOR_CODE,
    [LOG_DEBUG] = LOG_DEBUG_ANSI_COLOR_CODE,
};

void log_write(unsigned level, const char *format, ...)
{
    assert((level > 0) && (level < ARRAY_SIZE(_ansi_codes)));

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

#if !defined(__MSP430__)
    /* no fflush on msp430 */
    fflush(stdout);
#endif

    if (level == LOG_WTF) {
        pm_off();
    }
}

#endif /*MODULE_ESP_COMMON*/
/**@}*/
