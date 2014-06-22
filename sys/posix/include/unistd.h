/*
 * Copyright (C) 2013 Freie Universität Berlin
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
 * @file    unistd.h
 * @brief   standard symbolic constants and types
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/unistd.h.html">
 *              The Open Group Base Specifications Issue 7, <unistd.h>
 *          </a>
 *
 * @author  Freie Universität Berlin
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */
#ifndef _UNISTD_H
#define _UNISTD_H

#include <stdint.h>
#include <sys/types.h>

#include "timex.h"
#include "vtimer.h"

#define STDIN_FILENO    0   ///< stdin file descriptor
#define STDOUT_FILENO   1   ///< stdout file descriptor
#define STDERR_FILENO   2   ///< stderr file descriptor

/**
 * @brief        Read from a file descriptor.
 * @details      shall attempt to read *nbyte* bytes from the file associated
 *               with the open file descriptor, *fildes*, into the buffer pointed
 *               to by *buf*. The behavior of multiple concurrent reads on the
 *               same pipe, FIFO, or terminal device is unspecified.
 * @see          <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/read.html">
 *                   The Open Group Base Specifications Issue 7, read
 *               </a>
 * @param        fildes   The file descriptor to read from.
 * @param[out]   buf      Buffer to read into.
 * @param        n        Maximum number of bytes to read.
 * @return       Upon successful completion, a non-negative integer indicating the
 *               number of bytes actually read.
 *               Otherwise, the function shall return -1 and set errno to indicate the error.
 */
ssize_t read(int fildes, void *buf, size_t n);

/**
 * @brief        Write to a file descriptor.
 * @details      shall attempt to write *nbyte* bytes from the buffer pointed to by
 *               *buf* to the file associated with the open file descriptor, *fildes*.
 * @see          <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/write.html">
 *                   The Open Group Base Specifications Issue 7, write
 *               </a>
 * @param        fildes   The file descriptor to read from.
 * @param[int]   buf      Buffer to read from.
 * @param        n        Maximum number of bytes to write.
 * @return       Upon successful completion, a non-negative integer indicating the
 *               number of bytes actually written.
 *               Otherwise, the function shall return -1 and set errno to indicate the error.
 */
ssize_t write(int fildes, const void *buf, size_t n);

/**
 * @brief   Close a file descriptor.
 * @details shall deallocate the file descriptor indicated by *fildes*. To
 *          deallocate means to make the file descriptor available for return
 *          by subsequent calls to open() or other functions that allocate file
 *          descriptors. All outstanding record locks owned by the process on
 *          the file associated with the file descriptor shall be removed (that
 *          is, unlocked).
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/close.html">
 *          The Open Group Base Specification Issue 7, close
 *      </a>
 *
 * @param[in]   fildes  The file descriptor to the file which is to close.
 * @return  Upon successful completion, 0 shall be returned; otherwise, -1
 *          shall be returned and errno set to indicate the error.
 */
int close(int fildes);

#ifndef __USECONDS_T_TYPE
typedef unsigned long __USECONDS_T_TYPE;
#endif
typedef __USECONDS_T_TYPE __useconds_t;
typedef __useconds_t useconds_t;

/**
 * @brief the caller will sleep for given amount of micro seconds
 * @details The usleep() function will cause the calling thread to be
 *          suspended from execution until either the number of real-time microseconds
 *          specified by the argument useconds has elapsed or a signal is delivered to
 *          the calling thread and its action is to invoke a signal-catching function
 *          or to terminate the process. The suspension time may be longer than
 *          requested due to the scheduling of other activity by the system.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/7908799/xsh/usleep.html">
 *          The Open Group Base Specification Issue 2, usleep
 *      </a>
 *
 * @param useconds   time to sleep in micro seconds
 * @return           0 on success
 */
int usleep(useconds_t useconds);

/**
 * @brief the caller will sleep for given amount of seconds
 * @details The sleep() function shall cause the calling thread to be suspended
 *          from execution until either the number of realtime seconds
 *          specified by the argument seconds has elapsed or a signal is
 *          delivered to the calling thread and its action is to invoke a
 *          signal-catching function or to terminate the process. The
 *          suspension time may be longer than requested due to the scheduling
 *          of other activity by the system.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/009695399/functions/sleep.html">
 *          The Open Group Base Specification Issue 6, sleep
 *      </a>
 *
 * @param seconds   time to sleep in seconds
 * @return          0 on success
 */
unsigned int sleep(unsigned int seconds);

/**
 *
 */
int pipe(int filedes[2]);

#endif /* _UNISTD_H */

/**
 * @}
 */
