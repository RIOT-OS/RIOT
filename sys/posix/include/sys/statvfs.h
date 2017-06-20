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
/** @todo Remove ifdef __mips__ special case after
 * [#6639](https://github.com/RIOT-OS/RIOT/pull/6639) is merged */
#if MODULE_NEWLIB || defined(__mips__)
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

/**
 * @brief File system information
 */
struct statvfs {
    unsigned long f_bsize;   /**< File system block size. */
    unsigned long f_frsize;  /**< Fundamental file system block size. */
    fsblkcnt_t f_blocks;     /**< Total number of blocks on file system in
                                  units of @c f_frsize. */
    fsblkcnt_t f_bfree;      /**< Total number of free blocks. */
    fsblkcnt_t f_bavail;     /**< Number of free blocks available to
                                  non-privileged process. */
    fsfilcnt_t f_files;      /**< Total number of file serial numbers. */
    fsfilcnt_t f_ffree;      /**< Total number of free file serial numbers. */
    fsfilcnt_t f_favail;     /**< Number of file serial numbers available to
                                  non-privileged process. */

    unsigned long f_fsid;    /**< File system ID. */
    unsigned long f_flag;    /**< Bit mask of f_flag values. */
    unsigned long f_namemax; /**< Maximum filename length. */
};

enum {
    ST_RDONLY = 1,        /* Mount read-only.  */
    ST_NOSUID = 2,        /* Ignore suid and sgid bits.  */
};

#ifdef __cplusplus
}
#endif

#endif /* SYS_STATVFS_H */

#endif /* CPU_NATIVE */

/** @} */
