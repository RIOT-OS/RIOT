/*
 * Copyright (C) 2018 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup    sys_newlib_syscalls_riot
 * @{
 *
 * @file
 * @brief       Simple read and write to serial
 *
 * This modules makes every file descriptor refer to the default serial port.
 * It is a fallback for when VFS is not used.
 *
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include <reent.h>
#include <errno.h>

#include "stdio_base.h"

/**
 * Fallback read function
 *
 * All input is read from stdio_uart regardless of fd number. The function will
 * block until a byte is actually read.
 *
 * Note: the read function does not buffer - data will be lost if the function is not
 * called fast enough.
 */
_ssize_t _read_r(struct _reent *r, int fd, void *buffer, size_t count)
{
    (void)r;
    (void)fd;
    return stdio_read(buffer, count);
}

/**
 * Fallback write function
 *
 * All output is directed to stdio_uart, independent of the given file descriptor.
 * The write call will further block until the byte is actually written to the UART.
 */
_ssize_t _write_r(struct _reent *r, int fd, const void *data, size_t count)
{
    (void) r;
    (void) fd;
    return stdio_write(data, count);
}
