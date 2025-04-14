/*
 * Copyright (C) 2017 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_mtd_sdcard
 * @{
 *
 * @file
 * @brief       Driver for using sdcard_spi via mtd interface
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */
#define ENABLE_DEBUG 0
#include "debug.h"
#include "kernel_defines.h"
#include "macros/utils.h"
#include "mtd.h"
#include "mtd_sdcard.h"
#include "sdcard_spi.h"
#include "sdcard_spi_internal.h"

#include <inttypes.h>
#include <errno.h>
#include <string.h>

static int mtd_sdcard_init(mtd_dev_t *dev)
{
    DEBUG("mtd_sdcard_init\n");
    mtd_sdcard_t *mtd_sd = (mtd_sdcard_t*)dev;
    if ((mtd_sd->sd_card->init_done == true) ||
        (sdcard_spi_init(mtd_sd->sd_card, mtd_sd->params) == 0)) {
        /* erasing whole sectors is handled internally by the card so you can
           delete single blocks (i.e. pages) */
        dev->pages_per_sector = 1;
        dev->sector_count     = sdcard_spi_get_sector_count(mtd_sd->sd_card);

        /* sdcard_spi always uses the fixed block size of SD-HC cards */
        dev->page_size        = SD_HC_BLOCK_SIZE;
        dev->write_size       = SD_HC_BLOCK_SIZE;
        return 0;
    }
    return -EIO;
}

static int mtd_sdcard_read_page(mtd_dev_t *dev, void *buff, uint32_t page,
                                uint32_t offset, uint32_t size)
{
    sd_rw_response_t err;
    mtd_sdcard_t *mtd_sd = (mtd_sdcard_t*)dev;

    DEBUG("mtd_sdcard_read_page: page:%" PRIu32 " offset:%" PRIu32 " size:%" PRIu32 "\n",
          page, offset, size);

    if (offset || size % SD_HC_BLOCK_SIZE) {
#if IS_USED(MODULE_MTD_WRITE_PAGE)
        if (dev->work_area == NULL) {
            DEBUG("mtd_sdcard_read_page: no work area\n");
            return -ENOTSUP;
        }

        sdcard_spi_read_blocks(mtd_sd->sd_card, page,
                               dev->work_area, SD_HC_BLOCK_SIZE,
                               1, &err);
        if (err != SD_RW_OK) {
            return -EIO;
        }

        size = MIN(size, SD_HC_BLOCK_SIZE - offset);
        DEBUG("mtd_sdcard_read_page: read %" PRIu32 " bytes at offset %" PRIu32 "\n",
              size, offset);
        memcpy(buff, (uint8_t *)dev->work_area + offset, size);
        return size;
#else
        return -ENOTSUP;
#endif
    }

    sdcard_spi_read_blocks(mtd_sd->sd_card, page,
                           buff, SD_HC_BLOCK_SIZE,
                           size / SD_HC_BLOCK_SIZE, &err);
    if (err != SD_RW_OK) {
        return -EIO;
    }
    return size;
}

static int mtd_sdcard_write_page(mtd_dev_t *dev, const void *buff, uint32_t page,
                                 uint32_t offset, uint32_t size)
{
    sd_rw_response_t err;
    mtd_sdcard_t *mtd_sd = (mtd_sdcard_t*)dev;

    DEBUG("mtd_sdcard_write_page: page:%" PRIu32 " offset:%" PRIu32 " size:%" PRIu32 "\n",
          page, offset, size);

    if (offset || size % SD_HC_BLOCK_SIZE) {
#if IS_USED(MODULE_MTD_WRITE_PAGE)
        if (dev->work_area == NULL) {
            DEBUG("mtd_sdcard_write_page: no work area\n");
            return -ENOTSUP;
        }

        sdcard_spi_read_blocks(mtd_sd->sd_card, page,
                               dev->work_area, SD_HC_BLOCK_SIZE,
                               1, &err);
        if (err != SD_RW_OK) {
            return -EIO;
        }

        size = MIN(size, SD_HC_BLOCK_SIZE - offset);
        DEBUG("mtd_sdcard_write_page: write %" PRIu32 " bytes at offset %" PRIu32 "\n",
              size, offset);
        memcpy((uint8_t *)dev->work_area + offset, buff, size);
        sdcard_spi_write_blocks(mtd_sd->sd_card, page,
                                dev->work_area, SD_HC_BLOCK_SIZE,
                                1, &err);
#else
        return -ENOTSUP;
#endif
    } else {
        sdcard_spi_write_blocks(mtd_sd->sd_card, page,
                                buff, SD_HC_BLOCK_SIZE,
                                size / SD_HC_BLOCK_SIZE, &err);
    }

    if (err != SD_RW_OK) {
        DEBUG("mtd_sdcard_write_page: error %d\n", err);
        return -EIO;
    }
    return size;
}

