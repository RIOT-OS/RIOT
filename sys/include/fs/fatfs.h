/*
 * Copyright (C) 2017 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_fatfs  FatFs integration
 * @ingroup     sys_fs
 * @{
 *
 * @file
 * @brief       FatFs integration for vfs
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifndef FATFS_H
#define FATFS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VFS_DIR_BUFFER_SIZE
#define VFS_DIR_BUFFER_SIZE (44)
#endif

#ifndef VFS_FILE_BUFFER_SIZE
#define VFS_FILE_BUFFER_SIZE (72)
#endif

#include "fatfs/ff.h"
#include "vfs.h"

#ifndef FATFS_YEAR_OFFSET
#define FATFS_YEAR_OFFSET (1980)
#endif

#define EPOCH_YEAR_OFFSET (1970)

/** Size of the buffer needed for directory -> should be: sizeof(DIR)*/
#define FATFS_DIR_SIZE (44)
/** the problem with the above is: it's not possible to use sizeof(DIR) as this is later usen in #if (see below) */

/** Size of the buffer needed for directory -> should be: sizeof(fatfs_file_desc_t)*/
#define FATFS_FILE_SIZE (72)

#define FATFS_MAX_VOL_STR_LEN (4)   /**< size needed for volume strings like "n:/" where n is the volume id */
#define FATFS_MOUNT_OPT       (1)   /**< 0:mount on first file access, 1 mount in f_mount() call */

#if (VFS_DIR_BUFFER_SIZE < FATFS_DIR_SIZE)
#error "VFS_DIR_BUFFER_SIZE too small"
#endif

#if (VFS_FILE_BUFFER_SIZE < FATFS_FILE_SIZE)
#error "VFS_FILE_BUFFER_SIZE too small"
#endif

/**
 * needed info to run a FatFs instance
 */
typedef struct fatfs_desc {
    FATFS fat_fs;       /**< FatFs work area needed for each volume */
    uint8_t vol_idx;    /**< low level device that is used by FatFs */
} fatfs_desc_t;

/**
 * info of a single opened file
 */
typedef struct fatfs_file_desc {
    FIL file;                       /**< FatFs work area for a single file */
    char fname[VFS_NAME_MAX + 1]; /**< name of the file (some FatFs functions e.g. f_stat use filename instead of FIL) */
} fatfs_file_desc_t;

/** The FatFs vfs driver, a pointer to a fatfs_desc_t must be provided as vfs_mountp::private_data */
extern const vfs_file_system_t fatfs_file_system;

#ifdef __cplusplus
}
#endif

#endif /* FATFS_H */

/** @} */
