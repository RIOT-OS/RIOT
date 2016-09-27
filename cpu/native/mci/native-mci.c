/*
 * Copyright (C) Lucas Jenß
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup native
 * @{
 */

/**
 * @file
 * @brief   Implements the MCI storage driver by using the flash_sim component
 *          as backend. It is 8 MiB by default and its size can be increased
 *          using `NATIVE_MCI_SIZE_MIB` which specifies the MCI storage size in MiB.
 *
 *          Additional properties are:
 *
 *              Sector (page) size: 512 B
 *              Erase block size: 512 KiB (524288 Bytes)
 *
 * @author  Lucas Jenß <lucas@x3ro.de>
 *
 */

#include <string.h>
#include <stdbool.h>
#include "diskio.h"

#include "storage/flash_sim.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifndef NATIVE_MCI_SIZE_MIB
#define NATIVE_MCI_SIZE_MIB 8
#endif

#define NATIVE_MCI_SIZE (1024 * 1024 * NATIVE_MCI_SIZE_MIB)
#define NATIVE_MCI_SECTOR_SIZE (512)
#define NATIVE_MCI_BLOCK_SIZE (512 * 1024)
#define NATIVE_MCI_SECTOR_COUNT (NATIVE_MCI_SIZE / NATIVE_MCI_SECTOR_SIZE)

flash_sim fs;

bool _validate_parameters(unsigned long sector, unsigned char count) {
    if(count < 1) {
        return false;
    }

    if(sector + count > NATIVE_MCI_SECTOR_COUNT) {
        return false;
    }

    return true;
}

diskio_sta_t mci_initialize(void) {
    fs.page_size = NATIVE_MCI_SECTOR_SIZE;
    fs.block_size = NATIVE_MCI_BLOCK_SIZE;
    fs.storage_size = NATIVE_MCI_SIZE;

    if(flash_sim_init(&fs) == 0) {
        return DISKIO_RES_OK;
    } else {
        return DISKIO_RES_ERROR;
    }
}

diskio_sta_t mci_status(void) {
    return DISKIO_RES_OK;
}

/* Read |count| sectors starting at |sector| (LBA) */
diskio_result_t mci_read(unsigned char *buff, unsigned long sector, unsigned char count) {
    if(!_validate_parameters(sector, count)) {
        return DISKIO_RES_PARERR;
    }

    for(unsigned int i=0; i<count; i++) {
        unsigned char *page_buffer = buff + (i * NATIVE_MCI_SECTOR_SIZE);
        if(flash_sim_read(&fs, page_buffer, sector + i) != 0) {
            return DISKIO_RES_ERROR;
        }
    }

    return DISKIO_RES_OK;
}

/* Write |count| sectors starting at |sector| (LBA) */
diskio_result_t mci_write(const unsigned char *buff, unsigned long sector, unsigned char count) {
    if(!_validate_parameters(sector, count)) {
        return DISKIO_RES_PARERR;
    }

    for(unsigned int i=0; i<count; i++) {
        unsigned char *page_buffer = ((unsigned char*) buff) + (i * NATIVE_MCI_SECTOR_SIZE);
        if(flash_sim_write(&fs, page_buffer, sector + i) != 0) {
            return DISKIO_RES_ERROR;
        }
    }

    return DISKIO_RES_OK;
}

diskio_result_t mci_ioctl(
    unsigned char ctrl,     /* Control code */
    void *buff              /* Buffer to send/receive data block */
) {
    diskio_result_t res = DISKIO_RES_ERROR;
    unsigned long block;

    switch(ctrl) {

        /* Get number of sectors on the disk (unsigned long) */
        case GET_SECTOR_COUNT:
            *(unsigned long *) buff = NATIVE_MCI_SECTOR_COUNT;
            res = DISKIO_RES_OK;
            break;

        /* Get sector (page) size (unsigned short) */
        case GET_SECTOR_SIZE:
            *(unsigned short *) buff = NATIVE_MCI_SECTOR_SIZE;
            res = DISKIO_RES_OK;
            break;

        /* Get erase block size (unsigned long) */
        case GET_BLOCK_SIZE:
            *(unsigned long *) buff = NATIVE_MCI_BLOCK_SIZE;
            res = DISKIO_RES_OK;
            break;

        /* Erase a block of sectors */
        case CTRL_ERASE_SECTOR:
            block = *(unsigned long *)buff;

            if(!_validate_parameters(block, 1)) {
                return DISKIO_RES_PARERR;
            }

            if(flash_sim_erase(&fs, block/1024) == 0) {
                res = DISKIO_RES_OK;
            } else {
                res = DISKIO_RES_ERROR;
            }

            break;

        /* Get card type flags (1 byte) */
        case MMC_GET_TYPE:
            // Not implemented
            break;

        /* Get CSD (16 bytes) */
        case MMC_GET_CSD:
            // Not implemented
            break;

        /* Get CID (16 bytes) */
        case MMC_GET_CID:
            // Not implemented
            break;

        /* Get OCR (4 bytes) */
        case MMC_GET_OCR:
            // Not implemented
            break;

        /* Receive SD status as a data block (64 bytes) */
        case MMC_GET_SDSTAT:
            // Not implemented
            break;

        /* No-Ops for virtual MCI */
        case CTRL_SYNC: /* Make sure that all data has been written on the media */
        case CTRL_POWER: /* Power on/off */
            res = DISKIO_RES_OK;
            break;

        default:
            res = DISKIO_RES_PARERR;
    }

    return res;
}
