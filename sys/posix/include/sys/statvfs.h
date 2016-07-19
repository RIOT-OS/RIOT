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

#ifndef SYS_STATVFS_H_
#define SYS_STATVFS_H_

#include <sys/types.h> /* for fsblkcnt_t, fsfilcnt_t */

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

#endif

/** @} */
