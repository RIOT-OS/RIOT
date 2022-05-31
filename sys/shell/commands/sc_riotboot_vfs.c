/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell command to interact with the VFS based riotboot
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "riotboot/vfs.h"
#include "periph/pm.h"

int _cmd_dump_rom(int argc, char **argv)
{
    int res;

    if (argc != 2) {
        printf("usage: %s <file>\n", argv[0]);
        return -1;
    }

    res = riotboot_vfs_dump_rom(argv[1]);

    if (res > 0) {
        printf("wrote %d bytes to %s\n", res, argv[1]);
        return 0;
    } else {
        printf("can't write to %s: %d\n", argv[1], res);
        return res;
    }
}

int _cmd_riotboot_vfs_flash(int argc, char **argv)
{
    int res;
    char *file = argv[1];

    if (argc != 2) {
        printf("usage: %s <file>\n", argv[0]);
        return -1;
    }

    int fd = vfs_open(file, O_RDONLY, 0);
    if (fd < 0) {
        printf("can't open %s\n", file);
        return fd;
    }
    vfs_close(fd);

    res = riotboot_vfs_schedule(file);

    if (res > 0) {
        pm_reboot();
        return 0;
    }

    return res;
}
