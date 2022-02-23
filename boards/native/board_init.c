/**
 * Native Board board_init implementation
 *
 * Copyright (C) 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup boards_native
 * @{
 * @file
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @}
 */
#include <stdio.h>
#include "board.h"

#include "board_internal.h"

#ifdef MODULE_MTD
#include "mtd_native.h"

mtd_native_dev_t mtd0_dev = {
    .base = {
        .driver = &native_flash_driver,
        .sector_count = MTD_SECTOR_NUM,
        .pages_per_sector = MTD_SECTOR_SIZE / MTD_PAGE_SIZE,
        .page_size = MTD_PAGE_SIZE,
    },
    .fname = MTD_NATIVE_FILENAME,
};

mtd_dev_t *mtd0 = &mtd0_dev.base;
#endif

#ifdef MODULE_VFS
#include "vfs.h"

/*
 * On `native` we define auto-mounts for every file system.
 *
 * A 'real' board would typically always use the same file system to avoid
 * data loss when re-formatting, but since `native` is for testing only we
 * provide all file system definitions here.
 */

/* littlefs support */
#if defined(MODULE_LITTLEFS)

#include "fs/littlefs_fs.h"
VFS_AUTO_MOUNT(littlefs, VFS_MTD(mtd0_dev), "/nvm", 0);

/* littlefs2 support */
#elif defined(MODULE_LITTLEFS2)

#include "fs/littlefs2_fs.h"
VFS_AUTO_MOUNT(littlefs2, VFS_MTD(mtd0_dev), "/nvm", 0);

/* spiffs support */
#elif defined(MODULE_SPIFFS)

#include "fs/spiffs_fs.h"
VFS_AUTO_MOUNT(spiffs, VFS_MTD(mtd0_dev), "/nvm", 0);

/* FAT support */
#elif defined(MODULE_FATFS_VFS)

#include "fs/fatfs.h"
VFS_AUTO_MOUNT(fatfs, VFS_MTD(mtd0_dev), "/nvm", 0);

#endif
#endif /* MODULE_VFS */

/**
 * Nothing to initialize at the moment.
 * Turns the red LED on and the green LED off.
 */
void board_init(void)
{
    puts("RIOT native board initialized.");
}
