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
#include "log.h"

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
#  define DEBUG_EXTRA_STACKSIZE THREAD_EXTRA_STACKSIZE_PRINTF
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


/**
 * @brief   Contains the function name if compiler supports it.
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
 * @brief   Contains the file path if compiler supports it.
 *          Otherwise it is an empty string.
 */
#if defined(__FILE__) || defined(DOXYGEN)
#  define DEBUG_FILE_PATH __FILE__
#else
#  define DEBUG_FILE_PATH ""
#endif

/**
 * @brief   Contains the file name if compiler supports it.
 *          Otherwise it is an empty string.
 */
#if defined(__FILE_NAME__) || defined(DOXYGEN)
#  define DEBUG_FILE_NAME __FILE_NAME__
#else
#  define DEBUG_FILE_NAME ""
#endif


/**
 * @brief   Contains the file line number if compiler supports it.
 *          Otherwise it is an empty string.
 */
#if defined(__LINE__) || defined(DOXYGEN)
#  define DEBUG_LINE __LINE__
#else
#  define DEBUG_LINE ""
#endif

/** @} */ /* end of section */

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
    /* DO NOT call any function here that invokes DEBUG OR LOG in here. */
#if IS_ACTIVE(DEVELHELP)
    const thread_t *thread = thread_get_active();
    if (((thread != NULL) && (thread->stack_size < THREAD_EXTRA_STACKSIZE_PRINTF)) ||
#  ifdef ISR_STACKSIZE
        (irq_is_in() && (ISR_STACKSIZE < THREAD_EXTRA_STACKSIZE_PRINTF))) {
#  else
        false) {
#  endif
        if (print) {
            puts("Cannot debug, stack too small."
                 "Consider using DEBUG_PUTS() or increasing the stack size.");
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

/** @} */ /* end of section */

/**
 * @name User-facing debug print API
 * @{
 */

#define DEBUG(...) do { __LOG_PROLOGUE                                                                 \
        if ((ENABLE_DEBUG || _CAN_DEBUG_H(LOG_DEBUG, LOG_UNIT)) && __debug_sufficient_stack(false)) {        \
            log_write(LOG_DEBUG, LOG_UNIT, __VA_ARGS__);                                                                \
        }                                                                                          \
    } while (0) __LOG_EPILOGUE

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
#define DEBUG_CONT(...) do { __LOG_PROLOGUE                                                                      \
        if ((ENABLE_DEBUG || _CAN_DEBUG_H(LOG_DEBUG, LOG_UNIT)) && __debug_sufficient_stack(false)) {\
            log_write_continue(LOG_DEBUG, LOG_UNIT, __VA_ARGS__);                                                      \
        }                                                                                          \
    } while (0) __LOG_EPILOGUE


#define DEBUG_PUTS(str) do { __LOG_PROLOGUE                                                                \
        if ((ENABLE_DEBUG || _CAN_DEBUG_H(LOG_DEBUG, LOG_UNIT)) && __debug_sufficient_stack(false)) {\
            log_write(LOG_DEBUG, LOG_UNIT, str "\n");                                                           \
        }                                                                                          \
    } while (0) __LOG_EPILOGUE

/**
 * @deprecated use @ref DEBUG instead. Will be removed after release 2027.04.
 */
#define DEBUG_PRINT(...) DEBUG(__VA_ARGS__)

/** @} */ /* end of section */

#ifdef __cplusplus
}
#endif

/** @} */ /* end of group */
