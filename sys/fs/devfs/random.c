/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "random.h"

static ssize_t random_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes);

const vfs_file_ops_t random_vfs_ops = {
    .read = random_vfs_read,
};

static ssize_t random_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    (void)filp;
    random_bytes(dest, nbytes);

    return nbytes;
}
