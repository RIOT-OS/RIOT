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
 * @ingroup    sys_newlib_stubs
 * @{
 *
 * @file
 * @brief       Stubs for read() and write()
 *
 * This modules makes reads and write to any file descriptor fail with EBADF.
 *
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include <reent.h>
#include <errno.h>

/**
 * Stub for read().
 *
 * Always fails with errno = EBADF.
 */
_ssize_t _read_r(struct _reent *r, int fd, void *buffer, size_t count)
{
    (void)r;
    (void)fd;
    (void)buffer;
    (void)count;
    r->_errno = EBADF;
    return -1;
}

/**
 * Stub for write().
 *
 * Always fails with errno = EBADF.
 */
_ssize_t _write_r(struct _reent *r, int fd, const void *data, size_t count)
{
    (void)r;
    (void)fd;
    (void)data;
    (void)count;
    r->_errno = EBADF;
    return -1;
}
