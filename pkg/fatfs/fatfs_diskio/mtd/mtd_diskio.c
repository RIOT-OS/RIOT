/*
 * Copyright (C) 2017 Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup  pkg_fatfs
 * @{
 *
 * @file
 * @brief    Implementation of fatfs diskio interface that supports mtd driver
 *           based on low level disk I/O module example for FatFs by ChaN, 2016
 *
 * @author   Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */

#include "fatfs_diskio_mtd.h"
#include "ffconf.h"
#include "mtd.h"
#define ENABLE_DEBUG 0
#include "debug.h"

#if FATFS_FFCONF_OPT_FS_NORTC == 0
#include "periph/rtc.h"
#endif

/* mtd devices for use by FatFs should be provided by the application */
extern mtd_dev_t *fatfs_mtd_devs[FF_VOLUMES];

/**
 * @brief           returns the status of the disk
 *
 * @param[in] pdrv  drive number to identify the drive
 *
 * @return          STA_NODISK if no disk exists with the given id
 * @return          0 if disk is initialized
 * @return          STA_NOINIT if disk id exists, but disk isn't initialized
 */
DSTATUS disk_status(BYTE pdrv)
{
    DEBUG("disk_status %d\n", pdrv);
    if (pdrv >= FF_VOLUMES) {
        return STA_NODISK;
    } else if (fatfs_mtd_devs[pdrv]->driver == NULL){
        return STA_NOINIT;
    }

    return FATFS_DISKIO_DSTASTUS_OK;
}

/**
 * @brief           initializes the disk
 *
 * @param[in] pdrv  drive number to identify the drive
 *
 * @return          STA_NODISK if no disk exists with the given id
 * @return          0 if disk was initialized successfully
 * @return          STA_NOINIT if disk id exists, but couldn't be initialized
 */
DSTATUS disk_initialize(BYTE pdrv)
{
    DEBUG("disk_initialize %d\n", pdrv);
    if (pdrv >= FF_VOLUMES) {
        return STA_NODISK;
    } else if (fatfs_mtd_devs[pdrv]->driver == NULL){
        return STA_NOINIT;
    }

    if (mtd_init(fatfs_mtd_devs[pdrv])) {
        return STA_NOINIT;
    }

    uint32_t sector_size = fatfs_mtd_devs[pdrv]->page_size
                         * fatfs_mtd_devs[pdrv]->pages_per_sector;
    if (sector_size > FF_MAX_SS) {
        assert(0);
        return STA_NOINIT;
    }

    return FATFS_DISKIO_DSTASTUS_OK;
}

/**
 * @brief              reads sectors from disk
 *
 * @param[in]  pdrv    drive number to identify the drive
 * @param[out] buff    Data buffer to store read data
 * @param[in]  sector  Start sector in LBA
 * @param[in]  count   Number of sectors to read
 *
 * @return             RES_OK if no error occurred
 * @return             RES_ERROR if data wasn't read completely
 * @return             RES_PARERR if prdv is invalid or has no mtd-driver set
 */
DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    DEBUG("disk_read: %d, %lu, %d\n", pdrv, (long unsigned)sector, count);
    if ((pdrv >= FF_VOLUMES) || (fatfs_mtd_devs[pdrv]->driver == NULL)) {
        return RES_PARERR;
    }

    uint32_t sector_size = fatfs_mtd_devs[pdrv]->page_size
                         * fatfs_mtd_devs[pdrv]->pages_per_sector;

    int res = mtd_read_page(fatfs_mtd_devs[pdrv], buff,
                            sector, 0, count * sector_size);

    if (res != 0) {
        return RES_ERROR;
    }
    return RES_OK;
}

/**
 * @brief              writes sectors to disk
 *
 * @param[in]  pdrv    Physical drive nmuber to identify the drive
 * @param[in]  buff    Data to be written
 * @param[in]  sector  Start sector in LBA
 * @param[in]  count   Number of sectors to write
 *
 * @return             RES_OK if no error occurred
 * @return             RES_ERROR if data wasn't written completely
 * @return             RES_PARERR if prdv is invalid or has no mtd-driver set
 */
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    mtd_dev_t *mtd = fatfs_mtd_devs[pdrv];
    DEBUG("disk_write: %d, %lu, %d\n", pdrv, (long unsigned)sector, count);
    if ((pdrv >= FF_VOLUMES) || (mtd->driver == NULL)) {
        return RES_PARERR;
    }

    int res = mtd_write_sector(mtd, buff, sector, count);
    if (res != 0) {
        return RES_ERROR;
    }
    return RES_OK;
}

/**
 * @brief                  perform miscellaneous low-level control functions
 *
 * @param[in]      pdrv    Physical drive nmuber (0..FF_VOLUMES-1)
 * @param[in,out]  cmd     Control code
 * @param[in]      buff    Buffer to send/receive control data
 *
 * @return                 RES_OK if no error occurred
 * @return                 RES_PARERR if cmd is unknown or
 *                         prdv is either invalid or has no mtd-driver set
 */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    (void)buff;

    if ((pdrv >= FF_VOLUMES) || (fatfs_mtd_devs[pdrv]->driver == NULL)) {
        return RES_PARERR;
    }

    switch (cmd) {
#if (FF_FS_READONLY == 0)
        case CTRL_SYNC:
            /* r/w is always finished within r/w-functions of mtd */
            return RES_OK;
#endif

#if (FF_USE_MKFS == 1)
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = fatfs_mtd_devs[pdrv]->sector_count;
            return RES_OK;

        /*Erase block size in number of sectors (1 to 32768 in power of 2).
          Return 1 if the erase block size is unknown. */
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = fatfs_mtd_devs[pdrv]->pages_per_sector;
            return RES_OK;
#endif

#if (FF_MAX_SS != FF_MIN_SS)
        case GET_SECTOR_SIZE:
            *(DWORD *)buff = fatfs_mtd_devs[pdrv]->page_size;
            return RES_OK;
#endif

#if (FF_USE_TRIM == 1)
        case CTRL_TRIM:
            return RES_OK;
#endif
    }

    return RES_PARERR;
}

#if FATFS_FFCONF_OPT_FS_NORTC == 0
/**
 * @brief                  store local time in a 32-bit bitfiled
 *
 * @return                 timestamp as bitfield
 */
DWORD get_fattime(void)
{
    struct tm time;

    rtc_get_time(&time);

    /* bit 31:25 Year origin from 1980 (0..127) */
    uint8_t year = time.tm_year + RTC_YEAR_OFFSET - FATFS_YEAR_OFFSET;
    uint8_t month = time.tm_mon + 1;        /* bit 24:21 month (1..12) */
    uint8_t day_of_month = time.tm_mday;    /* bit 20:16 day (1..31) */
    uint8_t hour = time.tm_hour;            /* bit 15:11 hour (0..23) */
    uint8_t minute = time.tm_min;           /* bit 10:5 minute (0..59) */
    uint8_t second = (time.tm_sec / 2);     /* bit 4:0 second/2 (0..29) */

    return (DWORD)year << FATFS_DISKIO_FATTIME_YEAR_OFFS |
           (DWORD)month << FATFS_DISKIO_FATTIME_MON_OFFS |
           (DWORD)day_of_month << FATFS_DISKIO_FATTIME_DAY_OFFS |
           hour << FATFS_DISKIO_FATTIME_HH_OFFS |
           minute << FATFS_DISKIO_FATTIME_MM_OFFS |
           second;
}
#endif
