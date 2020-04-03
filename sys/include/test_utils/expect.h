/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2015 INRIA
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    test_utils_expect expect() utility function
 * @ingroup     sys
 *
 * @{
 * @file
 * @brief       test "expect condition" utility function
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef TEST_UTILS_EXPECT_H
#define TEST_UTILS_EXPECT_H

#include <stdio.h>
#include "panic.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RIOT_FILE_RELATIVE
#define RIOT_FILE_RELATIVE  (__FILE__)
#endif

/**
 * @brief   Function to handle failed expectation
 *
 * @note    This function was introduced for memory size optimization
 *
 * @warning this function **NEVER** returns!
 *
 * @param[in] file  The file name of the file the expectation failed in
 * @param[in] line  The code line of @p file the expectation failed in
 */
NORETURN static inline void _expect_failure(const char *file, unsigned line)
{
    printf("%s:%u => failed condition\n", file, line);
    core_panic(PANIC_EXPECT_FAIL, "CONDITION FAILED.");
}

/**
 * @brief    abort the program if condition is false
 *
 * This is similar to assert(), but will not be excluded from a build even if
 * NDEBUG is set. Use e.g., in test application to "assert" conditions, in
 * order to prevent a different compilation mode (a release build?) from making
 * the test non-functional.
 *
 * Otherwise, the macro expect() prints an error message to standard error and
 * terminates the application by calling core_panic().
 *
 * The purpose of this macro is to help programmers find bugs in their
 * programs.
 *
 * A failed condition generates output similar to:
 *
 *     0x89abcdef
 *     *** RIOT kernel panic:
 *     FAILED CONDITION.
 *
 *     ...
 *
 * Where 0x89abcdef is an address. This address can be used with tools like
 * `addr2line` (or e.g. `arm-none-eabi-addr2line` for ARM-based code), `objdump`,
 * or `gdb` (with the command `info line *(0x89abcdef)`) to identify the line
 * the condition failed in.
 *
 */
#define expect(cond) ((cond) ? (void)0 :  _expect_failure(RIOT_FILE_RELATIVE, \
                                                          __LINE__))

#ifdef __cplusplus
}
#endif

#endif /* TEST_UTILS_EXPECT_H */
/** @} */
