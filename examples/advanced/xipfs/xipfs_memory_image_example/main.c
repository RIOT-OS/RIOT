/*
 * SPDX-FileCopyrightText: 2024-2026 Université de Lille
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       An application demonstrating xipfs using a pre-made offboard mount point.
 *
 * @author      Gregory Guche <gregory.guche@univ-lille.fr>
 *
 * @}
 */

#include <fcntl.h>
#include <stdlib.h>

#include "fs/xipfs_fs.h"
#include "periph/flashpage.h"
#include "shell.h"
#include "vfs.h"

/*
 * Include a mount point image that has been built on a PC workstation.
 *
 * Please mind the deliberate missing semicolon character after
 * XIPFS_START_PARTITION_INCLUSION(nvme0p0).
 */
static
XIPFS_START_PARTITION_INCLUSION(nvme0p0)
#include "blob/nvme0p0.flash.h"
XIPFS_END_PARTITION_INCLUSION(nvme0p0, "/nvme0p0", nvme0p0_flash, nvme0p0_flash_len);

static shell_command_t shell_commands[] = { {NULL, NULL, NULL} };

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    if (vfs_mount(&nvme0p0.vfs_mp) < 0) {
        printf("vfs_mount: \"%s\": file system has not been "
               "initialized or is corrupted\n", nvme0p0.vfs_mp.mount_point);
        for (;;) {}
    }
    else {
        printf("vfs_mount: \"%s\": OK\n", nvme0p0.vfs_mp.mount_point);
    }

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
