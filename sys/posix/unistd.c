/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief   Providing implementation for close for fds defined in fd.h.
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 */
#include <errno.h>
#include <unistd.h>

#include "fd.h"

int close(int fildes)
{
    fd_t *fd_obj = fd_get(fildes);

    if (!fd_obj) {
        errno = EBADF;
        return -1;
    }

    if (fd_obj->close(fd_obj->internal_fd) < 0) {
        errno = EIO;    // EINTR may not occur since RIOT has no signals yet.
        return -1;
    }

    fd_destroy(fd_obj->internal_fd);

    return 0;
}

/**
 * @}
 */
