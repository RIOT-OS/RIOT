/*
 * SPDX-FileCopyrightText: 2026 TU Dresden
 * SPDX-FileCopyrightText: 2026 Carl Seifert <carl.seifert@tu-dresden.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdio.h>

#include "ansi_style.h"
#include "debug.h"
#include "macros/utils.h"
#include "fmt.h"

/**
 * @name Configuring contextual information
 * @{
 */

/**
 * @brief Show log level in log messages printed
 *
 * **Default**: Enabled (1)
 */
#if !defined(CONFIG_LOG_SHOW_LEVEL) || defined(DOXYGEN)
#  define CONFIG_LOG_SHOW_LEVEL 1
#endif

/**
 * @brief Show log level only as letter instead of word
 *
 * **Default**: Disabled (0)
 *
 * If enabled, turns levels into letters:
 * - `E` instead of `ERROR`
 * - `W` instead of `WARN`
 * - `I` instead of `INFO`
 * - `D` instead of `DEBUG`
 */
#if !defined(CONFIG_LOG_SHOW_LEVEL_LETTER_ONLY) || defined(DOXYGEN)
#  define CONFIG_LOG_SHOW_LEVEL_LETTER_ONLY 0
#endif

#if CONFIG_LOG_SHOW_LEVEL_LETTER_ONLY && !CONFIG_LOG_SHOW_LEVEL
#  error "CONFIG_LOG_SHOW_LEVEL_LETTER_ONLY requires CONFIG_LOG_SHOW_LEVEL to be set"
#endif

/**
 * @brief Show log unit in log messages printed, if log unit is set
 *
 * **Default**: Enabled (1)
 */
#if !defined(CONFIG_LOG_SHOW_UNIT) || defined(DOXYGEN)
#  define CONFIG_LOG_SHOW_UNIT 1
#endif

/**
 * @brief Show calling thread in log messages printed
 *
 * **Default**: Disabled (0)
 */
#if !defined(CONFIG_LOG_SHOW_THREAD) || defined(DOXYGEN)
#  define CONFIG_LOG_SHOW_THREAD 1
#endif

#if CONFIG_LOG_SHOW_THREAD && !defined(CONFIG_THREAD_NAMES)
#  error "CONFIG_LOG_SHOW_THREAD requires CONFIG_THREAD_NAMES to be set"
#endif

/**
 * @brief Show calling function in log messages printed
 *
 * **Default**: Disabled (0)
 */
#if !defined(CONFIG_LOG_SHOW_FUNC) || defined(DOXYGEN)
#  define CONFIG_LOG_SHOW_FUNC 1
#endif

/**
 * @brief Show file containing call site in log messages printed
 *
 * **Default**: Disabled (0)
 *
 * The file path and line number will be printed above each log line.
 */
#if !defined(CONFIG_LOG_SHOW_FILE) || defined(DOXYGEN)
#  define CONFIG_LOG_SHOW_FILE 1
#endif

/** @} */ /* section */

/**
 * @name Configuring styles
 * @{
 */

/**
 * @brief   Default ANSI color escape code for error logs
 *
 * **Default**: bold red
 */
#if !defined(LOG_LEVEL_STYLE_LOG_ERROR) || defined(DOXYGEN)
#  define LOG_LEVEL_STYLE_LOG_ERROR       ANSI_STYLE(BOLD, FOREGROUND(RED))
#endif

/**
 * @brief   Default ANSI color escape code for warning logs
 *
 * **Default**: bold yellow
 */
#if !defined(LOG_LEVEL_STYLE_LOG_WARNING) || defined(DOXYGEN)
#  define LOG_LEVEL_STYLE_LOG_WARNING     ANSI_STYLE(BOLD, FOREGROUND(YELLOW))
#endif

/**
 * @brief   Default ANSI color escape code for info logs
 *
 * **Default**: bold green
 */
#if !defined(LOG_LEVEL_STYLE_LOG_INFO) || defined(DOXYGEN)
#  define LOG_LEVEL_STYLE_LOG_INFO        ANSI_STYLE(BOLD, FOREGROUND(WHITE))
#endif

/**
 * @brief   Default ANSI color escape code for debug logs
 *
 * **Default**: bold
 */
#if !defined(LOG_LEVEL_STYLE_LOG_DEBUG) || defined(DOXYGEN)
#  define LOG_LEVEL_STYLE_LOG_DEBUG       ANSI_STYLE(BOLD, DIM, FOREGROUND(WHITE))
#endif

#if CONFIG_LOG_SHOW_LEVEL_LETTER_ONLY && !defined(DOXYGEN)
#  define LOG_LEVEL_STRING_LOG_ERROR   "E"
#  define LOG_LEVEL_STRING_LOG_WARNING "W"
#  define LOG_LEVEL_STRING_LOG_INFO    "I"
#  define LOG_LEVEL_STRING_LOG_DEBUG   "D"
#else
#  define LOG_LEVEL_STRING_LOG_ERROR   "ERROR"
#  define LOG_LEVEL_STRING_LOG_WARNING "WARN "
#  define LOG_LEVEL_STRING_LOG_INFO    "INFO "
#  define LOG_LEVEL_STRING_LOG_DEBUG   "DEBUG"
#endif

