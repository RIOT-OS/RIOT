/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup posix_select   POSIX select
 * @ingroup  posix
 * @brief   Select implementation for RIOT
 * @see     [The Open Group Base Specification Issue 7]
 *          (https://pubs.opengroup.org/onlinepubs/9699919799.2018edition/)
 * @todo    Omitted from original specification for now:
 *          - Inclusion of `<signal.h>`; no POSIX signal handling implemented
 *            in RIOT yet
 *          - `pselect()` as it uses `sigset_t` from `<signal.h>`
 *          - handling of the `writefds` and `errorfds` parameters of `select()`
 * @todo    Currently, only [sockets](@ref posix_sockets) are supported
 * @{
 *
 * @file
 * @brief   Select types
 * @see     [The Open Group Base Specification Issue 7, 2018 edition,
 *          <sys/select.h>](https://pubs.opengroup.org/onlinepubs/9699919799.2018edition/basedefs/sys_select.h)
 */


#ifdef CPU_NATIVE
/* On native, system headers may depend on system's <sys/select.h>. Hence,
 * include the real sys/select.h here. */
__extension__
#include_next <sys/select.h>
#endif

#include <string.h>
/* prevent cyclic dependency with newlib/picolibc's `sys/types.h` */
#if (defined(MODULE_NEWLIB) || defined(MODULE_PICOLIBC)) && !defined(CPU_ESP8266)
#include <sys/_timeval.h>
#else
#include <sys/time.h>
#endif

#include "bitfield.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   @ref core_thread_flags for POSIX select
 */
#define POSIX_SELECT_THREAD_FLAG    (1U << 3)

#ifndef CPU_NATIVE

/**
 * @addtogroup  config_posix
 * @{
 */
/**
 * @brief   Maximum number of file descriptors in an `fd_set` structure.
 *
 * @note    Should have at least the same value as VFS_MAX_OPEN_FILES.
 *
 * Config-exposed version
 */
#ifndef CONFIG_POSIX_FD_SET_SIZE
#define CONFIG_POSIX_FD_SET_SIZE    (16)
#endif
/** @} */

/* ESP's newlib has this already defined in `sys/types.h` */
#if !defined(CPU_ESP8266)
/**
 * @brief   Maximum number of file descriptors in an `fd_set` structure.
 *
 * POSIX-compliant version.
 */
#define FD_SETSIZE                  (CONFIG_POSIX_FD_SET_SIZE)

/**
 * @brief   The `fd_set` structure
 */
typedef struct {
    BITFIELD(fds, FD_SETSIZE);  /**< Bit-field to represent the set of file
                                 *   descriptors */
} fd_set;

/**
 * @brief   Removes a file descriptor from an `fd_set` if it is a member
 *
 * @param[in] fd        A file descriptor
 * @param[in] fdsetp    An `fd_set`
 */
static inline void FD_CLR(int fd, fd_set *fdsetp)
{
    bf_unset(fdsetp->fds, fd);
}

/**
 * @brief   Checks if a file descriptor is a member of an `fd_set`
 *
 * @param[in] fd        A file descriptor
 * @param[in] fdsetp    An `fd_set`
 *
 * @return  non-zero value, if @p fd is a member of @p fdsetp
 * @return  0, if @p fd is not a member of @p fdsetp
 */
static inline int FD_ISSET(int fd, fd_set *fdsetp)
{
    return (int)bf_isset(fdsetp->fds, fd);
}

/**
 * @brief   Adds a file descriptor from an `fd_set` if it is not already a
 *          member
 *
 * @param[in] fd        A file descriptor
 * @param[in] fdsetp    An `fd_set`
 */
static inline void FD_SET(int fd, fd_set *fdsetp)
{
    bf_set(fdsetp->fds, fd);
}

/**
 * @brief   Initializes the descriptor set as an empty set
 *
 * @param[in] fdsetp    An `fd_set`
 */
static inline void FD_ZERO(fd_set *fdsetp)
{
    memset(fdsetp->fds, 0, sizeof(fdsetp->fds));
}
#endif /* !defined(CPU_ESP32) && !defined(CPU_ESP8266) */

/**
 * @brief   Examines the given file descriptor sets if they are ready for their
 *          respective operation.
 *
 * @param[in] nfds          The range of descriptors tested. The first @p nfds
 *                          descriptors shall be checked in each set; that is,
 *                          the descriptors from zero through @p nfds - 1 in the
 *                          descriptor sets shall be examined.
 * @param[in,out] readfds   The set of file descriptors to be checked for being
 *                          ready to read. Indicates on output which file
 *                          descriptors are ready to read. May be NULL to check
 *                          no file descriptors.
 * @param[in,out] writefds  The set of file descriptors to be checked for being
 *                          ready to write. Indicates on output which file
 *                          descriptors are ready to write. May be NULL to check
 *                          no file descriptors.
 *                          **As only sockets are supported for now, these will
 *                          be ignored**
 * @param[in,out] errorfds  The set of file descriptors to be checked for being
 *                          error conditions pending. Indicates on output which
 *                          file descriptors have error conditions pending. May
 *                          be NULL to check no file descriptors.
 *                          **As only sockets are supported for now, these will
 *                          be ignored**
 * @param[in] timeout       Timeout for select to block until one or more of the
 *                          checked file descriptors is ready. Set timeout
 *                          to all-zero to return immediately without blocking.
 *                          May be NULL to block indefinitely.
 *
 * @return  number of members added to the file descriptor sets on success.
 * @return  -1 on error, `errno` is set to indicate the error.
 */
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
           struct timeval *timeout);

#endif /* CPU_NATIVE */

#ifdef __cplusplus
}
#endif

/** @} */
