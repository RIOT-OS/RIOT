/*
 * Copyright (C) 2015 INRIA
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef ASSERT_H
#define ASSERT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN
/**
 * @brief   Activate verbose output for @ref assert() when defined.
 *
 * Without this macro defined the @ref assert() macro will just print the
 * address of the code line the assertion failed in. With the macro defined
 * the macro will also print the file, the code line and the function this macro
 * failed in.
 *
 * To define just add it to your `CFLAGS` in your application's Makefile:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * CFLAGS += -DDEBUG_ASSERT_VERBOSE
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define DEBUG_ASSERT_VERBOSE

/**
 * @brief   Activate breakpoints for @ref assert() when defined
 *
 * Without this macro defined the @ref assert() macro will just print some
 * information about the failed assertion, see @ref assert and
 * @ref DEBUG_ASSERT_VERBOSE.
 * If @ref DEBUG_ASSERT_BREAKPOINT is defined, the execution will stop on a
 * failed assertion instead of producing the output. If the architecture
 * defines the macro @ref DEBUG_BREAKPOINT, a breakpoint is inserted and the
 * execution is stopped directly in the debugger. Otherwise the execution stops
 * in an endless while loop.
 */
#define DEBUG_ASSERT_BREAKPOINT

/**
 * @brief   abort the program if assertion is false
 *
 * @warning Do not use `assert(0);` to assert that a line of code is
 *          unreachable, @ref assert_unreachable instead.
 *
 * @note    This implementation has a special twist: It will fail at link time
 *          if the condition can be proven to be never true at compile time.
 *          If linking fails due to the symbol
 *          `__assertion_is_known_to_fail_at_compile_time` being undefined,
 *          an assertion has been proven to always blow by the optimizer.
 *
 * @note    Further note that when packages include this header, the check
 *          will always be performed at runtime, as replacing all
 *          `assert(0);` in packages with `assert_unreachable();` are not
 *          maintainable.
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
 * With @ref DEBUG_ASSERT_VERBOSE defined this will print also the file, the
 * line and the function this assertion failed in.
 *
 * If `NDEBUG` and @ref DEBUG_ASSERT_VERBOSE are not defined, a failed assertion
 * generates output similar to:
 *
 *     0x89abcdef
 *     *** RIOT kernel panic:
 *     FAILED ASSERTION.
 *
 *     ...
 *
 * Where 0x89abcdef is an address. This address can be used with tools like
 * `addr2line` (or e.g. `arm-none-eabi-addr2line` for ARM-based code), `objdump`,
 * or `gdb` (with the command `info line *(0x89abcdef)`) to identify the line
 * the assertion failed in.
 *
 * If the `backtrace` module is enabled (and implemented for architecture in use)
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
#  define assert(cond) /* implementation */

/**
 * @brief   abort the program if this line is reached, unless `NDEBUG` is
 *          defined
 */
#  define assert_unreachable() /* implementation */
#else
/* we should not include custom headers in standard headers */
#  define _likely(x)    __builtin_expect((uintptr_t)(x), 1)
#endif

/**
 * @def __NORETURN
 * @brief hidden (__) NORETURN definition
 * @internal
 *
 * Duplicating the definitions of kernel_defines.h as these are unsuitable for
 * system header files like the assert.h.
 * kernel_defines.h would define symbols that are not reserved.
 */
#ifndef __NORETURN
#  ifdef __GNUC__
#    define __NORETURN __attribute__((noreturn))
#  else /*__GNUC__*/
#    define __NORETURN
#  endif /*__GNUC__*/
#endif /*__NORETURN*/

/**
 * @brief   Function to report a failed assertion - verbose variant
 *
 * @warning This function is internal and should not be used by downstream code.
 *
 * @note    This function was introduced for memory size optimization
 *
 * @warning this function **NEVER** returns!
 *
 * @param[in] file  The file name of the file the assertion failed in
 * @param[in] line  The code line of @p file the assertion failed in
 */
__NORETURN void _assert_panic_verbose(const char *file, unsigned line);

/**
 * @brief   Concise version of reporting a blown assertion
 *
 * @warning This function is internal and should not be used by downstream code.
 *
 * This is an alternative to @ref _assert_panic_verbose that doesn't require
 * the caller to spent memory in `.rodata` for a file name and a line number.
 */
__NORETURN void _assert_panic_concise(void);

#ifdef NDEBUG
#  define assert(ignore)                ((void)0)
#  define assert_unreachable()          UNREACHABLE()
#else
#  if DEBUG_ASSERT_VERBOSE
#    define _assert_panic(file, line) _assert_panic_verbose(file, line)
#  else
#    define _assert_panic(file, line) _assert_panic_concise()
#  endif
#  if COMPILING_RIOT_PKG_CODE
#    define assert(cond) \
        (_likely(cond) ? (void)0 : _assert_panic(__FILE__, __LINE__))
#  else
#    define assert(cond) \
        do { \
            if (__builtin_constant_p(cond)) { \
                if (!(cond)) { \
                    extern void __assertion_is_known_to_fail_at_compile_time(void); \
                    __assertion_is_known_to_fail_at_compile_time(); \
                } \
            } \
            else { \
                if (!(_likely(cond))) { \
                    _assert_panic(__FILE__, __LINE__); \
                } \
            } \
        } \
        while (0)
#  endif
#  define assert_unreachable() \
        do { \
            _assert_panic(__FILE__, __LINE__); \
        } while (0)
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* ASSERT_H */
/** @} */
