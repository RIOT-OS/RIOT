/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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

    uint32_t addr_count;

    if (__builtin_add_overflow(addr, count, &addr_count)) {
        /* check for integer overflow */
        return -EOVERFLOW;
    }

    if (addr_count > mtd->size) {
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

    uint32_t page_addr;

    if (__builtin_mul_overflow(page, mtd->base.page_size, &page_addr)) {
        /* check for integer overflow */
        return -EOVERFLOW;
    }

    uint32_t offset_size;

    if (__builtin_add_overflow(offset, size, &offset_size)) {
        /* check for integer overflow */
        return -EOVERFLOW;
    }

    uint32_t page_addr_offset_size;

    if (__builtin_add_overflow(page_addr, offset_size, &page_addr_offset_size)) {
        /* check for integer overflow */
        return -EOVERFLOW;
    }

    if (page_addr_offset_size > mtd->size) {
        /* page addr + offset + size must not exceed the size of memory */
        return -EOVERFLOW;
    }
    memcpy(dest, mtd->memory + page_addr + offset, size);

    return size;
}

int _write_page(mtd_dev_t *dev, const void *src,
                uint32_t page, uint32_t offset, uint32_t size)
{
    mtd_emulated_t *mtd = (mtd_emulated_t *)dev;

    (void)mtd;

    assert(mtd);
    assert(src);

    if (offset >= mtd->base.page_size) {
        /* offset must be smaller than the page size */
        return -EOVERFLOW;
    }

    uint32_t page_addr;

    if (__builtin_mul_overflow(page, mtd->base.page_size, &page_addr)) {
        /* check for integer overflow */
        return -EOVERFLOW;
    }

    uint32_t offset_size;

    if (__builtin_add_overflow(offset, size, &offset_size)) {
        /* check for integer overflow */
        return -EOVERFLOW;
    }

    uint32_t page_addr_offset_size;

    if (__builtin_add_overflow(page_addr, offset_size, &page_addr_offset_size)) {
        /* check for integer overflow */
        return -EOVERFLOW;
    }

    if (page_addr_offset_size > mtd->size) {
        /* page addr + offset + size must not exceed the size of memory */
        return -EOVERFLOW;
    }
    memcpy(mtd->memory + page_addr + offset, src, size);

    return size;
}

static int _erase(mtd_dev_t *dev, uint32_t addr, uint32_t count)
{
    mtd_emulated_t *mtd = (mtd_emulated_t *)dev;

    (void)mtd;
    assert(mtd);

    uint32_t addr_count;

    if (__builtin_add_overflow(addr, count, &addr_count)) {
        /* check for integer overflow */
        return -EOVERFLOW;
    }

    if (addr % (mtd->base.pages_per_sector * mtd->base.page_size) != 0) {
        /* addr must be aligned on a sector boundary */
        return -EOVERFLOW;
    }

    if (count % (mtd->base.pages_per_sector * mtd->base.page_size) != 0) {
        /* count must be a multiple of a sector size. */
        return -EOVERFLOW;
    }

    if (addr_count > mtd->size) {
        /* addr + count must not exceed the size of memory */
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

    uint32_t sector_num;

    if (__builtin_add_overflow(sector, num, &sector_num)) {
        /* check for integer overflow */
        return -EOVERFLOW;
    }

    if (sector >= mtd->base.sector_count) {
        /* sector must not exceed the number of sectors */
        return -EOVERFLOW;
    }

    if (sector_num > mtd->base.sector_count) {
        /* sector + num must not exceed the number of sectors */
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
    .write_page = _write_page,
    .erase = _erase,
    .erase_sector = _erase_sector,
    .power = _power,
};
