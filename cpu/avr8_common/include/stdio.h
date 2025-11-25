/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_avr8_common_stdio_wrapper   stdio wrapper for AVR8
 * @ingroup     cpu_avr8_common
 *
 * This module a wrapper for the stdio.h header intended to make use of
 * flash_utils.h in printf() automatically
 *
 * @{
 *
 * @file
 * @brief       stdio wrapper to extend the C libs stdio
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef STDIO_H
#define STDIO_H
#include_next "stdio.h"

/* C++ does not support __flash. Hence, only wrap printf() and friends for
 * C and let C++ use them unmodified. */
#ifdef __cplusplus
extern "C" {
}
#else

#include "flash_utils.h"

#ifdef DOXYGEN
/**
 * @brief   A wrapper for the `printf()` function that passes arguments through
 *          unmodified, but fails to compile if the first argument is not a
 *          string literal.
 *
 * See e.g. `man 3 printf` or https://linux.die.net/man/3/printf for
 * documentation the printf function. This applies fully here, as it passes
 * through the arguments unmodified.
 *
 * The motivation for enforcing the first argument to be a string literal is
 * three-fold:
 *
 * 1. It prevents security issues due format strings controlled by adversaries.
 * 2. It makes sure that modern C compilers that do understand format
 *    specifiers have knowledge of the format string and can verify that the
 *    other arguments match what is given via format string specifiers
 * 3. It allows to force the format string to flash even for Harvard
 *    architectures transparently
 *
 * Similar wrappers are also in place for `vprintf()`, `fprintf()`,
 * `vfprintf()`, `snprintf()`, `vsnprintf()`.
 */
#define printf(...)     /* implementation details */
#else
/* this helper function-like macro takes at least 65 arguments and will
 * "return" the 65 argument unmodified. It is not useful by itself, but
 * needed to implement _SINGLEARG_OR_MULTIARG(). */
#define _TAKE_65TH_TOKEN( _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
                          _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                          _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                          _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
                          _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
                          _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
                          _61, _62, _63, _64, N, ...) N

#define _EXPAND_HELPER(x) x
/* this function-like macro expands its argument */
#define _EXPAND(x) _EXPAND_HELPER(x)

/* This function-like macro will expand to `SINGLEARG` if called with one
 * argument and `MULTIARG` if called with more than one.
 *
 * (Implementation detail: It will not work with more than 64 arguments. But
 * 64 arguments to one printf call ought to be enough for everyone...)
 */
#define _SINGLEARG_OR_MULTIARG(...) \
    _TAKE_65TH_TOKEN(__VA_ARGS__, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, MULTIARG, \
            MULTIARG, MULTIARG, MULTIARG, SINGLEARG)
#define _CONCAT_HELPER(a, b) a ## b
#define _CONCAT(a, b) _CONCAT_HELPER(a, b)

/* Implementation for `printf(fmt)` */
#define _PRINTF_SINGLEARG(x) \
        flash_printf(TO_FLASH(x))
/* Implementation for `printf(fmt, ...)` */
#define _PRINTF_MULTIARG(x, ...) \
        flash_printf(TO_FLASH(x), __VA_ARGS__)
/* Dispatch to _PRINTF_SINGLEARG() and _PRINTF_MULTIARG() depending on the
 * number of arguments. Special handling for `printf(fmt)` compared to
 * `printf(fmt, ...)` is needed because the `__VA_ARGS__` part must contain
 * at least one argument, which in case of `printf(fmt)` is not the case. */
#define printf(...) \
        _EXPAND(_CONCAT(_PRINTF_, _SINGLEARG_OR_MULTIARG(__VA_ARGS__))(__VA_ARGS__))

/* And now all other printf variants. For the v*printf() versions we do not
 * need to differentiate, because they have always the same number of arguments
 * (with the last being va_list). For the other printf variants, we again need
 * to dispatch to a _SINGLEARG and a _MULTIARG version. */
#define vprintf(fmt, args) flash_vprintf(TO_FLASH(fmt), args)

#define _FPRINTF_SINGLEARG(stream, x) \
        flash_fprintf(stream, TO_FLASH(x))
#define _FPRINTF_MULTIARG(stream, x, ...) \
        flash_fprintf(stream, TO_FLASH(x), __VA_ARGS__)
#define fprintf(stream, ...) \
        _EXPAND(_CONCAT(_FPRINTF_, _SINGLEARG_OR_MULTIARG(__VA_ARGS__))(stream, __VA_ARGS__))

#define vfprintf(stream, fmt, args) flash_vfprintf(stream, TO_FLASH(fmt), args)

#define _SNPRINTF_SINGLEARG(buf, buf_len, fmt) \
        flash_snprintf(buf, buf_len, TO_FLASH(fmt))
#define _SNPRINTF_MULTIARG(buf, buf_len, fmt, ...) \
        flash_snprintf(buf, buf_len, TO_FLASH(fmt), __VA_ARGS__)
#define snprintf(buf, buf_len, ...) \
        _EXPAND(_CONCAT(_SNPRINTF_, _SINGLEARG_OR_MULTIARG(__VA_ARGS__))(buf, buf_len, __VA_ARGS__))

#define vsnprintf(buf, buf_len, fmt, args) flash_vsnprintf(buf, buf_len, TO_FLASH(fmt), args)
#endif

#endif

#endif /* STDIO_H */
/** @} */
