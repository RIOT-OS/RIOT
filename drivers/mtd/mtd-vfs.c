/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#if MODULE_MTD

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "mtd.h"
#include "vfs.h"

/**
 * @ingroup     mtd
 * @{
 *
 * @file
 *
 * @brief       MTD generic VFS operations
 *
 * This allows the MTD driver to register as a node on DevFS
 *
 * See boards/mulle or tests/unittests/tests-devfs for examples on how to use.
 *
 * Tested with mtd_spi_nor on Mulle
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

static int mtd_vfs_fstat(vfs_file_t *filp, struct stat *buf);
static off_t mtd_vfs_lseek(vfs_file_t *filp, off_t off, int whence);
static ssize_t mtd_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes);
static ssize_t mtd_vfs_write(vfs_file_t *filp, const void *src, size_t nbytes);

const vfs_file_ops_t mtd_vfs_ops = {
    .fstat = mtd_vfs_fstat,
    .lseek = mtd_vfs_lseek,
    .read  = mtd_vfs_read,
    .write = mtd_vfs_write,
};

static int mtd_vfs_fstat(vfs_file_t *filp, struct stat *buf)
{
    if (buf == NULL) {
        return -EFAULT;
    }
    mtd_dev_t *mtd = filp->private_data.ptr;
    if (mtd == NULL) {
        return -EFAULT;
    }
    buf->st_nlink = 1;
    buf->st_size = mtd->page_size * mtd->sector_count * mtd->pages_per_sector;
    return 0;
}

static off_t mtd_vfs_lseek(vfs_file_t *filp, off_t off, int whence)
{
    mtd_dev_t *mtd = filp->private_data.ptr;
    if (mtd == NULL) {
        return -EFAULT;
    }
    switch (whence) {
        case SEEK_SET:
            break;
        case SEEK_CUR:
            off += filp->pos;
            break;
        case SEEK_END:
            off += mtd->page_size * mtd->sector_count * mtd->pages_per_sector;
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

static ssize_t mtd_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    mtd_dev_t *mtd = filp->private_data.ptr;
    uint32_t size = mtd->page_size * mtd->sector_count * mtd->pages_per_sector;
    if (mtd == NULL) {
        return -EFAULT;
    }
    uint32_t src = filp->pos;
    if (src >= size) {
        return 0;
    }
    if ((src + nbytes) > size) {
        nbytes = size - src;
    }
    int res = mtd_read(mtd, dest, src, nbytes);
    if (res < 0) {
        return res;
    }
    /* Advance file position */
    filp->pos += res;
    return res;
}

static ssize_t mtd_vfs_write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    mtd_dev_t *mtd = filp->private_data.ptr;
    uint32_t size = mtd->page_size * mtd->sector_count * mtd->pages_per_sector;
    if (mtd == NULL) {
        return -EFAULT;
    }
    uint32_t dest = filp->pos;
    if (dest >= size) {
        return 0;
    }
    if ((dest + nbytes) > size) {
        nbytes = size - dest;
    }
    int res = mtd_write(mtd, src, dest, nbytes);
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
