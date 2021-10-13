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
#include "mtd.h"
#include "mtd_sdcard.h"
#include "sdcard_spi.h"
#include "sdcard_spi_internal.h"
#include "kernel_defines.h"

#include <inttypes.h>
#include <errno.h>

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
        return 0;
    }
    return -EIO;
}

static int mtd_sdcard_read(mtd_dev_t *dev, void *buff, uint32_t addr,
                           uint32_t size)
{
    DEBUG("mtd_sdcard_read: addr:%" PRIu32 " size:%" PRIu32 "\n", addr, size);
    mtd_sdcard_t *mtd_sd = (mtd_sdcard_t*)dev;
    sd_rw_response_t err;
    sdcard_spi_read_blocks(mtd_sd->sd_card, addr / SD_HC_BLOCK_SIZE,
                           buff, SD_HC_BLOCK_SIZE,
                           size / SD_HC_BLOCK_SIZE, &err);

    if (err == SD_RW_OK) {
        return 0;
    }
    return -EIO;
}

static int mtd_sdcard_read_page(mtd_dev_t *dev, void *buff, uint32_t page,
                                uint32_t offset, uint32_t size)
{
    DEBUG("mtd_sdcard_read_page: page:%" PRIu32 " offset:%" PRIu32 " size:%" PRIu32 "\n",
          page, offset, size);

    if (offset) {
        return -ENOTSUP;
    }

    mtd_sdcard_t *mtd_sd = (mtd_sdcard_t*)dev;
    sd_rw_response_t err;
    int res = sdcard_spi_read_blocks(mtd_sd->sd_card, page,
                                     buff, SD_HC_BLOCK_SIZE,
                                     size / SD_HC_BLOCK_SIZE, &err);

    if (err != SD_RW_OK) {
        return -EIO;
    }
    return res * SD_HC_BLOCK_SIZE;
}

static int mtd_sdcard_write(mtd_dev_t *dev, const void *buff, uint32_t addr,
                            uint32_t size)
{
    DEBUG("mtd_sdcard_write: addr:%" PRIu32 " size:%" PRIu32 "\n", addr, size);
    mtd_sdcard_t *mtd_sd = (mtd_sdcard_t*)dev;
    sd_rw_response_t err;
    sdcard_spi_write_blocks(mtd_sd->sd_card, addr / SD_HC_BLOCK_SIZE,
                            buff, SD_HC_BLOCK_SIZE,
                            size / SD_HC_BLOCK_SIZE, &err);

    if (err == SD_RW_OK) {
        return 0;
    }
    return -EIO;
}

static int mtd_sdcard_write_page(mtd_dev_t *dev, const void *buff, uint32_t page,
                                 uint32_t offset, uint32_t size)
{
    DEBUG("mtd_sdcard_write_page: page:%" PRIu32 " offset:%" PRIu32 " size:%" PRIu32 "\n",
          page, offset, size);

    if (offset) {
        return -ENOTSUP;
    }

    mtd_sdcard_t *mtd_sd = (mtd_sdcard_t*)dev;
    sd_rw_response_t err;
    int res = sdcard_spi_write_blocks(mtd_sd->sd_card, page,
                                     buff, SD_HC_BLOCK_SIZE,
                                     size / SD_HC_BLOCK_SIZE, &err);

    if (err != SD_RW_OK) {
        return -EIO;
    }
    return res * SD_HC_BLOCK_SIZE;
}

static int mtd_sdcard_erase(mtd_dev_t *dev,
                            uint32_t addr,
                            uint32_t size)
{
    DEBUG("mtd_sdcard_erase: addr:%" PRIu32 " size:%" PRIu32 "\n", addr, size);
    (void)dev;
    (void)addr;
    (void)size;

    if (!IS_ACTIVE(CONFIG_MTD_SDCARD_ERASE)) {
        return 0;
    }
    else {
        return -ENOTSUP; /* explicit erase currently not supported */
    }
}

static int mtd_sdcard_power(mtd_dev_t *dev, enum mtd_power_state power)
{
    (void)dev;
    (void)power;

    /* TODO: implement power down of sdcard in sdcard_spi
    (make use of sdcard_spi_params_t.power pin) */
    return -ENOTSUP; /* currently not supported */
}

const mtd_desc_t mtd_sdcard_driver = {
    .init = mtd_sdcard_init,
    .read = mtd_sdcard_read,
    .read_page = mtd_sdcard_read_page,
    .write = mtd_sdcard_write,
    .write_page = mtd_sdcard_write_page,
    .erase = mtd_sdcard_erase,
    .power = mtd_sdcard_power,
    .flags = MTD_DRIVER_FLAG_DIRECT_WRITE,
};
