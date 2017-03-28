/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     auto_init_fs
 * @{
 *
 * @file
 * @brief       Automatic mount of DevFS on /dev
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "vfs.h"
#include "fs/devfs.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static vfs_mount_t _devfs_auto_init_mount = {
    .fs = &devfs_file_system,
    .mount_point = "/dev",
};

void auto_init_devfs(void)
{
    DEBUG("auto_init_devfs: mounting /dev\n");
    vfs_mount(&_devfs_auto_init_mount);
}
