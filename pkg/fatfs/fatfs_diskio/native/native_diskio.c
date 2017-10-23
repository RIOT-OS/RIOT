/*
 * Copyright (C) 2016 Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup  sys_fatfs_diskio
 * @{
 *
 * @file
 * @brief    Implementation of FatFs interface that makes use of a fatfs image
 *           file instead of hardware to allow FatFs usage/testing on native.
 *           Based on low level disk I/O module example for FatFs by ChaN, 2016
 *
 * @author   Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */
#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "fatfs/ffconf.h"
#include "fatfs/diskio.h"
#include "fatfs_diskio_common.h"
#include "fatfs/integer.h"
#include "periph/rtc.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

bool rtc_init_done = false;

typedef struct {
    const char *image_path;
    FILE *fd;
    bool opened;
} dummy_volume_t;

static dummy_volume_t volume_files[] = {
    {
        .image_path = FATFS_DISKIO_NATIVE_DEFAULT_FILE,
        .fd = NULL,
        .opened  = false
    },
};

static inline dummy_volume_t *get_volume_file(uint32_t idx)
{
    if (idx < sizeof(volume_files) / sizeof(dummy_volume_t)) {
        return &volume_files[idx];
    }
    else {
        return NULL;
    }
}

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
    dummy_volume_t *volume = get_volume_file(pdrv);

    if (volume == NULL) {
        return STA_NODISK;
    }
    if (volume->opened) {
        return FATFS_DISKIO_DSTASTUS_OK;
    }
    else {
        return STA_NOINIT;
    }
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
    dummy_volume_t *volume = get_volume_file(pdrv);
    DEBUG("disk_initialize: %d\n", pdrv);
    if (volume == NULL) {
        return STA_NODISK;
    }

    if (volume->opened) { /* if volume is already opened close it first */
        fclose(volume->fd);
        volume->opened = false;
    }

    /* open file for r/w but don't create if it doesn't exist */
    FILE *fd = fopen(volume->image_path, "r+");
    DEBUG("fd: %p\n", (void *)fd);
    if (fd == NULL) {
        DEBUG("diskio_native.c: disk_initialize: fopen: "
              "errno: 0x%08x\n", errno);
        return STA_NOINIT;
    }
    else {
        volume->fd = fd;
        volume->opened = true;
        return FATFS_DISKIO_DSTASTUS_OK;
    }
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
 * @return             RES_NOTRDY if data wasn't read completely
 */
DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    dummy_volume_t *volume = get_volume_file(pdrv);

    if ((volume != NULL) && volume->opened) {
        /* set read pointer to secor equivalent position */
        if (fseek(volume->fd, sector * FIXED_BLOCK_SIZE, SEEK_SET) == 0) {
            if (fread(buff, FIXED_BLOCK_SIZE, count, volume->fd) == count) {
                return RES_OK;
            }
            else {
                DEBUG("diskio_native.c: disk_read: fread: "
                      "errno: 0x%08x\n", errno);
            }
        }
        else {
            DEBUG("diskio_native.c: disk_read: fseek: errno: 0x%08x\n", errno);
        }
    }

    return RES_NOTRDY;
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
 * @return             RES_NOTRDY if data wasn't written completely
 */
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    dummy_volume_t *volume = get_volume_file(pdrv);

    if ((volume != NULL) && volume->opened) {
        /* set write pointer to secor equivalent position */
        if (fseek(volume->fd, sector * FIXED_BLOCK_SIZE, SEEK_SET) == 0) {
            if (fwrite(buff, FIXED_BLOCK_SIZE, count, volume->fd) == count) {
                if (fflush(volume->fd) == 0) {
                    return RES_OK;
                }
                else {
                    DEBUG("diskio_native.c: disk_write: fflush: "
                          "errno: 0x%08x\n", errno);
                }
            }
            else {
                DEBUG("diskio_native.c: disk_write: fwrite: "
                      "errno: 0x%08x\n", errno);
            }
        }
        else {
            DEBUG("diskio_native.c: disk_write: fseek: errno: 0x%08x\n", errno);
        }
    }
    return RES_NOTRDY;
}

/**
 * @brief                  perform miscellaneous low-level control functions
 *
 * @param[in]      pdrv    Physical drive nmuber (0..)
 * @param[in out]  cmd     Control code
 * @param[in]      sector  Buffer to send/receive control data
 *
 * @return                 RES_OK if no error occurred
 * @return                 RES_ERROR if an error occurred
 * @return                 RES_PARERR if an error occurred
 */
DRESULT disk_ioctl(
    BYTE pdrv,      /*  */
    BYTE cmd,       /*  */
    void *buff      /* Buffer to send/receive control data */
    )
{
    (void) pdrv;    /* prevent warning about unused param */
    (void) buff;    /* prevent warning about unused param */

    #if (_USE_MKFS == 1)
    dummy_volume_t *volume;
    struct stat s;
    #endif

    DEBUG("disk_ioctl: %d\n", cmd);

    switch (cmd) {
        #if (_FS_READONLY == 0)
        case CTRL_SYNC:
            /* r/w is always finished within r/w-functions */
            return RES_OK;
        #endif

        #if (_MAX_SS != _MIN_SS)
        case GET_SECTOR_SIZE;
            *buff = FIXED_BLOCK_SIZE;
            return RES_OK;
        #endif

        #if (_USE_MKFS == 1)
        case GET_SECTOR_COUNT:
            volume = get_volume_file(pdrv);
            DEBUG("GET_SECTOR_COUNT: volume: %p\n", (void*)volume);

            if ((volume != NULL) && volume->opened) {
                if (stat(volume->image_path, &s) == 0) {
                    *(DWORD *)buff = s.st_size / FIXED_BLOCK_SIZE;
                    DEBUG("GET_SECTOR_COUNT\n");
                    return RES_OK;
                }
                DEBUG("GET_SECTOR_COUNT: RES_ERROR\n");
            }
            return RES_ERROR;

        case GET_BLOCK_SIZE:
            *(DWORD *)buff = FIXED_BLOCK_SIZE;
            DEBUG("GET_BLOCK_SIZE: %d\n", FIXED_BLOCK_SIZE);
            return RES_OK;
        #endif

        #if (_USE_TRIM == 1)
        case CTRL_TRIM:
            return RES_OK;
        #endif
    }

    return RES_PARERR;
}
