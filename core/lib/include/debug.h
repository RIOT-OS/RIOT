/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Debug-header
 *
 * @details     If *ENABLE_DEBUG* is defined inside an implementation file, all
 *              calls to ::DEBUG will work the same as *printf* and output the
 *              given information to stdout. If *ENABLE_DEBUG* is not defined,
 *              all calls to ::DEBUG will be ignored.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 */

#include <stdio.h>

#include "ansi_colors.h"
#include "irq.h"
#include "sched.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def DEBUG_BREAKPOINT
 *
 * @brief Set a debug breakpoint
 *
 * When `DEVELHELP` is enabled, this traps the CPU and allows to debug the
 * program with e.g. `gdb`.
 * Without `DEVELHELP` this turns into a no-op.
 *
 * @warning     If no Debugger is attached, the CPU might get stuck here
 *              and consume a lot of power until reset.
 *
 * @param val   Breakpoint context for debugger, usually ignored.
 */
#ifdef DEVELHELP
#  include "architecture.h"
#  define DEBUG_BREAKPOINT(val) ARCHITECTURE_BREAKPOINT(val)
#else
#  define DEBUG_BREAKPOINT(val) (void)0
#endif

/**
 * @name Debugging defines
 * @{
 */
/**
 * @brief   This macro can be defined as 0 or other on a file-based level.
 *          @ref DEBUG() will generate output only if ENABLE_DEBUG is non-zero.
 */
#ifndef ENABLE_DEBUG
#  define ENABLE_DEBUG 0
#endif

/**
 * @brief   Common prefix for all debug messages, defaulting to an empty string.
 *          Expected to be set on a file-based level.
 */
#ifndef DEBUG_PREFIX
#  define DEBUG_PREFIX ""
#endif

/**
 * @def DEBUG_FUNC
 *
 * @brief   Contains the function name if given compiler supports it.
 *          Otherwise it is an empty string.
 */
#if defined(__cplusplus) && defined(__GNUC__)
#  define DEBUG_FUNC __PRETTY_FUNCTION__
#elif __STDC_VERSION__ >= 199901L
#  define DEBUG_FUNC __func__
#elif __GNUC__ >= 2
#  define DEBUG_FUNC __FUNCTION__
#else
#  define DEBUG_FUNC ""
#endif

/**
 * @brief   Whether calls to @ref DEBUG and @ref DEBUG_PUTS automatically include
 *          the calling thread name.
 *
 * **Default**: disabled
 */
#ifndef CONFIG_DEBUG_SHOW_THREAD
#  define CONFIG_DEBUG_SHOW_THREAD 0
#endif

#if IS_ACTIVE(CONFIG_DEBUG_SHOW_THREAD) && !defined(CONFIG_THREAD_NAMES)
#  error "CONFIG_DEBUG_SHOW_THREAD can only be set if CONFIG_THREAD_NAMES is set"
#endif

/**
 * @brief   Whether calls to @ref DEBUG and @ref DEBUG_PUTS automatically include
 *          the current function name.
 *
 * **Default**: disabled
 */
#if !defined(CONFIG_DEBUG_SHOW_FUNC) || defined(DOXYGEN)
#  define CONFIG_DEBUG_SHOW_FUNC 0
#endif

#define _DEBUG_SEP_FUNC     ":"
#define _DEBUG_SEP_THREAD   "@"
#define _DEBUG_SEP_MSG      " # "

#define _DEBUG_PREFIX_COLOR ANSI_COLOR_CYAN

static inline bool __debug_sufficient_stack(bool print)
{
#if IS_ACTIVE(DEVELHELP)
    const thread_t *thread = thread_get_active();
    if ((irq_is_in() && (ISR_STACKSIZE < THREAD_EXTRA_STACKSIZE_PRINTF)) ||
        ((thread != NULL) && (thread->stack_size < THREAD_EXTRA_STACKSIZE_PRINTF))) {
        if (print) {
            fputs("Cannot debug, stack too small. Consider using DEBUG_PUTS().\n", stdout);
        }
        return false;
    }
#endif
    return true;
}

static inline const char *__debug_thread_name_or_isr(void)
{
    const thread_t *thread = thread_get_active();
    return (irq_is_in() || thread == NULL) ? "(isr)" : thread_get_name(thread);
}

