/*
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  posix
 * @{
 */

/**
 * @file    fd.h
 * @brief   Unifies diverse identifiers of RIOT to POSIX like file descriptors.
 *
 * @author  Freie Universität Berlin
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 */
#ifndef FD_H
#define FD_H
#include <stdlib.h>
#include <sys/types.h>
#include "cpu.h"

typedef struct fd_ops {
    /**
     * Read *n* into *buf* from *fd*. Return the
     * number read, -1 for errors or 0 for EOF.
     */
    ssize_t (*read)(int fd, void *buf, size_t n);

    /** Write *n* bytes of *buf* to *fd*. Return the number written, or -1. */
    ssize_t (*write)(int fd, const void *buf, size_t n);

    /** Close the file descriptor *fd*. */
    int (*close)(int fd);
} fd_ops_t;

/**
 * File descriptor table.
 */
typedef struct fd {
    /** the functions of this filedescriptor */
    const fd_ops_t *ops;

    /** the internal filedescriptor */
    int fd;
} fd_t;

/**
 * @brief   Initializes file descriptors
 *
 * @return  maximum number of available file descriptors.
 */
int fd_init(void);

/**
 * @brief Creates a new file descriptor.
 *
 * @param[in] internal_fd RIOT-internal identifier for the new FD.
 * @param[in] internal_ops Function to operate on the new FD.
 *
 * @return 0 on success, -1 otherwise. *errno* is set accordingly.
 */
int fd_new(int internal_fd, const fd_ops_t *internal_ops);

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

#endif /* FD_H */
