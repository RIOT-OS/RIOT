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
#ifdef MODULE_ESP_COMMON
/* ESP_COMMON provides its own log_module implementation see
 * - cpu/esp_common/include/log_module.h
 * - cpu/esp_common/include/esp_common_log.h */

typedef int dont_be_pedantic; /* this c-file is not empty */

#else /*MODULE_ESP_COMMON*/
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "ansi_style.h"
#include "kernel_defines.h"
#include "log.h"

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
 * Default is bold
 */
#ifndef LOG_INFO_ANSI_COLOR_CODE
#define LOG_INFO_ANSI_COLOR_CODE        ANSI_STYLE(BOLD, FOREGROUND(GREEN))
#endif

/**
 * @brief   Default ANSI color escape code for debug logs
 *
 * Default is green
 */
#ifndef LOG_DEBUG_ANSI_COLOR_CODE
#define LOG_DEBUG_ANSI_COLOR_CODE       ANSI_STYLE(BOLD, FOREGROUND(WHITE))
#endif

#define LOG_UNIT_ANSI_COLOR_CODE        ANSI_STYLE(BOLD, FOREGROUND_BRIGHT(CYAN))
#define LOG_SEPARATORS_ANSI_COLOR_CODE  ANSI_STYLE(DIM, FOREGROUND(WHITE))

static const char * const level_strings[] =
{
    [LOG_ERROR] = LOG_ERROR_ANSI_COLOR_CODE "ERROR",
    [LOG_WARNING] = LOG_WARNING_ANSI_COLOR_CODE "WARN ",
    [LOG_INFO] = LOG_INFO_ANSI_COLOR_CODE "INFO ",
    [LOG_DEBUG] = LOG_DEBUG_ANSI_COLOR_CODE "DEBUG",
};

void log_write(unsigned level, const char* unit, const char *format, ...)
{
    assert((level > 0) && (level < ARRAY_SIZE(level_strings)));

    printf("%s " ANSI_STYLE_RESET "# ", level_strings[level]);
    if (unit && strlen(unit) > 0) {
        printf(LOG_UNIT_ANSI_COLOR_CODE "%s" LOG_SEPARATORS_ANSI_COLOR_CODE ": ", unit);
    }
    printf(ANSI_STYLE_RESET);
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
