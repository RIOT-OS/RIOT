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

#ifndef FS_FATFS_H
#define FS_FATFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fatfs/ff.h"
#include "vfs.h"

#ifndef FATFS_YEAR_OFFSET
/** The year in FatFs timestamps is relative to this offset */
#define FATFS_YEAR_OFFSET (1980)
#endif

/** The epoch offset is used to convert between FatFs and time_t timestamps */
#define EPOCH_YEAR_OFFSET (1970)

/** Size of the buffer needed for a directory entry -> @attention this should be: sizeof(DIR).
    sizeof(DIR) currently isn't used directly because it's not possible to use that within
    preprocessor-if (see below) */
#define FATFS_DIR_SIZE (44)

/** Size of the buffer needed for directory
    -> should be: sizeof(fatfs_file_desc_t) */
#define FATFS_FILE_SIZE (72)

/** size needed for volume strings like "n:/" where n is the volume id */
#define FATFS_MAX_VOL_STR_LEN (4)

/** 0:mount on first file access, 1 mount in f_mount() call */
#define FATFS_MOUNT_OPT       (1)

/** most FatFs file operations need an absolute path. This defines the size of the
    needed buffer to circumvent stack allocation within vfs-wrappers */
#define FATFS_MAX_ABS_PATH_SIZE (FATFS_MAX_VOL_STR_LEN + VFS_NAME_MAX + 1)

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

    /** most FatFs file operations need an absolute path. This buffer provides
        static memory to circumvent stack allocation within vfs-wrappers */
    char abs_path_str_buff[FATFS_MAX_ABS_PATH_SIZE];
} fatfs_desc_t;

/**
 * info of a single opened file
 */
typedef struct fatfs_file_desc {
    FIL file;                     /**< FatFs work area for a single file */
    char fname[VFS_NAME_MAX + 1]; /**< name of the file (e.g. f_stat uses
                                       filename instead of FIL) */
} fatfs_file_desc_t;

/** The FatFs vfs driver, a pointer to a fatfs_desc_t must be
    provided as vfs_mountp::private_data */
extern const vfs_file_system_t fatfs_file_system;

#ifdef __cplusplus
}
#endif

#endif /* FS_FATFS_H */

/** @} */
