/*
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 *
 * @{
 * @file
 * @brief       POSIX.1-2008 compliant version of the assert macro
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef ASSERT_H
#define ASSERT_H

#include "panic.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   the string that is passed to panic in case of a failing assertion
 */
extern const char assert_crash_message[];

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
 */

#ifdef NDEBUG
#define assert(ignore)((void) 0)
#else
#define assert(cond) ((cond) ? (void)0 : core_panic(PANIC_ASSERT_FAIL, assert_crash_message))
#endif

#ifdef __cplusplus
}
#endif

#endif /* ASSERT_H */
/** @} */
