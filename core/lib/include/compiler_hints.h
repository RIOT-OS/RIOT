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

#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def NORETURN
 * @brief The *NORETURN* keyword tells the compiler to assume that the function
 *        cannot return.
 */
#ifndef NORETURN
#ifdef __GNUC__
#define NORETURN  __attribute__((noreturn))
#else
#define NORETURN
#endif
#endif

/**
 * @def PURE
 * @brief The function has no effects except the return value and its return
 *        value depends only on the parameters and/or global variables. Such a
 *        function can be subject to common subexpression elimination and loop
 *        optimization just as an arithmetic operator would be.
 */
#ifndef PURE
#ifdef __GNUC__
#define PURE  __attribute__((pure))
#else
#define PURE
#endif
#endif

/**
 * @def MAYBE_UNUSED
 * @brief tell the compiler something may be unused
 *        static functions, function arguments, local variables
 */
#ifndef MAYBE_UNUSED
#ifdef __GNUC__
#define MAYBE_UNUSED  __attribute__((unused))
#else
#define MAYBE_UNUSED
#endif
#endif

/**
 * @def       NO_SANITIZE_ARRAY
 * @brief     Tell the compiler that this array should be ignored during sanitizing.
 * @details   In special cases, e.g. XFA, the address sanitizer might interfere
 *            in a way that breaks the application. Use this macro to disable
 *            address sanitizing for a given variable. Currently only utilised
 *            by llvm.
 */
#if defined(__llvm__) || defined(__clang__)
#define NO_SANITIZE_ARRAY __attribute__((no_sanitize("address")))
#else
#define NO_SANITIZE_ARRAY
#endif

/**
 * @def       UNREACHABLE()
 * @brief     Tell the compiler that this line of code cannot be reached.
 * @details   Most useful in junction with #NORETURN.
 *            Use this if the compiler cannot tell that e.g.
 *            an assembler instruction causes a longjmp, or a write causes a reboot.
 */
#if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ >= 5) || defined(__clang__)
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

/**
 * @brief   Hint to the compiler that the condition @p x is likely taken
 *
 * @param[in] x     Condition that is likely taken
 *
 * @return result of @p x
 */
#define likely(x)       __builtin_expect((uintptr_t)(x), 1)

/**
 * @brief   Hint to the compiler that the condition @p x is likely not taken
 *
 * @param[in] x     Condition that is unlikely
 *
 * @return result of @p x
 */
#define unlikely(x)     __builtin_expect((uintptr_t)(x), 0)

/**
 * @brief   Behaves like an `assert()`, but tells the compiler that @p cond can
 *          never be false.
 *          This allows the compiler to optimize the code accordingly even when
 *          `NDEBUG` is set / with `DEVELHELP=0`.
 *
 *          @p cond being false will result in undefined behavior.
 *
 * @param[in] cond  Condition that is guaranteed to be true
 */
#ifdef NDEBUG
#define assume(cond)    ((cond) ? (void)0 : UNREACHABLE())
#else
#define assume(cond)    assert(cond)
#endif

/**
 * @brief   Wrapper function to silence "comparison is always false due to limited
 *          range of data type" type of warning when the warning is caused by a
 *          preprocessor configuration value that may be zero.
 *
 * @param[in]   n   Variable that may be zero
 * @return      The same variable @p n
 */
static inline unsigned may_be_zero(unsigned n)
{
    return n;
}

#ifdef __cplusplus
}
#endif

#endif /* COMPILER_HINTS_H */
/** @} */
