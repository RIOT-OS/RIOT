/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 HAW-Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     core_internal
 * @{
 *
 * @file
 * @brief       Common macros and compiler attributes/pragmas configuration
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def         IS_ACTIVE(macro)
 * @brief       Allows to verify a macro definition outside the preprocessor.
 *
 * @details     This macro is based on Linux's clever 'IS_BUILTIN'
 *              (https://github.com/torvalds/linux/blob/master/include/linux/kconfig.h).
 *              It takes a @p macro value that may be defined to 1 or not even
 *              defined (e.g. FEATURE_FOO) and then expands it to an expression
 *              that can be used in C code, either 1 or 0.
 *
 *              The advantage of using this is that the compiler sees all the
 *              code, so checks can be performed, sections that would not be
 *              executed are removed during optimization. For example:
 * ```
 * if (IS_ACTIVE(FEATURE_FOO)) {
 *      do_something();
 * }
 * ```
 * @param[in]   macro   Macro to evaluate
 * @returns     1 if the macro is defined to 1
 * @returns     0 if the macro is not defined, of if it is defined to something
 *              else than 1.
 *
 * @note        This should only be used when macros are defined as 1, it will
 *              not work if the macro value is, for example, (1) or 1U.
 *
 * @note        Although this may seem to work similarly to the preprocessor's
 *              'defined', it is not entirely equal. If the given macro has
 *              been defined with no value, this will expand to 0. Also note
 *              that this is intended to be used with 'boolean' macros that act
 *              as switches, and usually will be defined as 1 or not defined.
 */
#define IS_ACTIVE(macro) __is_active(macro)

/**
 * @def         IS_USED(module)
 * @brief       Checks whether a module is being used or not. Can be used in C
 *              conditionals.
 *
 * @param[in]   module   Module to check
 * @returns     1 if the module is being used
 * @returns     0 if the module is not being used
 */
#define IS_USED(module) IS_ACTIVE(module)

/**
 * @cond INTERNAL
 */
/* Here a prefix "__PREFIX_WHEN_" is added to the macro. So if it was a 1 we
 * have "__PREFIX_WHEN_1", and if it was not defined we have "__PREFIX_WHEN_".
 */
#define __is_active(val)              ___is_active(__PREFIX_WHEN_##val)

/* With this placeholder we turn the original value into two arguments when the
 * original value was defined as 1 (note the comma).
 */
#define __PREFIX_WHEN_1 0,

/* Here we add two extra arguments, that way the next macro can accept varargs.
 *
 * If the original macro was defined as 1, this will have three arguments
 * (__take_second_arg(0, 1, 0, 0)), otherwise it will have two
 * (__take_second_arg(__PREFIX_WHEN_ 1, 0, 0)). The third zero is there just to
 * be compliant with C99, which states that when a function-like macro ends
 * with ellipsis (...) it should be called with at least one argument for the
 * variable list.
 */
#define ___is_active(arg1_or_junk)    __take_second_arg(arg1_or_junk 1, 0, 0)

/* Finally, we just always take the second argument, which will be either 1
 * (when three arguments are passed, i.e. macro was defined as 1) or 0 (when
 * only two arguments are passed).
 */
#define __take_second_arg(__ignored, val, ...) val
/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif

/** @} */
