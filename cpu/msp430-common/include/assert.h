/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     msp430
 * @{
 *
 * @file
 * @brief       assert.h for msp430
 * @see http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/assert.h.html
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef ASSERT_H_
#define ASSERT_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def ASSERT_FUNC
 *
 * @brief   Contains the function name if given compiler supports it.
 *          Otherwise it is an empty string.
 */
# if defined(__cplusplus) && defined(__GNUC__)
#  define ASSERT_FUNC __PRETTY_FUNCTION__
# elif __STDC_VERSION__ >= 199901L
#  define ASSERT_FUNC __func__
# elif __GNUC__ >= 2
#  define ASSERT_FUNC __FUNCTION__
# else
#  define ASSERT_FUNC ""
# endif

/**
 * @def assert
 * @see http://pubs.opengroup.org/onlinepubs/9699919799/functions/assert.html
 */
#ifndef NDEBUG
# define assert(assertion) \
    if (!(assertion)) { \
        printf("%s:%d: %s: Assertion '%s' failed", __FILE__, __LINE__, \
               ASSERT_FUNC, #assertion); \
        abort(); \
    }
#else
# define assert(assertion) ((void) 0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* ASSERT_H_ */
/**
 * @}
 */
