/*
 * Copyright (C) 2025 carl-tud
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef UTIL_VALGRIND_H
#define UTIL_VALGRIND_H

#ifdef HAVE_VALGRIND_H
# include <valgrind.h>
#define VALGRIND_DEBUG DEBUG
# elif defined(HAVE_VALGRIND_VALGRIND_H)
# include <valgrind/valgrind.h>
#define VALGRIND_DEBUG DEBUG
#else
# define VALGRIND_STACK_REGISTER(...) (0)
# define VALGRIND_DEBUG(...)
#endif

#ifdef __cplusplus
extern "C" {}
#endif

#endif /* UTIL_VALGRIND_H */
