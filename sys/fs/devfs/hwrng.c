/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_fs_devfs
 * @{
 *
 * @file
 * @brief       Random backends for devfs implementation
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include "vfs.h"

#include "periph/hwrng.h"

static ssize_t hwrng_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes);

const vfs_file_ops_t hwrng_vfs_ops = {
    .read  = hwrng_vfs_read,
};

static ssize_t hwrng_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    (void)filp;

    hwrng_read(dest, nbytes);

    return nbytes;
}
