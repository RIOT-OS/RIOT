/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <errno.h>
#include <string.h>

#include "assert.h"
#include "macros/utils.h"
#include "mtd_emulated.h"

static int _init(mtd_dev_t *dev)
{
    mtd_emulated_t *mtd = (mtd_emulated_t *)dev;
    assert(mtd);

    if (!mtd->init_done) {
        memset(mtd->memory, 0xff, mtd->size);
        mtd->init_done = true;
    }
    return 0;
}

static int _read(mtd_dev_t *dev, void *dest, uint32_t addr, uint32_t count)
{
    mtd_emulated_t *mtd = (mtd_emulated_t *)dev;

    assert(mtd);
    assert(dest);

    if ((addr + count) > mtd->size) {
        /* addr + count must not exceed the size of memory */
        return -EOVERFLOW;
    }
    memcpy(dest, mtd->memory + addr, count);

    return 0;
}

static int _read_page(mtd_dev_t *dev, void *dest,
                      uint32_t page, uint32_t offset, uint32_t size)
{
    mtd_emulated_t *mtd = (mtd_emulated_t *)dev;

    (void)mtd;

    assert(mtd);
    assert(dest);

    if (((page * mtd->base.page_size) + offset + size) > mtd->size) {
        /* page addr + offset + size must not exceed the size of memory */
        return -EOVERFLOW;
    }
    memcpy(dest, mtd->memory + (page * mtd->base.page_size) + offset, size);

    return size;
}

static int _write(mtd_dev_t *dev, const void *src, uint32_t addr, uint32_t count)
{
    mtd_emulated_t *mtd = (mtd_emulated_t *)dev;

    (void)mtd;

    assert(mtd);
    assert(src);

    if (/* addr + count must be inside a page boundary. */
        (((addr % mtd->base.page_size) + count) > mtd->base.page_size) ||
        /* addr + count must not exceed the size of memory */
        ((addr + count) > mtd->size)) {
        return -EOVERFLOW;
    }
    memcpy(mtd->memory + addr, src, count);

    return 0;
}

int _write_page(mtd_dev_t *dev, const void *src,
                uint32_t page, uint32_t offset, uint32_t size)
{
    mtd_emulated_t *mtd = (mtd_emulated_t *)dev;

    (void)mtd;

    assert(mtd);
    assert(src);

    if (/* offset must be smaller than the page size */
        (offset >= mtd->base.page_size) ||
        /* page addr + offset + size must not exceed the size of memory */
        ((page * mtd->base.page_size) + offset + size) > mtd->size) {
        return -EOVERFLOW;
    }
    memcpy(mtd->memory + (page * mtd->base.page_size) + offset, src, size);

    return size;
}

static int _erase(mtd_dev_t *dev, uint32_t addr, uint32_t count)
{
    mtd_emulated_t *mtd = (mtd_emulated_t *)dev;

    (void)mtd;
    assert(mtd);

    if (/* addr must be aligned on a sector boundary */
        (addr % (mtd->base.pages_per_sector * mtd->base.page_size) != 0) ||
        /* count must be a multiple of a sector size. */
        (count % (mtd->base.pages_per_sector * mtd->base.page_size) != 0) ||
        /* addr + count must not exceed the size of memory */
        ((addr + count) > mtd->size)) {
        return -EOVERFLOW;
    }

    memset(mtd->memory + addr, 0xff, count);

    return 0;
}

static int _erase_sector(mtd_dev_t *dev, uint32_t sector, uint32_t num)
{
    mtd_emulated_t *mtd = (mtd_emulated_t *)dev;

    (void)mtd;
    assert(mtd);

    if (/* sector must not exceed the number of sectors */
        (sector >= mtd->base.sector_count) ||
        /* sector + num must not exceed the number of sectors */
        ((sector + num) > mtd->base.sector_count)) {
        return -EOVERFLOW;
    }

    memset(mtd->memory + (sector * (mtd->base.pages_per_sector * mtd->base.page_size)),
           0xff, num * (mtd->base.pages_per_sector * mtd->base.page_size));

    return 0;
}

static int _power(mtd_dev_t *dev, enum mtd_power_state power)
{
    (void)dev;
    (void)power;
    return 0;
}

const mtd_desc_t _mtd_emulated_driver = {
    .init = _init,
    .read = _read,
    .read_page = _read_page,
    .write = _write,
    .write_page = _write_page,
    .erase = _erase,
    .erase_sector = _erase_sector,
    .power = _power,
};
