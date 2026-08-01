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
#include "fs/spiffs_fs.h"

static spiffs_desc_t spiffs_desc;

vfs_mount_t test_vfs_mount = {
    .fs = &spiffs_file_system,
    .mount_point = "/test-vfs",
    .private_data = &spiffs_desc,
};

void test_vfs_init(mtd_dev_t *dev)
{
    spiffs_desc.dev = dev;
}
/** @} */
