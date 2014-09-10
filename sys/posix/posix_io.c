/**
 * POSIX implementation of basic IO operations.
 *
 * Copyright (C) 2013, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_posix
 * @{
 * @file    posix_io.c
 * @brief   Implementation of basic POSIX IO functionality.
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include "thread.h"
#include "msg.h"

#include "posix_io.h"


static int _posix_fileop(kernel_pid_t pid, int op, int flags)
{
    blip_t m;
    m.type = op;
    m.content.value = flags;
    blip_send_receive(&m, &m, pid);
    return m.content.value;
}

static int _posix_fileop_data(kernel_pid_t pid, int op, char *buffer, int nbytes)
{
    struct posix_iop_t r;
    r.nbytes = nbytes;
    r.buffer = buffer;

    blip_t m;
    m.type = op;
    m.content.ptr = (char *) &r;

    blip_send_receive(&m, &m, pid);

    return r.nbytes;
}

int posix_open(kernel_pid_t pid, int flags)
{
    return _posix_fileop(pid, OPEN, flags);
}

int posix_close(kernel_pid_t pid)
{
    return _posix_fileop(pid, CLOSE, 0);
}

int posix_read(kernel_pid_t pid, char *buffer, int bufsize)
{
    return _posix_fileop_data(pid, READ, buffer, bufsize);
}

int posix_write(kernel_pid_t pid, char *buffer, int bufsize)
{
    return _posix_fileop_data(pid, WRITE, buffer, bufsize);
}
