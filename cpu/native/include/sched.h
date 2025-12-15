/*
 * SPDX-FileCopyrightText: 2025 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Inject some ugly workarounds needed for native on sched.h - the issue here
 * is that RIOT's sched.h shadows POSIX's sched.h - which on glibc contains
 * a type definition of cpu_set_t needed in a few other glibc headers. Since
 * (portable) RIOT apps never will actually call any functions provided only
 * on native, we don't really need to provide a meaningful definition here. */
#if __GLIBC__
typedef void cpu_set_t;
#endif

#ifdef __cplusplus
}
#endif

#include_next "sched.h"