static int mtd_sdcard_erase_sector(mtd_dev_t *dev, uint32_t sector, uint32_t count)
{
#if IS_ACTIVE(CONFIG_MTD_SDCARD_ERASE) && IS_USED(MODULE_MTD_WRITE_PAGE)
    mtd_sdcard_t *mtd_sd = (mtd_sdcard_t*)dev;

    DEBUG("mtd_sdcard_erase_sector: sector: %" PRIu32 " count: %" PRIu32 "\n",
          sector, count);

    if (dev->work_area == NULL) {
        DEBUG("mtd_sdcard_erase_sector: no work area\n");
        return -ENOTSUP;
    }
    memset(dev->work_area, 0, SD_HC_BLOCK_SIZE);
    while (count) {
        sd_rw_response_t err;
        sdcard_spi_write_blocks(mtd_sd->sd_card, sector,
                                dev->work_area, SD_HC_BLOCK_SIZE,
                                1, &err);
        if (err != SD_RW_OK) {
            DEBUG("mtd_sdcard_erase_sector: error %d\n", err);
            return -EIO;
        }
        --count;
        ++sector;
    }
#else
    (void)dev;
    (void)sector;
    (void)count;
    if (IS_ACTIVE(CONFIG_MTD_SDCARD_ERASE)) {
        return -ENOTSUP;
    }
#endif
    return 0;
}

static int mtd_sdcard_power(mtd_dev_t *dev, enum mtd_power_state power)
{
    (void)dev;
    (void)power;

    /* TODO: implement power down of sdcard in sdcard_spi
    (make use of sdcard_spi_params_t.power pin) */
    return -ENOTSUP; /* currently not supported */
}

static int mtd_sdcard_read(mtd_dev_t *dev, void *buff, uint32_t addr,
                           uint32_t size)
{
    int res = mtd_sdcard_read_page(dev, buff, addr / SD_HC_BLOCK_SIZE,
                                   addr % SD_HC_BLOCK_SIZE, size);
    if (res < 0) {
        return res;
    }
    if (res == (int)size) {
        return 0;
    }
    return -EOVERFLOW;
}

const mtd_desc_t mtd_sdcard_driver = {
    .init = mtd_sdcard_init,
    .read = mtd_sdcard_read,
    .read_page = mtd_sdcard_read_page,
    .write_page = mtd_sdcard_write_page,
    .erase_sector = mtd_sdcard_erase_sector,
    .power = mtd_sdcard_power,
};

#if IS_USED(MODULE_MTD_SDCARD_DEFAULT)
#include "sdcard_spi_params.h"
#include "vfs_default.h"

#define SDCARD_NUMOF ARRAY_SIZE(sdcard_spi_params)

#ifndef CONFIG_SDCARD_GENERIC_MTD_OFFSET
#define CONFIG_SDCARD_GENERIC_MTD_OFFSET 0
#endif

#define MTD_SDCARD_DEV(n, m)                \
    mtd_sdcard_t mtd_sdcard_dev ## n = {    \
        .base = {                           \
           .driver = &mtd_sdcard_driver,    \
        },                                  \
        .sd_card = &sdcard_spi_devs[n],     \
        .params = &sdcard_spi_params[n]     \
    };                                      \
                                            \
    XFA_CONST(mtd_dev_t*, mtd_dev_xfa, m, CONCAT(mtd, m)) = (mtd_dev_t *)&mtd_sdcard_dev ## n

#define MTD_SDCARD_DEV_FS(n, m, filesystem) \
    VFS_AUTO_MOUNT(filesystem, VFS_MTD(mtd_sdcard_dev ## n), VFS_DEFAULT_SD(n), m)

/* this is provided by the sdcard_spi driver see drivers/sdcard_spi/sdcard_spi.c */
extern sdcard_spi_t sdcard_spi_devs[SDCARD_NUMOF];

MTD_SDCARD_DEV(0, CONFIG_SDCARD_GENERIC_MTD_OFFSET);
#ifdef MODULE_FATFS_VFS
MTD_SDCARD_DEV_FS(0, CONFIG_SDCARD_GENERIC_MTD_OFFSET, fatfs);
#endif

#endif
