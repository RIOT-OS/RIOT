/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  posix
 * @{
 */

/**
 * @file    fd.h
 * @brief   Unifies diverse identifiers of RIOT to POSIX like file descriptors.
 *
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 */
#ifndef FD_H
#define FD_H
#include <stdlib.h>
#include <sys/types.h>
#include "kernel_types.h"
#include "cpu.h"

/**
 * File descriptor table.
 */
typedef struct {
    /** private status of this fd_t */
    int __active;

    /** the internal filedescriptor */
    kernel_pid_t fd;

    /**
     * Read *n* into *buf* from *fd*.  Return the
     * number read, -1 for errors or 0 for EOF.
     */
    ssize_t (*read)(int fd, void *buf, size_t n);

    /** Write *n* bytes of *buf* to *fd*.  Return the number written, or -1. */
    ssize_t (*write)(int fd, const void *buf, size_t n);

    /** Close the file descriptor *fd*. */
    int (*close)(kernel_pid_t fd);
} fd_t;

/**
 * @brief   Initializes file descriptors
 *
 * @return  maximum number of available file descriptors.
 */
int fd_init(void);

/**
 * @brief   Creates a new file descriptor.
 *
 * @param[in] internal_fd       RIOT-internal identifier for the new FD.
 * @param[in] internal_read     Function to read from new FD.
 * @param[in] internal_write    Function to write into new FD.
 * @param[in] internal_close    Function to close new FD.
 *
 * @return  0 on success, -1 otherwise. *errno* is set accordingly.
 */
int fd_new(int internal_fd, ssize_t (*internal_read)(int, void *, size_t),
           ssize_t (*internal_write)(int, const void *, size_t),
           int (*internal_close)(kernel_pid_t));

/**
 * @brief   Gets the file descriptor table entry associated with file
 *          descriptor *fd*.
 *
 * @param[in] fd    A POSIX-like file descriptor.
 *
 * @return  The file descriptor table entry associated with file descriptor
            *fd* or NULL if there is non.
 */
fd_t *fd_get(int fd);

/**
 * @brief   Removes file descriptor table entry associated with *fd* from table.
 *
 * @param[in] fd    A POSIX-like file descriptor.
 */
void fd_destroy(int fd);

#endif /* FD_H */
