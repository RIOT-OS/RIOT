/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init
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
#include "random-vfs.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static vfs_mount_t _devfs_auto_init_mount = {
    .fs = &devfs_file_system,
    .mount_point = "/dev",
};

#ifdef MODULE_DEVFS_HWRNG
static devfs_t hwrng_devfs = {
    .path = "/hwrng",
    .f_op = &hwrng_vfs_ops,
};
#endif

#ifdef MODULE_DEVFS_RANDOM
static devfs_t random_devfs = {
    .path = "/urandom",
    .f_op = &random_vfs_ops,
};
#endif

void auto_init_devfs(void)
{
    DEBUG("auto_init_devfs: mounting /dev\n");
    vfs_mount(&_devfs_auto_init_mount);

#ifdef MODULE_DEVFS_HWRNG
    devfs_register(&hwrng_devfs);
#endif

#ifdef MODULE_DEVFS_RANDOM
    devfs_register(&random_devfs);
#endif
}
