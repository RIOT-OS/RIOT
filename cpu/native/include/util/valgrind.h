/*
 * SPDX-FileCopyrightText: 2025 carl-tud
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#ifdef HAVE_VALGRIND_H
#  include <valgrind.h>
#define VALGRIND_DEBUG DEBUG
#  elif defined(HAVE_VALGRIND_VALGRIND_H)
#  include <valgrind/valgrind.h>
#define VALGRIND_DEBUG DEBUG
#else
#  define VALGRIND_STACK_REGISTER(...) (0)
#  define VALGRIND_DEBUG(...)
#endif

#ifdef __cplusplus
extern "C" {}
#endif
