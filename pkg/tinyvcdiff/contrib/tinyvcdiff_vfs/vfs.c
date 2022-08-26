/*
 * Copyright (C) 2022 Juergen Fitschen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "vcdiff_vfs.h"
#include <unistd.h>
#include "errno.h"

static int _read (void *dev, uint8_t *dest, size_t offset, size_t len)
{
    int rc;
    vcdiff_vfs_t *vfs = dev;

    /* seek to the given offset */
    rc = vfs_lseek(vfs->fd, offset, SEEK_SET);
    if (rc < 0) {
        return rc;
    } else if ((size_t) rc != offset) {
        return -EFAULT;
    }

    /* read from file into the buffer */
    rc = vfs_read(vfs->fd, dest, len);
    if (rc < 0) {
        return rc;
    } else if ((size_t) rc != len) {
        return -EFAULT;
    }

    return 0;
}

static int _write (void *dev, uint8_t *src, size_t offset, size_t len)
{
    int rc;
    vcdiff_vfs_t *vfs = dev;

    /* make sure to stay in the set file size bounds */
    if (offset + len > vfs->max_size) {
        return -EFBIG;
    }

    /* seek to the given offset */
    rc = vfs_lseek(vfs->fd, offset, SEEK_SET);
    if (rc < 0) {
        return rc;
    } else if ((size_t) rc != offset) {
        return -EFAULT;
    }

    /* write all bytes to the file */
    rc = vfs_write(vfs->fd, src, len);
    if (rc < 0) {
        return rc;
    } else if ((size_t) rc != len) {
        return -EFAULT;
    }

    return 0;
}

const vcdiff_driver_t vcdiff_vfs_driver = {
    .read = _read,
    .write = _write
};
