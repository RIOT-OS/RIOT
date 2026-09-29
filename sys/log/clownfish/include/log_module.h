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
#  define CONFIG_LOG_SHOW_THREAD 0
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

#if CONFIG_LOG_SHOW_LEVEL_LETTER_ONLY
#  define LOG_STRING_LEVEL_LOG_ERROR   "E"
#  define LOG_STRING_LEVEL_LOG_WARNING "W"
#  define LOG_STRING_LEVEL_LOG_INFO    "I"
#  define LOG_STRING_LEVEL_LOG_DEBUG   "D"
#else
#  define LOG_STRING_LEVEL_LOG_ERROR   "ERROR"
#  define LOG_STRING_LEVEL_LOG_WARNING "WARN "
#  define LOG_STRING_LEVEL_LOG_INFO    "INFO "
#  define LOG_STRING_LEVEL_LOG_DEBUG   "DEBUG"
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

/**
 * @brief File/stream to output log messages to
 *
 * **Default**: `stdout`
 */
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

/* Provide experimental fprint, eprint, print, fprintln, eprintln, println
 * macros that route to fprintf or fputs depending on whether you pass
 * a format string or not to avoid printf overhead. */

/* Step 1: Define two implementation with same macro signature. */
#  define _fprint_(stream, fmt, ...)    fprintf(stream, fmt, ##__VA_ARGS__)
#  define _fprint_noformat(stream, str) fputs(str, stream)

#define __fprint_get_macro(\
    _01, _02, _03, _04, _05, _06, _07, _08, _09, _0a, _0b, _0c, _0d, _0e, _0f, \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _1a, _1b, _1c, _1d, _1e, _1f, \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _2a, _2b, _2c, _2d, _2e, _2f, \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _3a, _3b, _3c, _3d, _3e, _3f, \
    N, ...) N

/* Step 2: Call _fprint_noformat or _fprintf_ depending on whether there's
 * a single argument or more, which is the format case. */
#  define _fprint(stream, s, ...) CONCAT(_fprint_, \
      __fprint_get_macro(dummy,\
        ##__VA_ARGS__,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\,,,,,,,,,,,,,,,,,,,,,,,,,,noformat) \
    )(stream, s, ##__VA_ARGS__)

/* Step 3: Define convenience print functions that route to stdout and stderr */
#  define _print(s, ...) _fprint(stdout, s, ##__VA_ARGS__)
#  define _eprint(s, ...) _fprint(stderr, s, ##__VA_ARGS__)

/* Step 3: Define convenience println functions that route to stdout and stderr */
#  define _fprintln(stream, s, ...) _fprint(stream, s "\n", ##__VA_ARGS__)
#  define _println(s, ...) _fprintln(stdout, s, ##__VA_ARGS__)
#  define _eprintln(s, ...) _fprintln(stderr, s, ##__VA_ARGS__)

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
