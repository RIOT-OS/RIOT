/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#if MODULE_VFS

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "nvram.h"
#include "vfs.h"

/**
 * @ingroup     drivers_nvram
 * @{
 *
 * @file
 *
 * @brief       NVRAM generic VFS operations
 *
 * This allows the nvram driver to register as a node on DevFS
 *
 * See boards/mulle or tests/unittests/tests-devfs for examples on how to use.
 *
 * Tested with nvram_spi on Mulle
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

static int nvram_vfs_fstat(vfs_file_t *filp, struct stat *buf);
static off_t nvram_vfs_lseek(vfs_file_t *filp, off_t off, int whence);
static ssize_t nvram_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes);
static ssize_t nvram_vfs_write(vfs_file_t *filp, const void *src, size_t nbytes);

const vfs_file_ops_t nvram_vfs_ops = {
    .fstat = nvram_vfs_fstat,
    .lseek = nvram_vfs_lseek,
    .read  = nvram_vfs_read,
    .write = nvram_vfs_write,
};

static int nvram_vfs_fstat(vfs_file_t *filp, struct stat *buf)
{
    if (buf == NULL) {
        return -EFAULT;
    }
    nvram_t *dev = filp->private_data.ptr;
    if (dev == NULL) {
        return -EFAULT;
    }
    buf->st_nlink = 1;
    buf->st_size = dev->size;
    return 0;
}

static off_t nvram_vfs_lseek(vfs_file_t *filp, off_t off, int whence)
{
    nvram_t *dev = filp->private_data.ptr;
    if (dev == NULL) {
        return -EFAULT;
    }
    switch (whence) {
        case SEEK_SET:
            break;
        case SEEK_CUR:
            off += filp->pos;
            break;
        case SEEK_END:
            off += dev->size;
            break;
        default:
            return -EINVAL;
    }
    if (off < 0) {
        /* the resulting file offset would be negative */
        return -EINVAL;
    }
    /* POSIX allows seeking past the end of the file */
    filp->pos = off;
    return off;
}

static ssize_t nvram_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    nvram_t *dev = filp->private_data.ptr;
    if (dev == NULL) {
        return -EFAULT;
    }
    uint32_t src = filp->pos;
    if (src >= dev->size) {
        return 0;
    }
    if (src + nbytes > dev->size) {
        nbytes = dev->size - src;
    }
    int res = dev->read(dev, dest, src, nbytes);
    if (res < 0) {
        return res;
    }
    /* Advance file position */
    filp->pos += res;
    return res;
}

static ssize_t nvram_vfs_write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    nvram_t *dev = filp->private_data.ptr;
    if (dev == NULL) {
        return -EFAULT;
    }
    uint32_t dest = filp->pos;
    if (dest >= dev->size) {
        return 0;
    }
    if (dest + nbytes > dev->size) {
        nbytes = dev->size - dest;
    }
    int res = dev->write(dev, src, dest, nbytes);
    if (res < 0) {
        return res;
    }
    /* Advance file position */
    filp->pos += res;
    return res;
}

/** @} */

#else
typedef int dont_be_pedantic;
#endif /* MODULE_VFS */
