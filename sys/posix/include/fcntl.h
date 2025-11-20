/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @file
 * @ingroup posix
 * @brief   POSIX compatible fcntl.h definitions
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @see http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/fcntl.h.html
 * @{
 */

#ifndef DOXYGEN
#if defined(CPU_NATIVE) || MODULE_NEWLIB || MODULE_PICOLIBC
/* If building on native or newlib we need to use the system header instead */
#pragma GCC system_header
/* without the GCC pragma above #include_next will trigger a pedantic error */
#include_next <fcntl.h>
#else

#include <sys/types.h> /* for mode_t, off_t */

#ifdef __cplusplus
extern "C" {
#endif

/* Constants used for the flags in fcntl and open */
/* The names of the constants are defined by POSIX but most of these numbers
 * were chosen somewhat arbitrarily */
#define O_ACCMODE   (O_RDONLY|O_WRONLY|O_RDWR)
#define O_RDONLY    0       /* Read only */
#define O_WRONLY    1       /* Write only */
#define O_RDWR      2       /* Read+write */
#define O_APPEND    0x0008  /* Set append mode */
#define O_CREAT     0x0010  /* Create file if it does not exist */
#define O_TRUNC     0x0020  /* Truncate flag */
#define O_EXCL      0x0040  /* Exclusive use flag */

#define F_DUPFD     0       /* Duplicate file descriptor */
#define F_GETFD     1       /* Get file descriptor flags */
#define F_SETFD     2       /* Set file descriptor flags */
#define F_GETFL     3       /* Get file status flags */
#define F_SETFL     4       /* Set file status flags */
#define F_GETOWN    5       /* Get owner */
#define F_SETOWN    6       /* Set owner */
#define F_GETLK     7       /* Get record-locking information */
#define F_SETLK     8       /* Set or Clear a record-lock (Non-Blocking) */
#define F_SETLKW    9       /* Set or Clear a record-lock (Blocking) */

#define FD_CLOEXEC  1       /* Close the file descriptor upon execution of an exec family function */

int  creat(const char *, mode_t);
int  fcntl(int, int, ...);
int  open(const char *, int, ...);
int  openat(int, const char *, int, ...);
int  posix_fadvise(int, off_t, off_t, int);
int  posix_fallocate(int, off_t, off_t);

#ifdef __cplusplus
}
#endif

#endif /* CPU_NATIVE */

#endif /* DOXYGEN */

/** @} */
