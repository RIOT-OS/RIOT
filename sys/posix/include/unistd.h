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
 */
#ifndef _UNISTD_H
#define _UNISTD_H

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

/**
 * @}
 */
#endif /* _UNISTD_H */
