/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include "vfs.h"
#include "fs/littlefs2_fs.h"

static littlefs2_desc_t littlefs_desc;

vfs_mount_t test_vfs_mount = {
    .fs = &littlefs2_file_system,
    .mount_point = "/test-vfs",
    .private_data = &littlefs_desc,
};

void test_vfs_init(mtd_dev_t *dev)
{
    littlefs_desc.dev = dev;
}
/** @} */
