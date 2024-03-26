/*
 * Copyright (C) 2017 HAW-Hamburg
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_mtd_sdmmc
 * @{
 *
 * @file
 * @brief       Driver for using sdmmc via mtd interface
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <errno.h>
#include <inttypes.h>
#include <string.h>

#define ENABLE_DEBUG 0
#include "debug.h"
#include "kernel_defines.h"
#include "macros/utils.h"
#include "mtd.h"
#include "mtd_sdmmc.h"
#include "sdmmc/sdmmc.h"

static int mtd_sdmmc_init(mtd_dev_t *dev)
{
    DEBUG("mtd_sdmmc_init\n");
    mtd_sdmmc_t *mtd_sd = (mtd_sdmmc_t*)dev;

    /* get the SDMMC device descriptor from SDMMC peripheral index */
    mtd_sd->sdmmc = sdmmc_get_dev(mtd_sd->sdmmc_idx);

    if ((mtd_sd->sdmmc->init_done == true) ||
        (sdmmc_card_init(mtd_sd->sdmmc) == 0)) {
        /* erasing whole sectors is handled internally by the card so you can
           delete single blocks (i.e. pages) */
        dev->pages_per_sector = 1;
        dev->sector_count     = (uint32_t)(sdmmc_get_capacity(mtd_sd->sdmmc) /
                                           SDMMC_SDHC_BLOCK_SIZE);

        /* sdcard uses the fixed block size of SD-HC cards */
        dev->page_size        = SDMMC_SDHC_BLOCK_SIZE;
        dev->write_size       = SDMMC_SDHC_BLOCK_SIZE;
        return 0;
    }

    return -EIO;
}

static int mtd_sdmmc_read_page(mtd_dev_t *dev, void *buff, uint32_t page,
                                uint32_t offset, uint32_t size)
{
    mtd_sdmmc_t *mtd_sd = (mtd_sdmmc_t*)dev;

    DEBUG("mtd_sdmmc_read_page: page:%" PRIu32 " offset:%" PRIu32 " size:%" PRIu32 "\n",
          page, offset, size);

    if (offset || size % SDMMC_SDHC_BLOCK_SIZE) {
#if IS_USED(MODULE_MTD_WRITE_PAGE)
        if (dev->work_area == NULL) {
            DEBUG("mtd_sdmmc_read_page: no work area\n");
            return -ENOTSUP;
        }

        if (sdmmc_read_blocks(mtd_sd->sdmmc, page, SDMMC_SDHC_BLOCK_SIZE,
                              1, dev->work_area, NULL)) {
            return -EIO;
        }
        size = MIN(size, SDMMC_SDHC_BLOCK_SIZE - offset);
        DEBUG("mtd_sdmmc_read_page: read %" PRIu32 " bytes at offset %" PRIu32 "\n",
              size, offset);
        memcpy(buff, (uint8_t *)dev->work_area + offset, size);
        return size;
#else
        return -ENOTSUP;
#endif
    }

    int err = sdmmc_read_blocks(mtd_sd->sdmmc, page, SDMMC_SDHC_BLOCK_SIZE,
                                size / SDMMC_SDHC_BLOCK_SIZE, buff, NULL);
    if (err) {
        DEBUG("mtd_sdmmc_read_page: error %d\n", err);
        return -EIO;
    }
    return size;
}

static int mtd_sdmmc_write_page(mtd_dev_t *dev, const void *buff, uint32_t page,
                                 uint32_t offset, uint32_t size)
{
    mtd_sdmmc_t *mtd_sd = (mtd_sdmmc_t*)dev;

    DEBUG("mtd_sdmmc_write_page: page:%" PRIu32 " offset:%" PRIu32 " size:%" PRIu32 "\n",
          page, offset, size);

    if (offset || size % SDMMC_SDHC_BLOCK_SIZE) {
#if IS_USED(MODULE_MTD_WRITE_PAGE)
        if (dev->work_area == NULL) {
            DEBUG("mtd_sdmmc_write_page: no work area\n");
            return -ENOTSUP;
        }

        if (sdmmc_read_blocks(mtd_sd->sdmmc, page, SDMMC_SDHC_BLOCK_SIZE,
                              1, dev->work_area, NULL)) {
            return -EIO;
        }

        size = MIN(size, SDMMC_SDHC_BLOCK_SIZE - offset);
        DEBUG("mtd_sdmmc_write_page: write %" PRIu32 " bytes at offset %" PRIu32 "\n",
              size, offset);
        memcpy((uint8_t *)dev->work_area + offset, buff, size);
        if (sdmmc_write_blocks(mtd_sd->sdmmc, page, SDMMC_SDHC_BLOCK_SIZE,
                               1, dev->work_area, NULL)) {
            return -EIO;
        }
#else
        return -ENOTSUP;
#endif
    }
    else {
        int err = sdmmc_write_blocks(mtd_sd->sdmmc, page, SDMMC_SDHC_BLOCK_SIZE,
                                     size / SDMMC_SDHC_BLOCK_SIZE, buff, NULL);
        if (err) {
            DEBUG("mtd_sdmmc_write_page: error %d\n", err);
            return -EIO;
        }
    }
    return size;
}

