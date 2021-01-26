/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mtd
 * @{
 * @brief       Low level Memory Technology Device interface
 *
 * Generic memory technology device interface
 *
 * @file
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>

#include "bitarithm.h"
#include "mtd.h"

int mtd_init(mtd_dev_t *mtd)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->init) {
        return mtd->driver->init(mtd);
    }
    else {
        return -ENOTSUP;
    }
}

int mtd_read(mtd_dev_t *mtd, void *dest, uint32_t addr, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->read) {
        return mtd->driver->read(mtd, dest, addr, count);
    }

    /* page size is always a power of two */
    const uint32_t page_shift = bitarithm_msb(mtd->page_size);
    const uint32_t page_mask = mtd->page_size - 1;

    return mtd_read_page(mtd, dest, addr >> page_shift, addr & page_mask, count);
}

int mtd_read_page(mtd_dev_t *mtd, void *dest, uint32_t page, uint32_t offset,
                  uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->read_page == NULL) {
        /* TODO: remove when all backends implement read_page */
        if (mtd->driver->read) {
            return mtd->driver->read(mtd, dest, mtd->page_size * page + offset, count);
        } else {
            return -ENOTSUP;
        }
    }

    /* Implementation assumes page size is <= INT_MAX and a power of two. */
    /* We didn't find hardware yet where this is not true.                */
    assert(mtd->page_size <= INT_MAX);
    assert(bitarithm_bits_set(mtd->page_size) == 1);

    /* page size is always a power of two */
    const uint32_t page_shift = bitarithm_msb(mtd->page_size);
    const uint32_t page_mask = mtd->page_size - 1;

    page  += offset >> page_shift;
    offset = offset & page_mask;

    char *_dst = dest;

    while (count) {
        int read_bytes = mtd->driver->read_page(mtd, _dst, page, offset, count);

        if (read_bytes < 0) {
            return read_bytes;
        }

        count -= read_bytes;

        if (count == 0) {
            break;
        }

        _dst   += read_bytes;
        page   += (offset + read_bytes) >> page_shift;
        offset  = (offset + read_bytes) & page_mask;
    }

    return 0;
}

int mtd_write(mtd_dev_t *mtd, const void *src, uint32_t addr, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->write) {
        return mtd->driver->write(mtd, src, addr, count);
    }

    /* page size is always a power of two */
    const uint32_t page_shift = bitarithm_msb(mtd->page_size);
    const uint32_t page_mask = mtd->page_size - 1;

    return mtd_write_page_raw(mtd, src, addr >> page_shift, addr & page_mask, count);
}

int mtd_write_page_raw(mtd_dev_t *mtd, const void *src, uint32_t page, uint32_t offset,
                       uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->write_page == NULL) {
        /* TODO: remove when all backends implement write_page */
        if (mtd->driver->write) {
            return mtd->driver->write(mtd, src, mtd->page_size * page + offset, count);
        } else {
            return -ENOTSUP;
        }
    }

    /* Implementation assumes page size is <= INT_MAX and a power of two. */
    /* We didn't find hardware yet where this is not true.                */
    assert(mtd->page_size <= INT_MAX);
    assert(bitarithm_bits_set(mtd->page_size) == 1);

    /* page size is always a power of two */
    const uint32_t page_shift = bitarithm_msb(mtd->page_size);
    const uint32_t page_mask = mtd->page_size - 1;

    page  += offset >> page_shift;
    offset = offset & page_mask;

    const char *_src = src;

    while (count) {
        int written = mtd->driver->write_page(mtd, _src, page, offset, count);

        if (written < 0) {
            return written;
        }

        count -= written;

        if (count == 0) {
            break;
        }

        _src   += written;
        page   += (offset + written) >> page_shift;
        offset  = (offset + written) & page_mask;
    }

    return 0;
}

int mtd_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->erase) {
        return mtd->driver->erase(mtd, addr, count);
    }

    uint32_t sector_size = mtd->pages_per_sector * mtd->page_size;

    if (count % sector_size) {
        return -EOVERFLOW;
    }

    if (addr % sector_size) {
        return -EOVERFLOW;
    }

    return mtd_erase_sector(mtd, addr / sector_size, count / sector_size);
}

int mtd_erase_sector(mtd_dev_t *mtd, uint32_t sector, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (sector >= mtd->sector_count) {
        return -EOVERFLOW;
    }

    if (mtd->driver->erase_sector == NULL) {
        /* TODO: remove when all backends implement erase_sector */
        if (mtd->driver->erase) {
            uint32_t sector_size = mtd->pages_per_sector * mtd->page_size;
            return mtd->driver->erase(mtd,
                                      sector * sector_size,
                                      count * sector_size);
        } else {
            return -ENOTSUP;
        }
    }

    return mtd->driver->erase_sector(mtd, sector, count);
}

int mtd_power(mtd_dev_t *mtd, enum mtd_power_state power)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->power) {
        return mtd->driver->power(mtd, power);
    }
    else {
        return -ENOTSUP;
    }
}

/** @} */
