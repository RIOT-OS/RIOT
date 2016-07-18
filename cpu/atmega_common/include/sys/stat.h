/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief   POSIX compatible sys/stat.h definitions
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

/* This header is only used when newlib is not available (e.g. avr) */
#if MODULE_NEWLIB
#include_next <sys/stat.h>
#else
#ifndef SYS_STAT_H_
#define SYS_STAT_H_

#include <time.h> /* for struct timespec */
#include <sys/types.h> /* for fsblkcnt_t, fsfilcnt_t */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief File information
 */
struct stat {
    dev_t st_dev;            /**< Device ID of device containing file. */
    ino_t st_ino;            /**< File serial number. */
    mode_t st_mode;          /**< Mode of file (see below). */
    nlink_t st_nlink;        /**< Number of hard links to the file. */
    uid_t st_uid;            /**< User ID of file. */
    gid_t st_gid;            /**< Group ID of file. */
    dev_t st_rdev;           /**< Device ID (if file is character or block special). */
    /**
     * For regular files, the file size in bytes.
     * For symbolic links, the length in bytes of the pathname contained in the
     * symbolic link.
     * For a shared memory object, the length in bytes.
     * For a typed memory object, the length in bytes.
     * For other file types, the use of this field is unspecified.
     */
    off_t st_size;
    struct timespec st_atim; /**< Last data access timestamp. */
    struct timespec st_mtim; /**< Last data modification timestamp. */
    struct timespec st_ctim; /**< Last file status change timestamp. */
    /**
     * A file system-specific preferred I/O block size for this object. In some
     * file system types, this may vary from file to file.
     */
    blksize_t st_blksize;
    blkcnt_t st_blocks;      /**< Number of blocks allocated for this object. */
};

/* These bitmasks and numbers are the same as in newlib */
#define S_IFMT       0170000 /* type of file */
#define     S_IFDIR  0040000 /* directory */
#define     S_IFCHR  0020000 /* character special */
#define     S_IFBLK  0060000 /* block special */
#define     S_IFREG  0100000 /* regular */
#define     S_IFLNK  0120000 /* symbolic link */
#define     S_IFSOCK 0140000 /* socket */
#define     S_IFIFO  0010000 /* fifo */

/* These numbers are well-known and can be found in the manual page for sys_stat.h */
#define S_IRWXU         0700  /**< Read, write, execute/search by owner.    */
#define S_IRUSR         0400  /**< Read permission, owner.                  */
#define S_IWUSR         0200  /**< Write permission, owner.                 */
#define S_IXUSR         0100  /**< Execute/search permission, owner.        */
#define S_IRWXG          070  /**< Read, write, execute/search by group.    */
#define S_IRGRP          040  /**< Read permission, group.                  */
#define S_IWGRP          020  /**< Write permission, group.                 */
#define S_IXGRP          010  /**< Execute/search permission, group.        */
#define S_IRWXO           07  /**< Read, write, execute/search by others.   */
#define S_IROTH           04  /**< Read permission, others.                 */
#define S_IWOTH           02  /**< Write permission, others.                */
#define S_IXOTH           01  /**< Execute/search permission, others.       */
#define S_ISUID        04000  /**< Set-user-ID on execution.                */
#define S_ISGID        02000  /**< Set-group-ID on execution.               */
#define S_ISVTX        01000  /**< On directories, restricted deletion flag */

/* File type test macros, taken from newlib */
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

/* These function prototypes are required by the standard */
int    chmod(const char *, mode_t);
int    fchmod(int, mode_t);
int    fchmodat(int, const char *, mode_t, int);
int    fstat(int, struct stat *);
int    fstatat(int, const char *restrict, struct stat *restrict, int);
int    futimens(int, const struct timespec [2]);
int    lstat(const char *restrict, struct stat *restrict);
int    mkdir(const char *, mode_t);
int    mkdirat(int, const char *, mode_t);
int    mkfifo(const char *, mode_t);
int    mkfifoat(int, const char *, mode_t);
int    mknod(const char *, mode_t, dev_t);
int    mknodat(int, const char *, mode_t, dev_t);
int    stat(const char *restrict, struct stat *restrict);
mode_t umask(mode_t);
int    utimensat(int, const char *, const struct timespec [2], int);

/* Special tv_nsec values for futimens(2) and utimensat(2). */
#define UTIME_NOW   (-2L)
#define UTIME_OMIT  (-1L)

#ifdef __cplusplus
}
#endif

#endif

#endif

/** @} */