static int mtd_sdmmc_erase_sector(mtd_dev_t *dev, uint32_t sector, uint32_t count)
{
    mtd_sdmmc_t *mtd_sd = (mtd_sdmmc_t*)dev;
    return sdmmc_erase_blocks(mtd_sd->sdmmc, sector, count);
}

static int mtd_sdmmc_power(mtd_dev_t *dev, enum mtd_power_state power)
{
    (void)dev;
    (void)power;

    /* TODO: implement power down of sdcard in sdcard_spi
    (make use of sdcard_spi_params_t.power pin) */
    return -ENOTSUP; /* currently not supported */
}

static int mtd_sdmmc_read(mtd_dev_t *dev, void *buff, uint32_t addr,
                           uint32_t size)
{
    int res = mtd_sdmmc_read_page(dev, buff, addr / SDMMC_SDHC_BLOCK_SIZE,
                                   addr % SDMMC_SDHC_BLOCK_SIZE, size);
    if (res < 0) {
        return res;
    }
    if (res == (int)size) {
        return 0;
    }
    return -EOVERFLOW;
}

const mtd_desc_t mtd_sdmmc_driver = {
    .init = mtd_sdmmc_init,
    .read = mtd_sdmmc_read,
    .read_page = mtd_sdmmc_read_page,
    .write_page = mtd_sdmmc_write_page,
    .erase_sector = mtd_sdmmc_erase_sector,
    .power = mtd_sdmmc_power,
    .flags = MTD_DRIVER_FLAG_DIRECT_WRITE,
};

#if IS_USED(MODULE_MTD_SDMMC_DEFAULT)
#include "vfs_default.h"

#ifndef CONFIG_SDMMC_GENERIC_MTD_OFFSET
#define CONFIG_SDMMC_GENERIC_MTD_OFFSET 0
#endif

#define MTD_SDMMC_DEV(n, m)                 \
    mtd_sdmmc_t mtd_sdmmc_dev ##n = {       \
        .base = {                           \
           .driver = &mtd_sdmmc_driver,     \
        },                                  \
        .sdmmc_idx = n,                     \
    };                                      \
                                            \
    XFA_CONST(mtd_dev_xfa, m) mtd_dev_t CONCAT(*mtd, m) = (mtd_dev_t *)&mtd_sdmmc_dev ## n

#if IS_USED(MODULE_MTD_SDCARD_DEFAULT)
/* we use /sd1 as default mount point for coexistence with mtd_sdcard */
#define MTD_SDMMC_DEV_FS(n, m, filesystem) \
    VFS_AUTO_MOUNT(filesystem, VFS_MTD(mtd_sdmmc_dev ##n), VFS_DEFAULT_SD(1), m)
#else
#define MTD_SDMMC_DEV_FS(n, m, filesystem) \
    VFS_AUTO_MOUNT(filesystem, VFS_MTD(mtd_sdmmc_dev ##n), VFS_DEFAULT_SD(n), m)
#endif

MTD_SDMMC_DEV(0, CONFIG_SDMMC_GENERIC_MTD_OFFSET);
#ifdef MODULE_FATFS_VFS
MTD_SDMMC_DEV_FS(0, CONFIG_SDMMC_GENERIC_MTD_OFFSET, fatfs);
#endif
#ifdef MODULE_LWEXT4
MTD_SDMMC_DEV_FS(0, CONFIG_SDMMC_GENERIC_MTD_OFFSET, lwext4);
#endif

#endif
