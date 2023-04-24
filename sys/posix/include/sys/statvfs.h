/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief   POSIX compatible sys/statvfs.h definitions
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @{
 */

/* If building on native we need to use the system libraries instead */
#ifdef CPU_NATIVE
#pragma GCC system_header
/* without the GCC pragma above #include_next will trigger a pedantic error */
#include_next <sys/statvfs.h>
#else
#ifndef SYS_STATVFS_H
#define SYS_STATVFS_H

#include <sys/types.h> /* for fsblkcnt_t, fsfilcnt_t */
#include "vfs.h" /* for struct statvfs */
#if MODULE_NEWLIB
/* newlib support for fsblkcnt_t was only recently added to the newlib git
 * repository, commit f3e587d30a9f65d0c6551ad14095300f6e81672e, 15 apr 2016.
 * Will be included in release 2.5.0, around new year 2016/2017.
 * We provide the below workaround if the used tool chain is too old. */
#ifndef _FSBLKCNT_T_DECLARED		/* for statvfs() */
#include <stdint.h>
/* Default to 32 bit file sizes on newlib platforms */
typedef uint32_t fsblkcnt_t;
typedef uint32_t fsfilcnt_t;
#define _FSBLKCNT_T_DECLARED
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYS_STATVFS_H */

#endif /* CPU_NATIVE */

/** @} */
