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

#define RTC_YEAR_OFFSET   (1900)
#define FATFS_YEAR_OFFSET (1980)

#define FIXED_BLOCK_SIZE (512)

#define FATFS_DISKIO_DSTASTUS_OK (0)

#define FATFS_DISKIO_FATTIME_YEAR_OFFS (25)
#define FATFS_DISKIO_FATTIME_MON_OFFS  (21)
#define FATFS_DISKIO_FATTIME_DAY_OFFS  (16)
#define FATFS_DISKIO_FATTIME_HH_OFFS   (11)
#define FATFS_DISKIO_FATTIME_MM_OFFS   (5)

#ifdef __cplusplus
}
#endif

#endif /* FATFS_DISKIO_COMMON_H */
/** @} */
