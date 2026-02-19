/*
 * SPDX-FileCopyrightText: 2025 carl-tud
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#ifdef HAVE_VALGRIND
/* __has_include() will only be reached on native and only when valgrind is
 * enabled, so we do not limit compatibility with embedded toolchains here */
#  if __has_include(<valgrind/valgrind.h>)
#    include <valgrind/valgrind.h>
#  else
#    include <valgrind.h>
#  endif
#  include <valgrind.h>
#  define VALGRIND_DEBUG DEBUG
#else
#  define VALGRIND_STACK_REGISTER(...) (0)
#  define VALGRIND_DEBUG(...)
#endif

#ifdef __cplusplus
extern "C" {}
#endif
