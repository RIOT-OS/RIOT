/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */
#include <errno.h>

#include "fd.h"
#include "unistd.h"

int close(int fildes)
{
    fd_t *fd_obj = fd_get(fildes);

    if (!fd_obj) {
        errno = EBADF;
        return -1;
    }

    if (fd_obj->close(fd_obj->fd) < 0) {
        errno = EIO;    // EINTR may not occur since RIOT has no signals yet.
        return -1;
    }

    fd_destroy(fd_obj->fd);

    return 0;
}
