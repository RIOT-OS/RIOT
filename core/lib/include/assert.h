/*
 * Copyright (C) 2015 INRIA
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     core_util
 *
 * @{
 * @file
 * @brief       POSIX.1-2008 compliant version of the assert macro
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN
/**
 * @brief   Activate verbose output for @ref assert() when defined.
 *
 * Without this macro defined, @ref assert() will just print the address of the
 * code line the assertion failed in. With this macro defined, @ref assert()
 * will also print the file and the code line of the failed assertion.
 *
 * Enabling verbose output will on the other hand lead to an increased size of
 * the binary, since it needs to contain the names of all files with assertions.
 *
 * To define just add it to your `CFLAGS` in your application's Makefile:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * CFLAGS += -DDEBUG_ASSERT_VERBOSE
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#  define DEBUG_ASSERT_VERBOSE

/**
 * @brief   Activate breakpoints for @ref assert() when defined
 *
 * Without this macro defined the @ref assert() macro will just print some
 * information about the failed assertion, see @ref assert() and
 * @ref DEBUG_ASSERT_VERBOSE.
 * If @ref DEBUG_ASSERT_BREAKPOINT is defined, the execution will stop on a
 * failed assertion instead of producing the output. If the architecture
 * defines the macro @ref DEBUG_BREAKPOINT, a breakpoint is inserted and the
 * execution is stopped directly in the debugger. Otherwise the execution stops
 * in an endless while loop.
 */
#  define DEBUG_ASSERT_BREAKPOINT
#else
/* we should not include custom headers in standard headers */
#  define _likely(x)      __builtin_expect((uintptr_t)(x), 1)
#endif

#ifndef __NORETURN
#  if defined(__GNUC__) || defined(DOXYGEN)
/**
 * @brief   Same as @ref NORETURN
 * @internal
 *
 * We are not using @ref NORETURN from `compiler_hints.h` here to avoid RIOT
 * dependencies for standard C headers
 */
#    define __NORETURN __attribute__((noreturn))
#  else
#    define __NORETURN
#  endif
#endif

/**
 * @brief   Internal function to trigger a panic with a failed assertion as
 *          identifying cause
 * @internal
 * @warning This is an internal function. API changes may happen without regard
 *          for out-of tree uses.
 *
 * The implementation will identify the cause of the panic as a blown assertion,
 * e.g. via a log output.
 */
__NORETURN void _assert_panic(void);

/**
 * @brief   Function to handle failed assertion
 *
 * @note    This function was introduced for memory size optimization
 *
 * @warning this function **NEVER** returns!
 *
 * @param[in] file  The file name of the file the assertion failed in
 * @param[in] line  The code line of @p file the assertion failed in
 */
__NORETURN void _assert_failure(const char *file, unsigned line);

#ifdef NDEBUG
#  define assert(ignore)((void)0)
#elif defined(DEBUG_ASSERT_VERBOSE)
/**
 * @brief    abort the program if assertion is false
 *
 * If the macro NDEBUG was defined at the moment <assert.h> was last included,
 * the macro assert() generates no code, and hence does nothing at all.
 *
 * Otherwise, the macro assert() prints an error message to standard error and
 * terminates the application by calling core_panic().
 *
 * The purpose of this macro is to help programmers find bugs in their
 * programs.
 *
 * A failed assertion generates output similar to:
 *
 *     0x89abcdef => FAILED ASSERTION.
 *
 * Where 0x89abcdef is an address. This address can be used with tools like
 * `addr2line` (or e.g. `arm-none-eabi-addr2line` for ARM-based code), `objdump`,
 * or `gdb` (with the command `info line *(0x89abcdef)`) to identify the line
 * the assertion failed in.
 *
 * With @ref DEBUG_ASSERT_VERBOSE defined this will instead directly print the
 * file and the line the assertion failed in:
 *
 *     example/file.c:42 => FAILED ASSERTION.
 *
 * If the `backtrace` module is enabled (and implemented for the architecture in use)
 * a backtrace will be printed in addition to the location of the failed assertion.
 *
 * If @ref DEBUG_ASSERT_BREAKPOINT is defined, the execution will stop on a
 * failed assertion instead of producing the above output. If the architecture
 * defines the macro @ref DEBUG_BREAKPOINT, a breakpoint is inserted and the
 * execution is stopped directly in the debugger. Otherwise the execution stops
 * in an endless while loop.
 *
 * @see http://pubs.opengroup.org/onlinepubs/9699919799/functions/assert.html
 */
#  define assert(cond) (_likely(cond) ? (void)0 :  _assert_failure(__FILE__, __LINE__))
#else /* DEBUG_ASSERT_VERBOSE */
#  define assert(cond) (_likely(cond) ? (void)0 : _assert_panic())
#endif /* DEBUG_ASSERT_VERBOSE */

#if !defined __cplusplus
#  if __STDC_VERSION__ >= 201112L
/**
 * @brief c11 static_assert() macro
 */
#    define static_assert(...) _Static_assert(__VA_ARGS__)
#  else
/**
 * @brief static_assert for c-version < c11
 *
 * Generates a division by zero compile error when cond is false
 */
#    define static_assert(cond, ...) \
        { enum { static_assert_failed_on_div_by_0 = 1 / (!!(cond)) }; }
#  endif
#endif

/**
 * @brief   Don't panic on a failed assertion, just halt the running thread.
 *
 * If the assertion failed in an interrupt, the system will still panic.
 */
#ifndef DEBUG_ASSERT_NO_PANIC
#  define DEBUG_ASSERT_NO_PANIC (1)
#endif

#ifdef __cplusplus
}
#endif

/** @} */
