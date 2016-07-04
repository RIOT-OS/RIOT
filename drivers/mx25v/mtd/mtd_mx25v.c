/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     mx25v mtd driver
 * @{
 *
 * @file
 * @brief       Memory Technology Device interface implementation for mx25v chip
 *
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 *
 * @}
 */
#include <string.h>
#include "xtimer.h"

#include "mtd_mx25v.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#define MX25V_MEMORY_TYPE       MEMORY_TYPE_NOR

#define SHIFT_BIT_BLOCK         (uint8_t)(16)
#define SHIFT_BIT_SECTOR        (uint8_t)(12)
#define SHIFT_BIT_PAGE          (uint8_t)(8)

static mtd_sta_t mx25v_mtd_init(mtd_dev_t *dev);
static mtd_sta_t mx25v_mtd_status(mtd_dev_t *dev);
static mtd_res_t mx25v_mtd_read(mtd_dev_t *dev, void *buf, uint32_t addr, uint32_t size);
static mtd_res_t mx25v_mtd_write(mtd_dev_t *dev, const void *buf, uint32_t addr, uint32_t count);
static mtd_res_t mx25v_mtd_erase(mtd_dev_t *dev, uint32_t addr, uint32_t count);
static mtd_res_t mx25v_mtd_ioctl(mtd_dev_t *dev, unsigned char ctrl, void *buf);

const mtd_desc_t mx25v_flash_interface =
{
    .init = mx25v_mtd_init,
    .status = mx25v_mtd_status,
    .read = mx25v_mtd_read,
    .write = mx25v_mtd_write,
    .erase = mx25v_mtd_erase,
    .ioctl = mx25v_mtd_ioctl,
};

static mtd_sta_t mx25v_mtd_init(mtd_dev_t *dev)
{
    mtd_sta_t status = MTD_STA_NOINIT;
    int res = 0x45;
    mx25v_id_t mx25v_id = {0x45, 0x45, 0x45};

    mx25v_t* mx25v_flash;
    mx25v_flash = (mx25v_t*)(dev);

    DEBUG("mtd_mx25v: init");

    res = mx25v_init(mx25v_flash);

    if (res == 0) {
        if (mx25v_read_id(mx25v_flash, &mx25v_id) == 0) {
            if ((mx25v_id.manufacturer_id == MX25V_MANUFACTURER_ID) &&
                (mx25v_id.memory_id == MX25V_MEMORY_ID) &&
                (mx25v_id.device_id == MX25V_DEVICE_ID) ){
                status = mx25v_mtd_status(dev);
            }
            else {
                status = MTD_STA_NODISK;
            }
        }
        else {
            status = MTD_STA_NODISK;
        }
    }
    else {
        status = MTD_STA_NOINIT;
    }

    return status;
}

static mtd_sta_t mx25v_mtd_status(mtd_dev_t *dev)
{
    mtd_sta_t status = MTD_STA_NOINIT;
    int res = 0x45;
    char read_buffer;

    mx25v_t* mx25v_flash;
    mx25v_flash = (mx25v_t*)(dev);

    res = mx25v_read_status(mx25v_flash, &read_buffer);
    if (res == 0) {
        if ((read_buffer & (MX25V_BP0 | MX25V_BP1 | MX25V_BP2)) == 0) {
            status = MTD_STA_INIT;
        }
        else {
            status = MTD_STA_PROTECT;
        }
    }
    else {
        status = MTD_STA_NODISK;
    }

    return status;
}

static mtd_res_t mx25v_mtd_read(mtd_dev_t *dev, void* buf, uint32_t addr, uint32_t size)
{
    mtd_res_t status = MTD_RES_WRPRT;
    int res = 0x45;

    mtd_mx25v_dev_t *mtd = (mtd_mx25v_dev_t *) dev;

    res = mx25v_read_buffer(&mtd->dev, addr, buf, size);

    if (res == 0) {
        status = MTD_RES_OK;
    }
    else if (res == -1) {
        status = MTD_RES_ERROR;
    }
    else if (res == -2) {
        status = MTD_RES_PARERR;
    }

    return status;
}