/**
 * @brief ANSI style escape code for log unit prefix
 *
 * **Default**: bright green and bold
 */
#if !defined(LOG_STYLE_UNIT)|| defined(DOXYGEN)
#  define LOG_STYLE_UNIT              ANSI_STYLE(BOLD, FOREGROUND_BRIGHT(GREEN))
#endif

/**
 * @brief ANSI style escape code for colons and hashes in log messages
 *
 * **Default**: dim foreground color
 */
#if !defined(LOG_STYLE_EXTRAS) || defined(DOXYGEN)
#  define LOG_STYLE_EXTRAS            ANSI_STYLE(DIM, FOREGROUND(WHITE))
#endif

/**
 * @brief ANSI style escape code for file path in log messages
 *
 * **Default**: dim foreground color
 */
#if !defined(LOG_STYLE_FILE) || defined(DOXYGEN)
#  define LOG_STYLE_FILE              ANSI_STYLE(DIM, FOREGROUND(WHITE))
#endif

/** @} */ /* section */

/** @cond */ /* hide */

#if CONFIG_LOG_SHOW_FILE
#  define _CLOWNFISH_FMT_FILE_FSTR LOG_STYLE_FILE "%s:%u:\n" ANSI_STYLE_RESET
#  define _CLOWNFISH_FMT_FILE_ARGS , __FILE__, __LINE__
#else
#  define _CLOWNFISH_FMT_FILE_FSTR ""
#  define _CLOWNFISH_FMT_FILE_ARGS
#endif

#if CONFIG_LOG_SHOW_LEVEL
#  define _CLOWNFISH_FMT_LEVEL_FSTR(level) "%s"
#  define _CLOWNFISH_FMT_LEVEL_ARGS(level) , LOG_LEVEL_STYLE_ ## level LOG_LEVEL_STRING_ ## level
#else
#  define _CLOWNFISH_FMT_LEVEL_FSTR(...) ""
#  define _CLOWNFISH_FMT_LEVEL_ARGS(...) 
#endif

#if CONFIG_LOG_SHOW_UNIT
#  define _CLOWNFISH_FMT_UNIT_FSTR(unit) LOG_STYLE_UNIT "%s"
#  define _CLOWNFISH_FMT_UNIT_ARGS(unit) , unit
#else
#  define _CLOWNFISH_FMT_UNIT_FSTR(...)
#  define _CLOWNFISH_FMT_UNIT_ARGS(...)
#endif

#if CONFIG_LOG_SHOW_FUNC && CONFIG_LOG_SHOW_THREAD
#  define _CLOWNFISH_FMT_EXTRAS_FSTR LOG_STYLE_EXTRAS " (%s@%s): " ANSI_STYLE_RESET
#  define _CLOWNFISH_FMT_EXTRAS_ARGS , DEBUG_FUNC, _debug_thread_name_or_isr()
#elif CONFIG_LOG_SHOW_FUNC
#  define _CLOWNFISH_FMT_EXTRAS_FSTR LOG_STYLE_EXTRAS " (%s): " ANSI_STYLE_RESET
#  define _CLOWNFISH_FMT_EXTRAS_ARGS , DEBUG_FUNC
#elif CONFIG_LOG_SHOW_THREAD
#  define _CLOWNFISH_FMT_EXTRAS_FSTR LOG_STYLE_EXTRAS " (@%s): " ANSI_STYLE_RESET
#  define _CLOWNFISH_FMT_EXTRAS_ARGS , _debug_thread_name_or_isr()
#else
#  define _CLOWNFISH_FMT_EXTRAS_FSTR LOG_STYLE_EXTRAS ": " ANSI_STYLE_RESET
#  define _CLOWNFISH_FMT_EXTRAS_ARGS
#endif

#define LOG_FORMAT_PRINT_PREFIX
#define LOG_FORMAT(level, unit, fmt, ...) \
        _CLOWNFISH_FMT_FILE_FSTR                 \
        _CLOWNFISH_FMT_LEVEL_FSTR(level)         \
        ANSI_STYLE_RESET " # "                   \
        _CLOWNFISH_FMT_UNIT_FSTR(unit)           \
        _CLOWNFISH_FMT_EXTRAS_FSTR               \
        fmt                                      \
        /* comma comes from macros below */      \
        _CLOWNFISH_FMT_FILE_ARGS                 \
        _CLOWNFISH_FMT_LEVEL_ARGS(level)         \
        _CLOWNFISH_FMT_UNIT_ARGS(unit)           \
        _CLOWNFISH_FMT_EXTRAS_ARGS               \
        , ##__VA_ARGS__                           

/** @endcond */ /* visible */
