/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_riotboot_vfs VFS Image loader
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       riotboot as a VFS based bootloader
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#ifndef RIOTBOOT_VFS_H
#define RIOTBOOT_VFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "vfs_util.h"
#include "vfs_default.h"

/**
 * @brief   File that contains the path to the new image
 *
 *          If this file is present, riotboot will attempt to load it
 *          as the new firmware.
 */
#ifndef RIOTBOOT_VFS_UPDATE_FILE
#define RIOTBOOT_VFS_UPDATE_FILE VFS_DEFAULT_DATA "/do_update"
#endif

/**
 * @brief   File that contains the previous firmware
 *
 *          Riotboot will make a backup of the current firmware to
 *          restore the old image if the new one fails to boot.
 */
#ifndef RIOTBOOT_VFS_BACKUP_FILE
#define RIOTBOOT_VFS_BACKUP_FILE VFS_DEFAULT_DATA "/old_image.bin"
#endif

/**
 * @brief   Mark the image behind @p file as the one that should be flashed
 *          the next time riotboot runs.
 *
 *          This only creates the @ref RIOTBOOT_VFS_UPDATE_FILE, it will not perform a reboot.
 *
 * @param[in] file  Path to the firmware image
 *
 * @return 0 on success
 *        <0 on error
 */
static inline int riotboot_vfs_schedule(const char *file)
{
    return vfs_file_from_buffer(RIOTBOOT_VFS_UPDATE_FILE, file, strlen(file));
}

/**
 * @brief   Unschedule a pending firmware update
 *
 *          This removes the flag file created by @ref riotboot_vfs_schedule
 */
void riotboot_vfs_cancel(void);

/**
 * @brief   Dump the current firmware image to a file
 *
 * @param[in] file  Path where the firmware image should be written to
 *
 * @return 0 on success
 *        <0 on error
 */
int riotboot_vfs_dump_rom(const char *file);

#ifdef __cplusplus
}
#endif

#endif /* RIOTBOOT_VFS_H */
