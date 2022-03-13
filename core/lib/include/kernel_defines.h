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

#ifndef KERNEL_DEFINES_H
#define KERNEL_DEFINES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* uncrustify gets mightily confused by these macros... */
/* begin{code-style-ignore} */

/**
 * @def         container_of(PTR, TYPE, MEMBER)
 * @brief       Returns the container of a pointer to a member.
 * @details     For a struct `TYPE` with a member `MEMBER`,
 *              given a pointer `PTR` to `TYPE::MEMBER` this function returns a pointer
 *              to the instance of `TYPE`.
 * @details     E.g. for `struct my_struct_t { ...; something_t n; ... } my_struct;`,
 *              `&my_struct == container_of(&my_struct.n, struct my_struct_t, n)`.
 * @param[in]   PTR      pointer to a member
 * @param[in]   TYPE     a type name (a struct or union), container of PTR
 * @param[in]   MEMBER   name of the member of TYPE which PTR points to
 * @return      Pointer to the container of PTR.
 */
#if __STDC_VERSION__ >= 201112L
#   define container_of(PTR, TYPE, MEMBER) \
        (_Generic((PTR), \
            const __typeof__ (((TYPE *) 0)->MEMBER) *: \
                ((TYPE *) ((uintptr_t) (PTR) - offsetof(TYPE, MEMBER))), \
            __typeof__ (((TYPE *) 0)->MEMBER) *: \
                ((TYPE *) ((uintptr_t) (PTR) - offsetof(TYPE, MEMBER))) \
        ))
#elif defined __GNUC__
#   define container_of(PTR, TYPE, MEMBER) \
        (__extension__ ({ \
            __extension__ const __typeof__ (((TYPE *) 0)->MEMBER) *__m____ = (PTR); \
            ((TYPE *) ((uintptr_t) __m____ - offsetof(TYPE, MEMBER))); \
        }))
#else
#   define container_of(PTR, TYPE, MEMBER) \
        ((TYPE *) ((char *) (PTR) - offsetof(TYPE, MEMBER)))
#endif

/**
 * @def         index_of(ARRAY, ELEMENT)
 * @brief       Returns the index of a pointer to an array element.

 * @param[in]   ARRAY    an array
 * @param[in]   ELEMENT  pointer to an array element
 * @return      Index of the element in the array
 */
#define index_of(ARRAY, ELEMENT) (((uintptr_t)(ELEMENT) - (uintptr_t)(ARRAY)) / sizeof((ARRAY)[0]))

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
 * @def CONST
 * @brief A function declared as *CONST* is #PURE and also not allowed to
 *        examine global memory. I.e. a *CONST* function cannot even
 *        dereference a pointer parameter.
 */
#ifdef __GNUC__
#define CONST  __attribute__((const))
#else
#define CONST
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
 * @def ARRAY_SIZE(a)
 * @brief       Calculate the number of elements in a static array.
 * @param[in]   a   Array to examine
 * @returns     The number of elements in the array a.
 */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
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
 * @def         RIOT_VERSION_NUM(major, minor, patch, extra)
 * @brief       Generates a 64 bit variable of a release version.
 *              Comparisons to this only apply to released branches
 *
 *              To define @p extra add a file `EXTRAVERSION` to the RIOT root
 *              with the content
 *
 *                  RIOT_EXTRAVERSION = <extra>
 *
 *              with `<extra>` being the number of your local version.
 *              This can be useful if you are maintaining a downstream
 *              release to base further work on.
 *
 * @warning     This is only intended to be used with external boards or
 *              modules.
 *              In-tree code must not make use of this macro.
 *
 * @param[in]   major   Mayor version of the release
 * @param[in]   minor   Minor version of the release
 * @param[in]   patch   Patch level of the release
 * @param[in]   extra   Extra version, user defined
 *
 * @returns     A machine readable version variable
 */
#define RIOT_VERSION_NUM(major, minor, patch, extra)   \
    (((0ULL + major) << 48) + ((0ULL + minor) << 32) + \
     ((0ULL + patch) << 16) + (extra))

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

/* end{code-style-ignore} */

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_DEFINES_H */
/** @} */
