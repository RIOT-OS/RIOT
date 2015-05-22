/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     posix
 * @{
 * @file
 * @brief       POSIX-like IO
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author      Oliver Hahm <oleg@hobbykeller.org>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef __READ_H
#define __READ_H

#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OPEN 0
#define CLOSE 1
#define READ 2
#define WRITE 3

/**
 * @brief POSIX IO ringbuffer
 */
struct posix_iop_t {
    /** number of bytes */
    int nbytes;
    /** array for the ringbuffer */
    char *buffer;
};

/**
 * @brief Opens a file descriptor - represented by a corresponding thread
 *
 * @param[in] pid   The thread managing the fd to open
 * @param[in] flags Access modes
 *
 * @return 0 on success
 * @return a negative value in error case
 */
int posix_open(int pid, int flags);

/**
 * @brief Closes an open file descriptor
 *
 * @param[in] pid   The opened thread
 *
 * @return 0 on success
 * @return a negative value in error case
 */
int posix_close(int pid);

/**
 * @brief Reads from an open file descriptor
 *
 * @param[in]  pid      The thread managing the open fd
 * @param[out] buffer   Buffer to fill
 * @param[in]  bufsize  Read up to that many bytes into @p buffer
 *
 * @return the number of read bytes
 */
int posix_read(int pid, char *buffer, int bufsize);

/**
 * @brief Writes to an open file descriptor
 *
 * @param[in] pid      The thread managing the open fd
 * @param[in] buffer   Buffer to write
 * @param[in] bufsize  Write that many bytes from @p buffer
 *
 * @return the number of written bytes
 */
int posix_write(int pid, char *buffer, int bufsize);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __READ_H */
