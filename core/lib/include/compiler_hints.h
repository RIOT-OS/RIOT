/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2017 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef COMPILER_HINTS_H
#define COMPILER_HINTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def NORETURN
 * @brief The *NORETURN* keyword tells the compiler to assume that the function
 *        cannot return.
 */
#ifdef __GNUC__
#define NORETURN  __attribute__((noreturn))
#else
#define NORETURN
#endif

/**
 * @def PURE
 * @brief The function has no effects except the return value and its return
 *        value depends only on the parameters and/or global variables. Such a
 *        function can be subject to common subexpression elimination and loop
 *        optimization just as an arithmetic operator would be.
 */
#ifdef __GNUC__
#define PURE  __attribute__((pure))
#else
#define PURE
#endif

/**
 * @def       UNREACHABLE()
 * @brief     Tell the compiler that this line of code cannot be reached.
 * @details   Most useful in junction with #NORETURN.
 *            Use this if the compiler cannot tell that e.g.
 *            an assembler instruction causes a longjmp, or a write causes a reboot.
 */
#if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ >= 5)
#define UNREACHABLE() __builtin_unreachable()
#else
#define UNREACHABLE() do { /* nothing */ } while (1)
#endif

/**
 * @brief   Disable -Wpedantic for the argument, but restore diagnostic
 *          settings afterwards
 * @param   ...     The expression that -Wpendantic should not apply to
 *
 * @warning This is intended for internal use only
 *
 * This is particularly useful when declaring non-strictly conforming
 * preprocessor macros, as the diagnostics need to be disabled where the
 * macro is evaluated, not where the macro is declared.
 */
#define WITHOUT_PEDANTIC(...) \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
    __VA_ARGS__ \
    _Pragma("GCC diagnostic pop")

/**
 * @brief   Declare a constant named @p identifier as anonymous `enum` that has
 *          the value @p const_expr
 *
 * @warning This is intended for internal use only
 *
 * This turns any expression that is constant and known at compile time into
 * a formal compile time constant. This allows e.g. using non-formally but
 * still constant expressions in `static_assert()`.
 */
#define DECLARE_CONSTANT(identifier, const_expr) \
    WITHOUT_PEDANTIC(enum { identifier = const_expr };)

#if DOXYGEN
/**
 * @brief   Check if given variable / expression is detected as compile time
 *          constant
 * @note    This might return 0 on compile time constant expressions if the
 *          compiler is not able to prove the constness at the given level
 *          of optimization.
 * @details This will return 0 if the used compiler does not support this
 * @warning This is intended for internal use only
 *
 * This allows providing two different implementations in C, with one being
 * more efficient if constant folding is used.
 */
#define IS_CT_CONSTANT(expr) <IMPLEMENTATION>
#elif defined(__GNUC__)
/* both clang and gcc (which both define __GNUC__) support this */
#define IS_CT_CONSTANT(expr) __builtin_constant_p(expr)
#else
#define IS_CT_CONSTANT(expr) 0
#endif

#ifdef __cplusplus
}
#endif

#endif /* COMPILER_HINTS_H */
/** @} */
