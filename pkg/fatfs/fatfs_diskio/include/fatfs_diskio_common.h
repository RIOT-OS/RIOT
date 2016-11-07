/*
 * Copyright (C) 2016 Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_fatfs_diskio
 * @brief
 * @{
 *
 * @brief       Common defines for fatfs low-level diskio defines
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifndef FATFS_DISKIO_COMMON_H
#define FATFS_DISKIO_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fatfs/diskio.h" /* FatFs lower layer API */

#define CTRL_SYNC        0 /* Complete pending write process
                              (needed when _FS_READONLY == 0) */

#define GET_SECTOR_COUNT 1 /* (needed when _USE_MKFS == 1) */

#define GET_SECTOR_SIZE  2 /* (needed when _MAX_SS > 512 ) */

#define GET_BLOCK_SIZE   3 /* erase block size (needed when _USE_MKFS == 1) */

#define CTRL_TRIM        4 /* Inform device about no longer needed data blocks
                              (needed when _USE_TRIM == 1) */

#define RTC_YEAR_OFFSET   1900
#define FATFS_YEAR_OFFSET 1980

#define FIXED_BLOCK_SIZE 512

#define FATFS_DISKIO_DSTASTUS_OK 0

#ifdef __cplusplus
}
#endif

#endif /* FATFS_DISKIO_COMMON_H */
/** @} */
