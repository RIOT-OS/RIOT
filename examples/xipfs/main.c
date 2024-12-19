/*
 * Copyright (C) 2024 Université de Lille
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       An application demonstrating xipfs
 *
 * @author      Damien Amara <damien.amara@univ-lille.fr>
 *
 * @}
 */

#include "fs/xipfs_fs.h"
#include "periph/flashpage.h"
#include "shell.h"
#include "vfs.h"

/**
 * @def PANIC
 *
 * @brief This macro handles fatal errors
 */
#define PANIC() for (;;);

/**
 * @def NVME0P0_PAGE_NUM
 *
 * @brief The number of flash page for the nvme0p0 file system
 */
#define NVME0P0_PAGE_NUM 10

/**
 * @def NVME0P1_PAGE_NUM
 *
 * @brief The number of flash page for the nvme0p1 file system
 */
#define NVME0P1_PAGE_NUM 15

/*
 * Allocate a new contiguous space for the nvme0p0 file system
 */
XIPFS_NEW_PARTITION(nvme0p0, "/dev/nvme0p0", NVME0P0_PAGE_NUM);

/*
 * Allocate a new contiguous space for the nvme0p1 file system
 */
XIPFS_NEW_PARTITION(nvme0p1, "/dev/nvme0p1", NVME0P1_PAGE_NUM);

/**
 * @internal
 *
 * @brief Mount a partition, or if it is corrupted, format and
 * remount it
 *
 * @param xipfs_mp A pointer to a memory region containing an
 * xipfs mount point structure
 */
static void mount_or_format(vfs_xipfs_mount_t *xipfs_mp)
{
    if (vfs_mount(&xipfs_mp->vfs_mp) < 0) {
        printf("vfs_mount: \"%s\": file system has not been "
            "initialized or is corrupted\n", xipfs_mp->vfs_mp.mount_point);
        printf("vfs_format: \"%s\": try initializing it\n",
            xipfs_mp->vfs_mp.mount_point);
        vfs_format(&xipfs_mp->vfs_mp);
        printf("vfs_format: \"%s\": OK\n", xipfs_mp->vfs_mp.mount_point);
        if (vfs_mount(&xipfs_mp->vfs_mp) < 0) {
            printf("vfs_mount: \"%s\": file system is corrupted!\n",
                xipfs_mp->vfs_mp.mount_point);
            PANIC();
        }
    }
    printf("vfs_mount: \"%s\": OK\n", xipfs_mp->vfs_mp.mount_point);
}

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    mount_or_format(&nvme0p0);
    mount_or_format(&nvme0p1);

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
