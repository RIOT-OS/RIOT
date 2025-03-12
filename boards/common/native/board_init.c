/**
 * Native Board board_init implementation
 *
 * Copyright (C) 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup boards_common_native
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
        .write_size = MTD_WRITE_SIZE,
    },
    .fname = MTD_NATIVE_FILENAME,
};

MTD_XFA_ADD(mtd0_dev.base, 0);
#endif

#ifdef MODULE_VFS_DEFAULT
#include "vfs_default.h"

/*
 * On `native` we define auto-mounts for every file system.
 *
 * A 'real' board would typically always use the same file system to avoid
 * data loss when re-formatting, but since `native` is for testing only we
 * provide all file system definitions here.
 */

/* littlefs support */
#if defined(MODULE_LITTLEFS)
VFS_AUTO_MOUNT(littlefs, VFS_MTD(mtd0_dev), VFS_DEFAULT_NVM(0), 0);

/* littlefs2 support */
#elif defined(MODULE_LITTLEFS2)
VFS_AUTO_MOUNT(littlefs2, VFS_MTD(mtd0_dev), VFS_DEFAULT_NVM(0), 0);

/* spiffs support */
#elif defined(MODULE_SPIFFS)
VFS_AUTO_MOUNT(spiffs, VFS_MTD(mtd0_dev), VFS_DEFAULT_NVM(0), 0);

/* FAT support */
#elif defined(MODULE_FATFS_VFS)
VFS_AUTO_MOUNT(fatfs, VFS_MTD(mtd0_dev), VFS_DEFAULT_NVM(0), 0);

/* ext2/3/4 support */
#elif defined(MODULE_LWEXT4)
VFS_AUTO_MOUNT(lwext4, VFS_MTD(mtd0_dev), VFS_DEFAULT_NVM(0), 0);

/* host fs pass-through */
#elif defined(MODULE_FS_NATIVE)
VFS_AUTO_MOUNT(native, { .hostpath = FS_NATIVE_DIR }, VFS_DEFAULT_NVM(0), 0);

#endif
#endif /* MODULE_VFS_DEFAULT */

/**
 * Nothing to initialize at the moment.
 * Turns the red LED on and the green LED off.
 */
void board_init(void)
{
    puts("RIOT " RIOT_BOARD " board initialized.");
}
