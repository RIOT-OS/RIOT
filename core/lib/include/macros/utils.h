/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_macros
 * @{
 *
 * @file
 * @brief       Various helper macros
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef MACROS_UTILS_H
#define MACROS_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN
/**
 * @brief   Expands the arguments and concatenates them afterwards
 *
 * @details This is currently only implemented for up to 8 arguments.
 *          Implementations for more arguments can be extended as the need
 *          arises.
 */
#define CONCAT(...)                 /* implementation details */

/**
 * @brief   Ensure that the preprocessor expands the token given as argument
 *
 * This is useful to e.g. when "calling" function-like macros as argument to
 * a function-like macro
 */
#define EXPAND(x)                   /* implementation details */

/**
 * @brief   Takes a number of tokens as argument and is replaced by
 *          the decimal number of arguments
 *
 * @warning This only works for up to 64 tokens as argument
 */
#define COUNT_ARGS(...) /* implementation details */

#else
#define EXPAND(x) x

#define _TAKE_65TH_TOKEN( _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
                          _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                          _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                          _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
                          _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
                          _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
                          _61, _62, _63, _64, N, ...) N

#define COUNT_ARGS(...) \
        _TAKE_65TH_TOKEN(__VA_ARGS__,        64, 63, 62, 61, 60, \
                         59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
                         49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
                         39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
                         29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
                         19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
                          9,  8,  7,  6,  5,  4,  3,  2,  1)

#define _CONCAT2_(a, b)         a ## b
#define _CONCAT2(a, b)          _CONCAT2_(a, b)

#define _OTHER_CONCAT2_(a, b)   a ## b
#define _OTHER_CONCAT2(a, b)    _OTHER_CONCAT2_(a, b)

#define _CONCAT3_(a, b, c)       a ## b ## c
#define _CONCAT3(a, b, c)       _CONCAT3_(a, b, c)

#define _CONCAT4_(a, b, c, d)   a ## b ## c ## d
#define _CONCAT4(a, b, c, d)    _CONCAT4_(a, b, c, d)

#define _CONCAT5_(a, b, c, d, e) \
        a ## b ## c ## d ## e
#define _CONCAT5(a, b, c, d, e) \
        _CONCAT5_(a, b, c, d, e)

#define _CONCAT6_(a, b, c, d, e, f) \
        a ## b ## c ## d ## e ## f
#define _CONCAT6(a, b, c, d, e, f) \
        _CONCAT6_(a, b, c, d, e, f)

#define _CONCAT7_(a, b, c, d, e, f, g) \
        a ## b ## c ## d ## e ## f ## g
#define _CONCAT7(a, b, c, d, e, f, g) \
        _CONCAT7_(a, b, c, d, e, f, g)

#define _CONCAT8_(a, b, c, d, e, f, g, h) \
        a ## b ## c ## d ## e ## f ## g ## h
#define _CONCAT8(a, b, c, d, e, f, g, h)    \
        _CONCAT8_(a, b, c, d, e, f, g, h)

/*
 * This works as follows:
 * CONCAT(foo, bar)
 * --> EXPAND(_OTHER_CONCAT2(_CONCAT, COUNT_ARGS(foo, bar))(foo, bar))
 * --> EXPAND(_OTHER_CONCAT2(_CONCAT, 2)(foo, bar))
 * --> EXPAND(_CONCAT2(foo, bar))
 * --> foobar
 *
 * It basically dispatches to a call of CONCAT<NUM>(__VA_ARGS__) where <NUM>
 * is replaced with the number of arguments. We need to use _OTHER_CONCAT2
 * instead of _CONCAT2 for the concatenation of _CONCAT and the number of
 * args, as otherwise for 2 arguments _CONCAT2 would need to be expended twice,
 * but the preprocessor will only expand the first instance of _CONCAT2
 */
#define CONCAT(...) \
        EXPAND(_OTHER_CONCAT2(_CONCAT, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__))

#endif

/* For compatibility with vendor headers, only provide MAX() and MIN() if not
 * provided. (The alternative approach of using #undef has the downside that
 * vendor header files may provide a smarter version of MAX() / MIN() that does
 * not evaluate the argument twice and rely on this).
 */
#ifndef MAX
/**
 * @brief   Get the maximum of the two parameters
 *
 * @note    This is the trivial implementation that does evaluate the arguments
 *          more than once
 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
/**
 * @brief   Get the minimum of the two parameters
 *
 * @note    This is the trivial implementation that does evaluate the arguments
 *          more than once
 */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifdef __cplusplus
}
#endif

#endif /* MACROS_UTILS_H */
/** @} */
