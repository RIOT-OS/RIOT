/*
 * Copyright (C) 2020 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_mtd
 * @{
 *
 * @file
 * @brief       Driver for using lpc23xx-mci via mtd interface
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "diskio.h"
#include "mtd.h"
#include "sdcard_spi.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define min(a, b) ((a) > (b) ? (b) : (a))

/* MCI driver only supports whole page reads / writes */
static uint8_t _page_buffer[SD_HC_BLOCK_SIZE];

static int mtd_mci_init(mtd_dev_t *dev)
{
    if (mci_initialize()) {
        return -EIO;
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        uint32_t block_size;
        mci_ioctl(GET_BLOCK_SIZE, &block_size);
        DEBUG("block size: %lu\n", block_size);
    }

    dev->pages_per_sector = 1;
    dev->page_size = SD_HC_BLOCK_SIZE;
    mci_ioctl(GET_SECTOR_COUNT, &dev->sector_count);

    DEBUG("sector size: %lu\n", dev->page_size);
    DEBUG("sector count: %lu\n", dev->sector_count);
    DEBUG("pages / sector: %lu\n", dev->pages_per_sector);

    return 0;
}

static int mtd_mci_read_page(mtd_dev_t *dev, void *buff, uint32_t page,
                             uint32_t offset, uint32_t size)
{
    (void)dev;
    uint8_t pages = size / SD_HC_BLOCK_SIZE;

    DEBUG("%s(%lu, %lu, %lu)\n", __func__, page, offset, size);

    /* emulate unaligned / sub-page read */
    if (pages == 0 || offset) {
        size = min(SD_HC_BLOCK_SIZE - offset, size);

        if (mci_read(_page_buffer, page, 1)) {
            return -EIO;
        }

        memcpy(buff, _page_buffer + offset, size);

        return size;
    }

    if (mci_read(buff, page, pages)) {
        return -EIO;
    }

    return pages * SD_HC_BLOCK_SIZE;
}

static int mtd_mci_write_page(mtd_dev_t *dev, const void *buff, uint32_t page,
                              uint32_t offset, uint32_t size)
{
    (void)dev;
    uint8_t pages = size / SD_HC_BLOCK_SIZE;

    DEBUG("%s(%lu, %lu, %lu)\n", __func__, page, offset, size);

    /* emulate unaligned / sub-page write */
    if (pages == 0 || offset) {
        size = min(SD_HC_BLOCK_SIZE - offset, size);

        if (mci_read(_page_buffer, page, 1)) {
            return -EIO;
        }

        memcpy(_page_buffer + offset, buff, size);

        buff  = _page_buffer;
        pages = 1;
    }

    if (mci_write(buff, page, pages)) {
        return -EIO;
    }

    return pages * SD_HC_BLOCK_SIZE;
}

static int mtd_mci_erase_sector(mtd_dev_t *dev, uint32_t sector, uint32_t count)
{
    (void)dev;

    while (count--) {
        mci_ioctl(CTRL_ERASE_SECTOR, &sector);
        ++sector;
    }
    return 0;
}

static int mtd_mci_power(mtd_dev_t *dev, enum mtd_power_state power)
{
    (void)dev;

    unsigned char on = power == MTD_POWER_UP ? 1 : 0;
    if (mci_ioctl(CTRL_POWER, &on)) {
        return -EIO;
    }

    return 0;
}

const mtd_desc_t mtd_mci_driver = {
    .init           = mtd_mci_init,
    .read_page      = mtd_mci_read_page,
    .write_page     = mtd_mci_write_page,
    .erase_sector   = mtd_mci_erase_sector,
    .power          = mtd_mci_power,
    .flags          = MTD_DRIVER_FLAG_DIRECT_WRITE,
};
