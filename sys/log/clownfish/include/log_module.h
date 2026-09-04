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
#include "modules.h"

#if !defined(CONFIG_LOG_SHOW_UNIT) || defined(DOXYGEN)
#  define CONFIG_LOG_SHOW_UNIT 1
#endif

#if !defined(CONFIG_LOG_SHOW_THREAD) || defined(DOXYGEN)
#  define CONFIG_LOG_SHOW_THREAD 1
#endif

#if CONFIG_LOG_SHOW_THREAD && !defined(CONFIG_THREAD_NAMES)
#  error "CONFIG_LOG_SHOW_THREAD requires CONFIG_THREAD_NAMES to be set"
#endif

#if !defined(CONFIG_LOG_SHOW_FUNC) || defined(DOXYGEN)
#  define CONFIG_LOG_SHOW_FUNC 1
#endif

#if !defined(CONFIG_LOG_SHOW_FILE) || defined(DOXYGEN)
#  define CONFIG_LOG_SHOW_FILE 1
#endif

/**
 * @brief   Default ANSI color escape code for error logs
 *
 * **Default**: bold red
 */
#if !defined(LOG_STYLE_LEVEL_LOG_ERROR) || defined(DOXYGEN)
#  define LOG_STYLE_LEVEL_LOG_ERROR       ANSI_STYLE(BOLD, FOREGROUND(RED))
#endif

/**
 * @brief   Default ANSI color escape code for warning logs
 *
 * **Default**: bold yellow
 */
#if !defined(LOG_STYLE_LEVEL_LOG_WARNING) || defined(DOXYGEN)
#  define LOG_STYLE_LEVEL_LOG_WARNING     ANSI_STYLE(BOLD, FOREGROUND(YELLOW))
#endif

/**
 * @brief   Default ANSI color escape code for info logs
 *
 * **Default**: bold green
 */
#if !defined(LOG_STYLE_LEVEL_LOG_INFO) || defined(DOXYGEN)
#  define LOG_STYLE_LEVEL_LOG_INFO        ANSI_STYLE(BOLD, FOREGROUND(WHITE))
#endif

/**
 * @brief   Default ANSI color escape code for debug logs
 *
 * **Default**: bold
 */
#if !defined(LOG_STYLE_LEVEL_LOG_DEBUG) || defined(DOXYGEN)
#  define LOG_STYLE_LEVEL_LOG_DEBUG       ANSI_STYLE(BOLD, DIM, FOREGROUND(WHITE))
#endif

#define LOG_STRING_LEVEL_LOG_ERROR   "ERROR"
#define LOG_STRING_LEVEL_LOG_WARNING "WARN "
#define LOG_STRING_LEVEL_LOG_INFO    "INFO "
#define LOG_STRING_LEVEL_LOG_DEBUG   "DEBUG"

#if !defined(LOG_STYLE_UNIT)|| defined(DOXYGEN)
#  define LOG_STYLE_UNIT              ANSI_STYLE(BOLD, FOREGROUND_BRIGHT(GREEN))
#endif

#if !defined(LOG_STYLE_EXTRAS) || defined(DOXYGEN)
#  define LOG_STYLE_EXTRAS            ANSI_STYLE(DIM, FOREGROUND(WHITE))
#endif

#if !defined(LOG_STYLE_FILE) || defined(DOXYGEN)
#  define LOG_STYLE_FILE              ANSI_STYLE(DIM, FOREGROUND(WHITE))
#endif

#if !defined(LOG_STREAM) || defined(DOXYGEN)
#  define LOG_STREAM stdout
#endif

#if !defined(DOXYGEN)

#  if CONFIG_LOG_SHOW_FUNC
#    define _LOG_FUNC DEBUG_FUNC
#  else
#    define _LOG_FUNC ""
#  endif

#  if CONFIG_LOG_SHOW_FILE
#    define _LOG_FILE DEBUG_FILE_PATH
#    define _LOG_LINE DEBUG_LINE
#  else
#    define _LOG_FILE ""
#    define _LOG_LINE 0
#  endif

#  if CONFIG_LOG_SHOW_THREAD
#    define _LOG_THREAD __debug_thread_name_or_isr()
#  else
#    define _LOG_THREAD ""
#  endif

#  define _fprint_(stream, fmt, ...)    fprintf(stream, fmt, ##__VA_ARGS__)
#  define _fprint_noformat(stream, str) fputs(str, stream)

#define __fprint_get_macro(\
    _01, _02, _03, _04, _05, _06, _07, _08, _09, _0a, _0b, _0c, _0d, _0e, _0f, \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _1a, _1b, _1c, _1d, _1e, _1f, \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _2a, _2b, _2c, _2d, _2e, _2f, \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _3a, _3b, _3c, _3d, _3e, _3f, \
    N, ...) N

#define __fprint_is_noformat(...) __fprint_get_macro( \
        dummy, ##__VA_ARGS__,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,noformat)

#  define _fprint(stream, s, ...) CONCAT(_fprint_, __fprint_is_noformat(__VA_ARGS__))(stream, s, ##__VA_ARGS__)
#  define _print(s, ...) _fprint(stdout, s, ##__VA_ARGS__)

#  define _fprintln(stream, s, ...) _fprint(stream, s "\n", ##__VA_ARGS__)
#  define _println(s, ...) _fprintln(stdout, s, ##__VA_ARGS__)

#  define log_write(level, unit, ...) do { \
    _clownfish_print_prologue( \
        LOG_STYLE_LEVEL_## level LOG_STRING_LEVEL_## level, \
        unit, _LOG_FILE, _LOG_LINE, _LOG_FUNC, _LOG_THREAD);\
    _fprint(LOG_STREAM, __VA_ARGS__); \
} while (0)

#  define log_write_continue(level, unit, ...) \
    _fprint(LOG_STREAM, __VA_ARGS__)

void _clownfish_print_prologue(
    const char* levelstr, const char* unit, const char* file, unsigned int line, 
    const char* func, const char* thread);
#endif
