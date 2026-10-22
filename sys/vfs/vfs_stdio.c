/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       STDIO binding for the VFS module
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "assert.h"
#include "stdio_base.h"
#include "vfs.h"

static ssize_t _stdio_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    int fd = filp->private_data.value;
    if (fd != STDIN_FILENO) {
        return -EBADF;
    }
    return stdio_read(dest, nbytes);
}

static ssize_t _stdio_write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    int fd = filp->private_data.value;
    if (fd == STDIN_FILENO) {
        return -EBADF;
    }
    return stdio_write(src, nbytes);
}

/**
 * @brief   VFS file operation table for stdin/stdout/stderr
 */
static vfs_file_ops_t _stdio_ops = {
    .read = _stdio_read,
    .write = _stdio_write,
};

void vfs_bind_stdio(void)
{
    int fd;
    fd = vfs_bind(STDIN_FILENO, O_RDONLY, &_stdio_ops, (void *)STDIN_FILENO);
    /* Is there a better way to handle errors on init? */
    assert(fd >= 0);
    fd = vfs_bind(STDOUT_FILENO, O_WRONLY, &_stdio_ops, (void *)STDOUT_FILENO);
    assert(fd >= 0);
    fd = vfs_bind(STDERR_FILENO, O_WRONLY, &_stdio_ops, (void *)STDERR_FILENO);
    assert(fd >= 0);
    /* we are not interested in the return value in case assert is not
     * compiled in */
    (void)fd;
}
