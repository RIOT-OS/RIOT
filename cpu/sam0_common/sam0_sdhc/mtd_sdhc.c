/*
 * Copyright (C) 2022 Benjamin Valentin
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
 * @brief       Driver for using sam0 SDHC controller via mtd interface
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "mtd_sam0_sdhc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define min(a, b) ((a) > (b) ? (b) : (a))

static int _init(mtd_dev_t *dev)
{
    mtd_sam0_sdhc_t *ctx = container_of(dev, mtd_sam0_sdhc_t, base);

    if (sdhc_init(&ctx->state)) {
        DEBUG("SDHC init failed\n");
        return -EIO;
    }

    dev->pages_per_sector = 1;
    dev->page_size = SD_MMC_BLOCK_SIZE;
    dev->sector_count = ctx->state.sectors;
    dev->write_size = SD_MMC_BLOCK_SIZE;

#if IS_USED(MODULE_MTD_WRITE_PAGE)
    /* TODO: move to MTD layer */
    dev->work_area = malloc(SD_MMC_BLOCK_SIZE);
    if (dev->work_area == NULL) {
        return -ENOMEM;
    }
    dev->write_size = 1;
#endif

    DEBUG("sector size: %lu\n", dev->page_size);
    DEBUG("sector count: %lu\n", dev->sector_count);
    DEBUG("pages / sector: %lu\n", dev->pages_per_sector);

    return 0;
}

static int _read_page(mtd_dev_t *dev, void *buff, uint32_t page,
                      uint32_t offset, uint32_t size)
{
    mtd_sam0_sdhc_t *ctx = container_of(dev, mtd_sam0_sdhc_t, base);
    uint16_t pages = size / SD_MMC_BLOCK_SIZE;

    DEBUG("%s(%lu, %lu, %lu)\n", __func__, page, offset, size);

    /* emulate unaligned / sub-page read */
    if (pages == 0 || offset || ((uintptr_t)buff & 0x3)) {
#if IS_USED(MODULE_MTD_WRITE_PAGE)
        if (dev->work_area == NULL) {
            DEBUG("mtd_sdhc: no work area\n");
            return -ENOTSUP;
        }

        size = min(SD_MMC_BLOCK_SIZE - offset, size);

        if (sdhc_read_blocks(&ctx->state, page, dev->work_area, 1)) {
            return -EIO;
        }

        memcpy(buff, dev->work_area + offset, size);
        return size;
#else
        return -ENOTSUP;
#endif
    }

    if (sdhc_read_blocks(&ctx->state, page, buff, pages)) {
        return -EIO;
    }

    return pages * SD_MMC_BLOCK_SIZE;
}

static int _write_page(mtd_dev_t *dev, const void *buff, uint32_t page,
                              uint32_t offset, uint32_t size)
{
    mtd_sam0_sdhc_t *ctx = container_of(dev, mtd_sam0_sdhc_t, base);
    uint16_t pages = size / SD_MMC_BLOCK_SIZE;

    DEBUG("%s(%lu, %lu, %lu)\n", __func__, page, offset, size);

    /* emulate unaligned / sub-page write */
    if (pages == 0 || offset || ((uintptr_t)buff & 0x3)) {
#if IS_USED(MODULE_MTD_WRITE_PAGE)
        if (dev->work_area == NULL) {
            DEBUG("mtd_sdhc: no work area\n");
            return -ENOTSUP;
        }

        size = min(SD_MMC_BLOCK_SIZE - offset, size);

        if (sdhc_read_blocks(&ctx->state, page, dev->work_area, 1)) {
            return -EIO;
        }

        memcpy(dev->work_area + offset, buff, size);

        buff  = dev->work_area;
        pages = 1;
#else
        return -ENOTSUP;
#endif
    }

    if (sdhc_write_blocks(&ctx->state, page, buff, pages)) {
        return -EIO;
    }

    return min(size, pages * SD_MMC_BLOCK_SIZE);
}

static int _erase_sector(mtd_dev_t *dev, uint32_t sector, uint32_t count)
{
    mtd_sam0_sdhc_t *ctx = container_of(dev, mtd_sam0_sdhc_t, base);

    if (sdhc_erase_blocks(&ctx->state, sector, count)) {
        return -EIO;
    }

    return 0;
}

const mtd_desc_t mtd_sam0_sdhc_driver = {
    .init           = _init,
    .read_page      = _read_page,
    .write_page     = _write_page,
    .erase_sector   = _erase_sector,
    .flags          = MTD_DRIVER_FLAG_DIRECT_WRITE,
};