static inline bool __debug_print_prefix(const char *func_name)
{
    if (IS_ACTIVE(CONFIG_DEBUG_SHOW_FUNC) && IS_ACTIVE(CONFIG_DEBUG_SHOW_THREAD)) {
        printf(_DEBUG_PREFIX_COLOR DEBUG_PREFIX _DEBUG_SEP_FUNC "%s" \
               _DEBUG_SEP_THREAD "%s" _DEBUG_SEP_MSG ANSI_COLOR_RESET,
               func_name, __debug_thread_name_or_isr());
        return true;
    }
    else if (IS_ACTIVE(CONFIG_DEBUG_SHOW_FUNC)) {
        printf(DEBUG_PREFIX _DEBUG_SEP_FUNC "%s" _DEBUG_SEP_MSG, func_name);
        return true;
    }
    else if (IS_ACTIVE(CONFIG_DEBUG_SHOW_THREAD)) {
        printf(DEBUG_PREFIX _DEBUG_SEP_THREAD "%s" _DEBUG_SEP_MSG, __debug_thread_name_or_isr());
        return true;
    }
    return false;
}

static inline void __debug_put_prefix(const char *func_name)
{
    if (IS_ACTIVE(CONFIG_DEBUG_SHOW_FUNC) && IS_ACTIVE(CONFIG_DEBUG_SHOW_THREAD)) {
        fputs(_DEBUG_PREFIX_COLOR DEBUG_PREFIX _DEBUG_SEP_FUNC, stdout);
        fputs(func_name, stdout);
        fputs(_DEBUG_SEP_THREAD, stdout);
        fputs(__debug_thread_name_or_isr(), stdout);
        fputs(_DEBUG_SEP_MSG ANSI_COLOR_RESET, stdout);
    }
    else if (IS_ACTIVE(CONFIG_DEBUG_SHOW_FUNC)) {
        fputs(DEBUG_PREFIX _DEBUG_SEP_FUNC, stdout);
        fputs(func_name, stdout);
        fputs(_DEBUG_SEP_MSG, stdout);
    }
    else if (IS_ACTIVE(CONFIG_DEBUG_SHOW_THREAD)) {
        fputs(DEBUG_PREFIX _DEBUG_SEP_THREAD, stdout);
        fputs(__debug_thread_name_or_isr(), stdout);
        fputs(_DEBUG_SEP_MSG, stdout);
    }
    else {
        fputs(DEBUG_PREFIX _DEBUG_SEP_MSG, stdout); // todo: what if DEBUG_PREFIX not set, also below!
    }
}

// todo: update doc to mention `DEBUG_CONT`
/**
 * @def DEBUG
 *
 * @brief Print debug information to stdout
 *
 * Use this macro similarly to `printf` when starting a new line.
 * Remember to end the line with an explicit newline character `\n`.
 * If you instead want to continue writing to the same line afterwards,
 * use @ref DEBUG_CONT for the subsequent calls (and end the line there).
 *
 * DEBUG macros will perform a crude check whether the current stack may be
 * big enough for a call to `printf` when `DEVELHELP` is defined.
 *
 * @note    This looks similar to the @ref LOG_DEBUG() function. However, it is
 *          enabled on a per-file basis. Prefer @ref DEBUG for debug output
 *          relevant for debugging a module in RIOT. Prefer @ref LOG_DEBUG() for
 *          debug output relevant for application developers using your module
 *          (e.g. to hint potentially incorrect / inefficient use of your
 *          library).
 * @details If a variable is only accessed by `DEBUG()`, the compiler will
 *          warn about unused variables when `ENABLE_DEBUG` is set to `0`.
 */
#define DEBUG(...)                                               \
    do {                                                         \
        if (ENABLE_DEBUG && __debug_sufficient_stack(true)) {    \
            if (__debug_print_prefix(DEBUG_FUNC)) {              \
                printf(__VA_ARGS__);                             \
            }                                                    \
            else {                                               \
                printf(DEBUG_PREFIX _DEBUG_SEP_MSG __VA_ARGS__); \
            }                                                    \
        }                                                        \
    } while (0)

/**
 * @def DEBUG_CONT
 *
 * @brief Continue printing debug information to stdout
 *
 * Use this macro the same way as `printf` if you want to continue printing to the
 * same line that has been started with @ref DEBUG previously.
 */
#define DEBUG_CONT(...)                                        \
    do {                                                       \
        if (ENABLE_DEBUG && __debug_sufficient_stack(false)) { \
            printf(__VA_ARGS__);                               \
        }                                                      \
    } while (0)

/**
 * @def DEBUG_PUTS
 *
 * @brief Print debug information to stdout using puts(), so no stack size
 *        restrictions do apply.
 */
#define DEBUG_PUTS(str)                     \
    do {                                    \
        if (ENABLE_DEBUG) {                 \
            __debug_put_prefix(DEBUG_FUNC); \
            puts(str);                      \
        }                                   \
    } while (0)
/** @} */

/**
 * @def DEBUG_PRINT
 *
 * @deprecated use @ref DEBUG instead. Will be removed after release 2027.04.
 */
#define DEBUG_PRINT(...) DEBUG(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

/** @} */
