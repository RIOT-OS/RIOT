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

/* We cannot use e.g. `sizeof(ssize_t)` in preprocessor macros, but only
 * rely on other macros. We assume instead `sizeof(ssize_t) == sizeof(size_t)`
 */
#ifndef SSIZE_MAX
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
