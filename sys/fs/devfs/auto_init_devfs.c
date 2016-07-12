/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of ADC lines directly mapped to SAUL reg
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "vfs.h"
#include "fs/devfs.h"

#define ENABLE_DEBUG (1)
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
