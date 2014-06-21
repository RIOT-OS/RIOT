/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     sys
 * @{
 * @file
 * @brief       Implementation for POSIX conformant pipes.
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include <malloc.h>

#include "pipe.h"
#include "fd.h"
#include "irq.h"
#include "unistd.h"

static int pipe_close(int internal_fd_)
{
    pipe_t *rp = (pipe_t *) internal_fd_;
    unsigned old_state = disableIRQ();

    int close_count = ++rp->close_count;
    if (close_count >= 2) {
        pipe_free(rp);
    }

    restoreIRQ(old_state);
    return 0;
}

static const fd_ops_t pipe_ops_in = {
    .read = (ssize_t (*)(int, void *, size_t)) pipe_read,
    .close = (int (*)(int)) pipe_close,
};

static const fd_ops_t pipe_ops_out = {
    .write = (ssize_t (*)(int, const void *, size_t)) pipe_write,
    .close = (int (*)(int)) pipe_close,
};

static const fd_ops_t pipe_ops_inout = {
    .read = (ssize_t (*)(int, void *, size_t)) pipe_read,
    .write = (ssize_t (*)(int, const void *, size_t)) pipe_write,
    .close = (int (*)(int)) pipe_close,
};

int pipe_open(int filedes[2], pipe_t *pipe)
{
    int result = -1;
    if ((filedes[0] = fd_new((int) pipe, &pipe_ops_in)) >= 0) {
        if ((filedes[1] = fd_new((int) pipe, &pipe_ops_out)) >= 0) {
            result = 0;
        }
        else {
            close(filedes[0]);
        }
    }
    pipe->close_count = 0;
    return result;
}

int pipe_open_dual(pipe_t *pipe)
{
    int result = fd_new((int) pipe, &pipe_ops_inout);
    pipe->close_count = 1;
    return result;
}

int pipe(int filedes[2])
{
    int result = -1;

    pipe_t *pipe = pipe_malloc(PIPE_BUF);
    if (pipe) {
        if (pipe_open(filedes, pipe) == 0) {
            result = 0;
        }
        else {
            pipe_free(pipe);
        }
    }

    return result;
}
