/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    core_util_debug Debugging
 * @ingroup     core_util
 * @brief       Macros for debugging and printing debug messages
 * @{
 */

/**
 * @file
 * @brief       Debug header
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * If *ENABLE_DEBUG* is defined inside an implementation file, all
 * calls to ::DEBUG will work the same as *printf* and output the
 * given information to stdout. If *ENABLE_DEBUG* is not defined,
 * all calls to ::DEBUG will be ignored.
 */

#include <stdio.h>
#include <string.h>

#include "ansi_style.h"
#include "irq.h"
#include "sched.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Breakpoints
 * @{
 */
/**
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
/** @} */ /* end of section */

/**
 * @name Configuring debug printing
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
 * @brief Extra stacksize needed when ENABLE_DEBUG==1
 *
 * @deprecated This macro definition does not work anyway as ENABLE_DEBUG
 *             is only set on file-level. Just remove its usages.
 *             Will be removed after release 2027.04.
 */
#if ENABLE_DEBUG
#  define DEBUG_EXTRA_STACKSIZE THREAD_EXTRA_STACKSIZE_PRINTF
#else
#  define DEBUG_EXTRA_STACKSIZE (0)
#endif

/**
 * @brief   Common prefix for all debug messages, defaulting to an empty string.
 *          Expected to be set on a file-based level.
 * 
 * Defaults to `""` unless @ref 
 */
#ifndef DEBUG_UNIT
#  ifdef LOG_UNIT
#    define DEBUG_UNIT LOG_UNIT
#  else
#    define DEBUG_UNIT ""
#  endif
#endif

#ifndef DOXYGEN
#  ifdef LOG_UNITS
#    define FORCE_DEBUG(unit) (                                                                      \
        (strlen(unit) > 0) && ({                                                                   \
            bool forced = false;                                                                   \
            for (unsigned int i = 0; i < ARRAY_SIZE((const char *[]){ LOG_UNITS }); i += 1) {      \
                forced |= strncmp(((const char*[]){ LOG_UNITS })[i], unit,                         \
                           strlen(((const char*[]){ LOG_UNITS })[i])) == 0;                        \
            }                                                                                      \
            forced;                                                                                \
        }))
#  else
#    define FORCE_DEBUG(unit) (false)
#  endif
#endif


/**
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
 * @brief   Specify whether calls to @ref DEBUG and @ref DEBUG_PUTS automatically
 *          include the calling thread name.
 *
 * @warning Only applies to files where @ref DEBUG_UNIT is non-empty.
 *
 * **Default**: disabled
 */
#ifndef CONFIG_DEBUG_SHOW_THREAD
#  define CONFIG_DEBUG_SHOW_THREAD 0
#endif

#if IS_ACTIVE(CONFIG_DEBUG_SHOW_THREAD) && !defined(CONFIG_THREAD_NAMES)
#  error "CONFIG_DEBUG_SHOW_THREAD requires CONFIG_THREAD_NAMES to be set"
#endif

/**
 * @brief   Specify whether calls to @ref DEBUG and @ref DEBUG_PUTS automatically
 *          include the current function name.
 *
 * @warning Only applies to files where @ref DEBUG_UNIT is non-empty.
 *
 * **Default**: disabled
 */
#if !defined(CONFIG_DEBUG_SHOW_FUNC) || defined(DOXYGEN)
#  define CONFIG_DEBUG_SHOW_FUNC 0
#endif

/**
 * @brief   Determines whether debug messages are printed like @ref LOG_DEBUG messages for visual
 *          consistency
 *
 * **Default**: enabled if @ref CONFIG_STDIO_ANSI_STYLING is
 */
#if !defined(CONFIG_DEBUG_LOG_COLOR_COMPAT) || defined(DOXYGEN)
#  define CONFIG_DEBUG_LOG_COLOR_COMPAT CONFIG_STDIO_ANSI_STYLING
#endif
/** @} */ /* end of section */

#if !defined(DOXYGEN)
#  define _DEBUG_STYLE_FOR_PREFIX                ANSI_STYLE(FOREGROUND_BRIGHT(CYAN), BOLD)
#  define _DEBUG_STYLE_FOR_THREAD_FUNC           ANSI_STYLE(FOREGROUND(WHITE), DIM)
#  if MODULE_LOG_COLOR
#    define _DEBUG_PREFIX                        ANSI_STYLE(BOLD) "DEBUG " ANSI_STYLE_RESET "# "
#  else
#    define _DEBUG_PREFIX                        ""
#  endif
#endif

/**
 * @name Debug print implementation details
 * @{
 */

/**
 * @brief Check whether the stack of the current thread (or ISR) is big enough in total
 *        for printf formatting when `DEVELHELP` is enabled.
 *
 * @warning This only checks for the whole stack size, not for the currently free part of it.
 *
 * @private
 *
 * @param    print              Whether to print a warning message
 * @retval   true               Stack is sufficiently big, or `DEVELHELP` is disabled
 * @retval   false              Stack is too small
 */
static inline bool __debug_sufficient_stack(bool print)
{
#if IS_ACTIVE(DEVELHELP)
    const thread_t *thread = thread_get_active();
    if (((thread != NULL) && (thread->stack_size < THREAD_EXTRA_STACKSIZE_PRINTF)) ||
#  ifdef ISR_STACKSIZE
        (irq_is_in() && (ISR_STACKSIZE < THREAD_EXTRA_STACKSIZE_PRINTF))) {
#  else
        false) {
#  endif
        if (print) {
            fputs("Cannot debug, stack too small."
                  "Consider using DEBUG_PUTS() or increasing the stack size.\n",
                  stdout);
        }
        return false;
    }
#endif /* IS_ACTIVE(DEVELHELP) */
    (void)print;
    return true;
}

/**
 * @brief Get thread name of the currently running thread, or "<isr>"
 *
 * @private
 *
 * @return   the thread name, or "<isr>"
 */
static inline const char *__debug_thread_name_or_isr(void)
{
    const thread_t *thread = thread_get_active();
    return (irq_is_in() || thread == NULL) ? "<isr>" : thread_get_name(thread);
}

/**
 * @brief Debug print implementation for `printf`-like functions
 *
 * @private
 * @experimental
 *
 * @param func Printf macro or function to use to to print
 * @param prefix String literal to be used as line prefix
 * @param ... Variadic arguments to `printf`
 *
 * Use this internal macro if you want to use a custom print function to debug. This implementation
 * expects a `print` function handling format strings. @p func is called once with a format string
 * for the entire line prefix and once for your message.
 * You may use your own print function, e.g., to send logs over the network.
 * Treat the remaining variadic arguments like you would treat `printf` arguments: format string
 * followed by format arguments.
 *
 * @remark If you intend to use this macro with `printf` as @p func,
 *         use @ref DEBUG or @ref DEBUG_ instead.
 *
 * ## Example
 * ```c
 * #define __fprintf_err(...) fprintf(stderr, __VA_ARGS__)
 * #define COMPLAIN(...) __DEBUG_IMPL_FORMATTED(__fprintf_err, "custom-prefix", __VA_ARGS__)
 * ```
 *
 * Experimentally, you can also define @ref DEBUG_ to customize the function used, in which
 * case `debug.h` will not define @ref DEBUG_, defaulting to your provided definition. @ref DEBUG
 * always uses your implementation via @ref DEBUG_.
 *
 * ```c
 * #define DEBUG_(prefix, ...) __DEBUG_IMPL_FORMATTED(__fprintf_err, prefix, __VA_ARGS__)
 * ```
 */
#define __DEBUG_IMPL_FORMATTED(func, prefix, ...)                                                                     \
    do {                                                                                        \
        if ((ENABLE_DEBUG || FORCE_DEBUG(prefix)) && __debug_sufficient_stack(true)) {                                   \
            if (strlen(prefix) > 0) {                                                           \
                if (IS_ACTIVE(CONFIG_DEBUG_SHOW_FUNC) && IS_ACTIVE(CONFIG_DEBUG_SHOW_THREAD)) { \
                    func(_DEBUG_PREFIX _DEBUG_STYLE_FOR_PREFIX prefix _DEBUG_STYLE_FOR_THREAD_FUNC            \
                           " (%s@%s): " ANSI_STYLE_RESET,                                       \
                           DEBUG_FUNC, __debug_thread_name_or_isr());                           \
                }                                                                               \
                else if (IS_ACTIVE(CONFIG_DEBUG_SHOW_FUNC)) {                                   \
                    func(_DEBUG_PREFIX _DEBUG_STYLE_FOR_PREFIX prefix _DEBUG_STYLE_FOR_THREAD_FUNC            \
                           " (%s): " ANSI_STYLE_RESET,                                          \
                           DEBUG_FUNC);                                                         \
                }                                                                               \
                else if (IS_ACTIVE(CONFIG_DEBUG_SHOW_THREAD)) {                                 \
                    func(_DEBUG_PREFIX _DEBUG_STYLE_FOR_PREFIX prefix _DEBUG_STYLE_FOR_THREAD_FUNC            \
                           " (@%s): " ANSI_STYLE_RESET,                                         \
                           __debug_thread_name_or_isr());                                       \
                }                                                                               \
                else {                                                                          \
                    func(_DEBUG_PREFIX _DEBUG_STYLE_FOR_PREFIX prefix _DEBUG_STYLE_FOR_THREAD_FUNC            \
                           ": " ANSI_STYLE_RESET);                                              \
                }                                                                               \
            }                                                                                   \
            func(__VA_ARGS__);                                                                  \
        }                                                                                       \
    } while (0)

/**
 * @brief Debug print implementation for continuing debug message without printing prefix again
 *
 * @private
 * @experimental
 *
 * @param func Printf macro or function to use to to print
 * @param ... Variadic arguments to `printf`
 *
 * Use this macro the same way as `printf` if you want to continue printing to the
 * same line that has been started with @ref DEBUG previously. For this to work, do not append
 * newline sequence (`\n`) in previous @ref DEBUG call.
 */
#define __DEBUG_IMPL_CONT(func, ...)                                        \
    do {                                                       \
        if (ENABLE_DEBUG && __debug_sufficient_stack(false)) { \
            func(__VA_ARGS__);                               \
        }                                                      \
    } while (0)

/**
 * @brief Debug print implementation for `puts`-like functions
 *
 * @private
 * @experimental
 *
 * @param func Printf macro or function to use to to print
 * @param prefix String literal to be used as line prefix
 * @param str A message string, does not need to be constant
 * @param ... Variadic arguments passed to @p on every invocation
 *
 * Use this internal macro if you want to use a custom print function to debug. This implementation
 * expects a function that accepts a string literal, and optionally the variadic arguments passed.
 * @p func is called for each piece of the debug message printed, depending on internal formatting
 * of the prefix printed. You may use your own print function, e.g., to send logs over the network.
 *
 * @remark If you intend to use this macro with `puts` as @p func,
 *         use @ref DEBUG_PUTS or @ref DEBUG_PUTS_ instead.
 *
 * ## Example
 * ```c
 * #define COMPLAIN_PUTS(str) __DEBUG_IMPL_PIECEWISE(fputs, "custom-prefix", str, stderr)
 * ```
 *
 * Experimentally, you can also define @ref DEBUG_PUTS_ to customize the function used, in which
 * case `debug.h` will not define @ref DEBUG_PUTS_, defaulting to your provided definition.
 * @ref DEBUG_PUTS always uses your implementation via @ref DEBUG_PUTS_.
 *
 * ```c
 * #define DEBUG_PUTS_(prefix, str) __DEBUG_IMPL_PIECEWISE(fputs, prefix, str, stderr)
 * ```
 */
#define __DEBUG_IMPL_PIECEWISE(func, prefix, str, ...)                                          \
    do {                                                                                        \
        if (ENABLE_DEBUG || FORCE_DEBUG(prefix)) {                                                                     \
            if (strlen(prefix) > 0) {                                                           \
                func(_DEBUG_PREFIX _DEBUG_STYLE_FOR_PREFIX prefix, ##__VA_ARGS__);                            \
                if (IS_ACTIVE(CONFIG_DEBUG_SHOW_FUNC) || IS_ACTIVE(CONFIG_DEBUG_SHOW_THREAD)) { \
                    func(_DEBUG_STYLE_FOR_THREAD_FUNC " (", ##__VA_ARGS__);                     \
                }                                                                               \
                if (IS_ACTIVE(CONFIG_DEBUG_SHOW_FUNC)) {                                        \
                    func(DEBUG_FUNC, ##__VA_ARGS__);                                            \
                }                                                                               \
                if (IS_ACTIVE(CONFIG_DEBUG_SHOW_THREAD)) {                                      \
                    func("@", ##__VA_ARGS__);                                                   \
                    func(__debug_thread_name_or_isr(), ##__VA_ARGS__);                          \
                }                                                                               \
                if (IS_ACTIVE(CONFIG_DEBUG_SHOW_FUNC) || IS_ACTIVE(CONFIG_DEBUG_SHOW_THREAD)) { \
                    func(")", ##__VA_ARGS__);                                                   \
                }                                                                               \
                func(": " ANSI_STYLE_RESET, ##__VA_ARGS__);                                     \
            }                                                                                   \
            func(str, ##__VA_ARGS__);                                                           \
            func("\n", ##__VA_ARGS__);                                                          \
        }                                                                                       \
    } while (0)

/** @} */ /* end of section */

/**
 * @name User-facing debug print API
 * @{
 */

/**
 * @brief Print debug information to the standard output stream with a custom prefix
 *
 * @param prefix String literal to be used as line prefix
 * @param ... Variadic arguments compatible with `printf`
 *
 * Use this internal macro if you want to have something more fine-grained than
 * the file-wide @ref DEBUG_UNIT, to define your own debug function like
 *
 * ```c
 * #define CUSTOM_DEBUG(...) DEBUG_("custom-prefix", __VA_ARGS__)
 * ```
 *
 * Otherwise, just use @ref DEBUG.
 *
 * Experimentally, you may define your own version of this macro to provide a custom debug printing
 * backend, in which case this function will not be defined by `debug.h`. This must be done
 * in conjunction with defining @ref DEBUG_CONT.
 */
#if !defined(DEBUG_) || defined(DOXYGEN)
#  define DEBUG_(prefix, ...) __DEBUG_IMPL_FORMATTED(printf, prefix, __VA_ARGS__)
#endif

/**
 * @brief Print debug information to the standard output stream
 *
 * Use this macro similarly to `printf` when starting a new line.
 * Remember to end the line with an explicit newline character `\n`.
 * This will prefix the print with @ref DEBUG_UNIT. Therefore,
 * if you want to continue writing to the same line afterwards,
 * use @ref DEBUG_CONT for subsequent calls (and end the line there).
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
 * @warning If a variable is only accessed by `DEBUG()`, the compiler will
 *          warn about unused variables when `ENABLE_DEBUG` is set to `0`.
 *
 * Make use of @ref DEBUG_ if you need to use a custom prefix.
 */
#define DEBUG(...) DEBUG_(DEBUG_UNIT, __VA_ARGS__)

/**
 * @brief Continue printing debug information to stdout, without repeating the prefix
 *
 * Use this macro the same way as `printf` if you want to continue printing to the
 * same line that has been started with @ref DEBUG previously.
 *
 * Experimentally, you may define your own version of this macro to provide a custom debug printing
 * backend, in which case this function will not be defined by `debug.h`. This must be done
 * in conjunction with defining @ref DEBUG_.
 */
#if !defined(DEBUG_CONT) || defined(DOXYGEN)
#  define DEBUG_CONT(...) __DEBUG_IMPL_CONT(printf, __VA_ARGS__)
#endif

/**
 * @brief Print debug information to the standard output stream using puts() with a custom prefix.
 *
 * @param prefix String literal to be used as line prefix
 * @param str A message string, does not need to be constant
 *
 * Use this internal macro if you want to have something more fine-grained than
 * the file-wide @ref DEBUG_UNIT, to define your own debug function like
 *
 * ```c
 * #define CUSTOM_DEBUG_PUTS(str) DEBUG_PUTS_("custom-prefix", str)
 * ```
 *
 * Otherwise, just use @ref DEBUG_PUTS.
 *
 * Experimentally, you may define your own version of this macro to provide a custom debug printing
 * backend, in which case this function will not be defined by `debug.h`.
 */
#if !defined(DEBUG_PUTS_) || defined(DOXYGEN)
#  define DEBUG_PUTS_(prefix, str) __DEBUG_IMPL_PIECEWISE(fputs, prefix, str, stdout)
#endif

/**
 * @brief Print debug information to standard output stream using puts(), so no stack size
 *        restrictions do apply.
 *
 * @param str A message string, does not need to be constant
 *
 * Make use of @ref DEBUG_PUTS_ if you need to use a custom prefix.
 */
#define DEBUG_PUTS(str) DEBUG_PUTS_(DEBUG_UNIT, str)

/**
 * @deprecated use @ref DEBUG instead. Will be removed after release 2027.04.
 */
#define DEBUG_PRINT(...) DEBUG(__VA_ARGS__)

/** @} */ /* end of section */

#ifdef __cplusplus
}
#endif

/** @} */ /* end of group */