static mtd_res_t mx25v_mtd_write(mtd_dev_t *dev, const void *buf, uint32_t addr, uint32_t size)
{
    mtd_res_t status = MTD_RES_WRPRT;
    int res = 0x45;

    mtd_mx25v_dev_t *mtd = (mtd_mx25v_dev_t *) dev;

    res = mx25v_write_buffer(&mtd->dev, addr, buf, size);

    if (res == 0) {
        status = MTD_RES_OK;
    }
    else if (res == -1) {
        status = MTD_RES_ERROR;
    }
    else if (res == -2) {
        status = MTD_RES_PARERR;
    }
    else if (res == -3) {
        status = MTD_RES_NOTRDY;
    }
    else if (res == -4) {
        status = MTD_RES_NOTRDY;
    }

    return status;
}


static mtd_res_t mx25v_mtd_erase(mtd_dev_t *dev, unsigned long addr, unsigned long size)
{
    mtd_res_t status = MTD_RES_WRPRT;
    int res = 0x45;

    mtd_mx25v_dev_t *mtd = (mtd_mx25v_dev_t *) dev;

    uint8_t cur_size = 0;

    /* At least one sector to erase */
    if (size >= MX25V_SECTOR_SIZE) {
        cur_size = 0;
        while ((cur_size < size) && (res >= 0) ) {
            addr = addr + cur_size;

            res = mx25v_erase_sector(&mtd->dev, addr);
            cur_size += MX25V_SECTOR_SIZE;
        }

        if (res == 0) {
            status = MTD_RES_OK;
        }
        else if (res == -1) {
            status = MTD_RES_ERROR;
        }
        else if (res == -2) {
            status = MTD_RES_PARERR;
        }
        else if (res == -3) {
            status = MTD_RES_NOTRDY;
        }
        else if (res == -4) {
            status = MTD_RES_NOTRDY;
        }
    }
    else {
        status = MTD_RES_PARERR;
    }

    return status;
}

static mtd_res_t mx25v_mtd_ioctl(mtd_dev_t *dev, unsigned char ctrl, void* buf)
{
    mtd_res_t status = MTD_RES_WRPRT;

    mtd_mx25v_dev_t *mtd = (mtd_mx25v_dev_t *) dev;

    unsigned long *ptr = (unsigned long *)buf;
    int res = 0x45;
    uint32_t address = 0;
    uint32_t length = 0;
    uint8_t nb_block_count = 0;

    switch (ctrl) {
    case MTD_CTRL_ERASE_BLOCK: /* force erased a block of blocks */
        /* end sector higher than start sector */
        if (ptr[1] >= ptr[0]) {
            /* erase <start block> to <end block> included */
            length = ptr[1] - ptr[0] + 1;

            nb_block_count = 0;
            while ((nb_block_count < length) && (res >= 0)) {
                address = (ptr[0] + nb_block_count) << SHIFT_BIT_BLOCK;

                res = mx25v_erase_block(&mtd->dev, address);
                nb_block_count++;
            }

            if (res == 0) {
                status = MTD_RES_OK;
            }
            else if (res == -1) {
                status = MTD_RES_ERROR;
            }
            else if (res == -2) {
                status = MTD_RES_PARERR;
            }
            else if (res == -3) {
                status = MTD_RES_NOTRDY;
            }
            else if (res == -4) {
                status = MTD_RES_NOTRDY;
            }
        }
        else {
            status = MTD_RES_PARERR;
        }
        break;

    case MTD_CTRL_POWER:        /* get/set power status */
    case MTD_CTRL_LOCK:         /* lock/unlock media removal */
    case MTD_CTRL_EJECT:        /* eject media */
        status = MTD_RES_OK;
            break;

    case MTD_MMC_GET_TYPE:      /* get card type */
        *(unsigned long*)buf = MX25V_MEMORY_TYPE;
        status = MTD_RES_OK;
        break;

    case MTD_MMC_GET_SDSTAT:    /* get SD status */
    default:                /* parameter not recognized */
        status = MTD_RES_PARERR;
        break;
    }

    return status;
}
