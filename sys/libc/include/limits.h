/*
 * SPDX-FileCopyrightText: 2026 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  posix
 * @{
 */

/**
 * @file
 * @brief   Wrapper around `limits.h` filling gaps
 *
 * @author  Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#include_next <limits.h> /* IWYU pragma: export */
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif
#ifdef DOXYGEN
/**
 * @brief   Highest numeric value an `ssize_t` can hold
 *
 * @note    A POSIX compliant libc must provide this macro in `<limits.h>`. RIOT
 *          ensures an `#include <limits.h>` does provide it, even if the used
 *          libc is not.
 */
#  define SSIZE_MAX implementation_defined
#elif !defined(SSIZE_MAX)
#  ifndef SIZE_MAX
#    error "Neither SSIZE_MAX NOR SIZE_MAX defined"
#  endif
#  if SIZE_MAX == UINT64_MAX
#    define SSIZE_MAX INT64_MAX
#  elif SIZE_MAX == UINT32_MAX
#    define SSIZE_MAX INT32_MAX
#  elif SIZE_MAX == UINT16_MAX
#    define SSIZE_MAX INT16_MAX
#  else
#    error "Bogus SIZE_MAX value?"
#  endif
#endif

#ifdef __cplusplus
}
#endif
/** @} */
