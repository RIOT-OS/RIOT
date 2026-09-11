/*
 * SPDX-FileCopyrightText: 2022 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for vfs_default
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <stdio.h>

#include "vfs_default.h"

int main(void)
{
    vfs_DIR mount = {0};

    /* list mounted file systems */
    puts("mount points:");
    while (vfs_iterate_mount_dirs(&mount)) {
        printf("\t%s\n", mount.mp->mount_point);
    }
    printf("\ndata dir: %s\n", VFS_DEFAULT_DATA);

    return 0;
}
