/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_stdio_wrapper   stdio wrapper to extend the C libs stdio
 * @ingroup     sys
 *
 * This module a wrapper for the stdio.h header intended to extend the behavior
 * of the standard C libs stdio.h
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

/* External tools such as uhcpd also use `-I $(RIOTBASE)/sys/include but are
 * compiled for the host instead and fail to locate macros/utils.h. So just
 * don't wrap stdio.h when not building a RIOT app. */
#ifdef RIOT_BOARD

#include "macros/utils.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 */
#define printf(...) /* implementation details */
#else
#define _PRINTF_SINGLEARG(x) printf(ASSERT_IS_STR_LITERAL(x))
#define _PRINTF_MULTIARG(x, ...) printf(ASSERT_IS_STR_LITERAL(x), __VA_ARGS__)


#define printf(...) \
        EXPAND(CONCAT(_PRINTF_, SINGLEARG_OR_MULTIARG(__VA_ARGS__))(__VA_ARGS__))
#endif

#ifdef __cplusplus
}
#endif

#endif /* RIOT_BOARD */

#endif /* STDIO_H */
/** @} */
