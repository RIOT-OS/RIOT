/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "kernel_defines.h"
#include "log.h"
#include "ansi_style.h"

/**
 * @brief   Default ANSI color escape code for error logs
 *
 * Default is bold red
 */
#ifndef LOG_ERROR_ANSI_COLOR_CODE
#define LOG_ERROR_ANSI_COLOR_CODE       ANSI_STYLE(BOLD, FOREGROUND(RED))
#endif

/**
 * @brief   Default ANSI color escape code for warning logs
 *
 * Default is bold yellow
 */
#ifndef LOG_WARNING_ANSI_COLOR_CODE
#define LOG_WARNING_ANSI_COLOR_CODE     ANSI_STYLE(BOLD, FOREGROUND(YELLOW))
#endif

/**
 * @brief   Default ANSI color escape code for info logs
 *
 * Default is bold white
 */
#ifndef LOG_INFO_ANSI_COLOR_CODE
#define LOG_INFO_ANSI_COLOR_CODE        ANSI_STYLE(BOLD)
#endif

/**
 * @brief   Default ANSI color escape code for debug logs
 *
 * Default is green
 */
#ifndef LOG_DEBUG_ANSI_COLOR_CODE
#define LOG_DEBUG_ANSI_COLOR_CODE       ANSI_STYLE(FOREGROUND(GREEN))
#endif

/**
 * @brief   ANSI color escape code used for resetting color
 */
#define LOG_RESET_ANSI_COLOR_CODE       ("\033[0m")

static const char * const _ansi_codes[] =
{
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
    printf(ANSI_STYLE_RESET);

#if !defined(__MSP430__)
    /* no fflush on msp430 */
    fflush(stdout);
#endif
}

#endif /*MODULE_ESP_COMMON*/
/**@}*/
