/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mtd_at25xxx
 * @{
 *
 * @file
 * @brief       MTD wrapper for AT25XXX based SPI EEPROMs (like AT25xxx, M95xxx, 25AAxxx, 25LCxxx,
 *              CAT25xxx & BR25Sxxx)
 *
 * @author      Johannes Koster <johannes.koster@ml-pa.com>
 *
 * @}
 */
#define ENABLE_DEBUG 0
#include "debug.h"
#include "mtd.h"
#include "at25xxx/mtd.h"
#include "at25xxx.h"
#include "at25xxx_params.h"

#include <inttypes.h>
#include <errno.h>

static int mtd_at25xxx_init(mtd_dev_t *dev)
{
    DEBUG("[mtd_at25xxx] initializing\n");
    mtd_at25xxx_t *mtd_at25xxx = (mtd_at25xxx_t*)dev;
    if (at25xxx_init(mtd_at25xxx->at25xxx_eeprom, mtd_at25xxx->params) == 0)
    {
        dev->pages_per_sector = 1;
        dev->page_size        = mtd_at25xxx->params->page_size;
        dev->sector_count     = mtd_at25xxx->params->size / mtd_at25xxx->params->page_size;
        dev->write_size       = 1;
        return 0;
    }
    return -EIO;
}

static int mtd_at25xxx_read(mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size)
{
    DEBUG("[mtd_at25xxx] read: addr:%" PRIu32 " size:%" PRIu32 "\n", addr, size);
    mtd_at25xxx_t *mtd_at25xxx_ = (mtd_at25xxx_t*)dev;
    return at25xxx_read(mtd_at25xxx_->at25xxx_eeprom, addr, buff, size);
}

static int mtd_at25xxx_write_page(mtd_dev_t *dev, const void *src, uint32_t page, uint32_t offset,
                                  uint32_t size)
{
    DEBUG("[mtd_at25xxx] write_page: page:%" PRIu32 " offset:%" PRIu32 " size:%" PRIu32 "\n",
          page, offset, size);
    mtd_at25xxx_t *mtd_at25xxx_ = (mtd_at25xxx_t*)dev;
    return at25xxx_write_page(mtd_at25xxx_->at25xxx_eeprom, page, offset, src, size);
}

static int mtd_at25xxx_erase(mtd_dev_t *dev, uint32_t addr, uint32_t size)
{
    DEBUG("[mtd_at25xxx] mtd_at25xxx_erase: addr:%" PRIu32 " size:%" PRIu32 "\n", addr, size);
    mtd_at25xxx_t *mtd_at25xxx_ = (mtd_at25xxx_t*)dev;
    return at25xxx_clear(mtd_at25xxx_->at25xxx_eeprom, addr, size);
}

static int mtd_at25xxx_power(mtd_dev_t *dev, enum mtd_power_state power)
{
    (void)dev;
    (void)power;

    /* TODO: implement power down/up of EEPROM (at25xxx driver?)
    */
    return -ENOTSUP; /* currently not supported */
}

const mtd_desc_t mtd_at25xxx_driver = {
    .init = mtd_at25xxx_init,
    .read = mtd_at25xxx_read,
    .write_page = mtd_at25xxx_write_page,
    .erase = mtd_at25xxx_erase,
    .power = mtd_at25xxx_power,
    .flags = MTD_DRIVER_FLAG_DIRECT_WRITE,
};
