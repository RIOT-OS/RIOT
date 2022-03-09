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
#include <stdlib.h>
#include <string.h>

#include "bitarithm.h"
#include "mtd.h"

int mtd_init(mtd_dev_t *mtd)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    int res = -ENOTSUP;

    if (mtd->driver->init) {
        res = mtd->driver->init(mtd);
    }

#ifdef MODULE_MTD_WRITE_PAGE
    if ((mtd->driver->flags & MTD_DRIVER_FLAG_DIRECT_WRITE) == 0) {
        mtd->work_area = malloc(mtd->pages_per_sector * mtd->page_size);
        if (mtd->work_area == NULL) {
            res = -ENOMEM;
        }
    }
#endif

    return res;
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

#ifdef MODULE_MTD_WRITE_PAGE
/**
 * @brief   Write to a sector on a Memory Technology Device (MTD) by performing a
 *          read-modify-write cycle.
 *
 *          This reads the sector into RAM, modifies it, clears the sector on the
 *          device and writes it back from RAM.
 *
 * @param[in]  mtd      Pointer to the selected device
 * @param[in]  data     Pointer to the data to be written
 * @param[in]  sector   Sector to write
 * @param[in]  offset   Byte offset from the start of the sector
 * @param[in]  size     Number of bytes
 *
 * @return bytes written on success
 * @return < 0 value on error
 */
static size_t _write_sector(mtd_dev_t *mtd, const void *data, uint32_t sector,
                            uint32_t offset, uint32_t len)
{
    int res;
    uint8_t *work = mtd->work_area;
    const uint32_t sector_page = sector * mtd->pages_per_sector;
    const uint32_t sector_size = mtd->pages_per_sector * mtd->page_size;

    if (offset >= sector_size) {
        return len;
    }

    if (offset + len > sector_size) {
        len = sector_size - offset;
    }

    /* copy sector to RAM */
    res = mtd_read_page(mtd, work, sector_page, 0, sector_size);
    if (res < 0) {
        return res;
    }

    /* erase sector */
    res = mtd_erase_sector(mtd, sector, 1);
    if (res < 0) {
        return res;
    }

    /* modify sector in RAM */
    memcpy(work + offset, data, len);

    /* write back modified sector copy */
    res = mtd_write_page_raw(mtd, work, sector_page, 0, sector_size);
    if (res < 0) {
        return res;
    }

    return len;
}

int mtd_write_page(mtd_dev_t *mtd, const void *data, uint32_t page,
                   uint32_t offset, uint32_t len)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->work_area == NULL) {
        return mtd_write_page_raw(mtd, data, page, offset, len);
    }

    uint32_t sector = page / mtd->pages_per_sector;
    const uint32_t sector_page = sector * mtd->pages_per_sector;
    const char *src = data;

    offset += (page - sector_page) * mtd->page_size;

    while (len) {
        int written = _write_sector(mtd, src, sector, offset, len);
        if (written < 0) {
            return written;
        }

        len -= written;
        src += written;
        offset = 0;
        ++sector;
    }

    return 0;
}
#endif

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
