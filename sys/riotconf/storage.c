/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riotconf_storage
 * @{
 *
 * @file
 * @brief       RIOT configuration storage
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "macros/utils.h"
#include "mtd.h"
#include "riotconf/slot.h"
#include "riotconf/storage.h"

riotconf_storage_t _riotconf_slots[RIOTCONF_SLOT_NUMOF];

riotconf_storage_t *riotconf_storage_get(riotconf_slot_t slot)
{
    assert(slot < RIOTCONF_SLOT_NUMOF);
    return &_riotconf_slots[slot];
}

void riotconf_storage_init(riotconf_slot_t slot, mtd_dev_t *dev, uint32_t sector_offset, void *sector_buf, size_t sector_size)
{
    assert(slot < RIOTCONF_SLOT_NUMOF);
    assert(dev);
    assert(sector_buf);
    assert(sector_size);
    assert(sector_size >= dev->page_size * dev->pages_per_sector);
    assert(sector_size % (dev->page_size * dev->pages_per_sector) == 0);
    riotconf_storage_t storage = {
        .dev = dev,
        .sector_offset = sector_offset,
        .sector_buf = sector_buf,
        .sector_size = sector_size
    };
    _riotconf_slots[slot] = storage;
}

void riotconf_storage_start_read(riotconf_storage_t *dev, void **buf, size_t *size)
{
    *buf = dev->sector_buf;
    *size = dev->sector_size;
}

int riotconf_storage_read(riotconf_storage_t *dev, void *buf, size_t offset, size_t size)
{
    uint32_t page = dev->sector_offset * dev->dev->pages_per_sector;
    return mtd_read_page(dev->dev, buf, page, offset, size);
}

void riotconf_storage_finish_read(riotconf_storage_t *dev)
{
    /* nothing to do */
    (void)dev;
}

int riotconf_storage_erase_sector(riotconf_storage_t *dev, uint32_t sector, uint32_t count)
{
    uint32_t sec = dev->sector_offset + sector;
    return mtd_erase_sector(dev->dev, sec, count);
}

int riotconf_storage_start_write(riotconf_storage_t *dev)
{
    int rd = riotconf_storage_read(dev, dev->sector_buf, 0, dev->sector_size);
    if (rd < 0) {
        return rd;
    }
    return riotconf_storage_erase_sector(dev, 0, 1);
}

int riotconf_storage_write(riotconf_storage_t *dev, const void *data, size_t offset, size_t size)
{
    if (offset < dev->sector_size) {
        size_t l = MIN(size, dev->sector_size - offset);
        memcpy(((uint8_t *)dev->sector_buf) + offset, data, l);
        size -= l;
        data = (const uint8_t *)data + l;
    }
    uint32_t page = dev->sector_offset * dev->dev->pages_per_sector;
    return mtd_write_page(dev->dev, data, page, offset, size);
}

void riotconf_storage_finish_write(riotconf_storage_t *dev, const riotconf_hdr_t *hdr)
{
    riotconf_hdr_t *h = (riotconf_hdr_t *)dev->sector_buf;
    memcpy(h, hdr, sizeof(*hdr));
    uint32_t page = dev->sector_offset * dev->dev->pages_per_sector;
    /* sector has been erased before */
    mtd_write_page_raw(dev->dev, dev->sector_buf, page, 0, dev->sector_size);
}
