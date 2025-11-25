/*
 * Copyright (C) 2022 Juergen Fitschen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @ingroup  tests
 * @brief    Fake MTD device
 *
 * @author   Juergen Fitschen <me@jue.yt>
 */

#include <string.h>
#include <errno.h>
#include "fakemtd.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static inline uint32_t sector2offset(fake_mtd_t *ctx, uint32_t sector)
{
    return sector * ctx->mtd.page_size * ctx->mtd.pages_per_sector;
}

static inline uint32_t page2offset(fake_mtd_t *ctx, uint32_t page)
{
    return page * ctx->mtd.page_size;
}

static inline bool within_bounds(fake_mtd_t *ctx, uint32_t offset, uint32_t count)
{
    return (offset + count) <= sizeof(ctx->buffer);
}

static int _init(mtd_dev_t *dev)
{
    (void) dev;

    return 0;
}

static int _read_page(mtd_dev_t *dev, void *buff, uint32_t page, uint32_t offset, uint32_t size)
{
    fake_mtd_t *ctx = container_of(dev, fake_mtd_t, mtd);

    DEBUG("fake_mtd: _read_page(%"PRIu32", %"PRIu32", %"PRIu32")\n", page, offset, size);

    offset += page2offset(ctx, page);
    if (!within_bounds(ctx, offset, size)) {
        return -EOVERFLOW;
    }

    memcpy(buff, &ctx->buffer[offset], size);

    return size;
}

static int _write_page(mtd_dev_t *dev, const void *buff, uint32_t page, uint32_t offset, uint32_t size)
{
    fake_mtd_t *ctx = container_of(dev, fake_mtd_t, mtd);

    DEBUG("fake_mtd: _write_page(%"PRIu32", %"PRIu32", %"PRIu32")\n", page, offset, size);

    offset += page2offset(ctx, page);
    if (!within_bounds(ctx, offset, size)) {
        return -EOVERFLOW;
    }

    memcpy(&ctx->buffer[offset], buff, size);

    return size;
}

static int _erase_sector(mtd_dev_t *dev, uint32_t sector, uint32_t count)
{
    fake_mtd_t *ctx = container_of(dev, fake_mtd_t, mtd);

    DEBUG("fake_mtd: _erase_sector(%"PRIu32", %"PRIu32")\n", sector, count);

    uint32_t offset = sector2offset(ctx, sector);
    uint32_t size = sector2offset(ctx, count);
    if (!within_bounds(ctx, offset, count)) {
        return -EOVERFLOW;
    }

    memset(&ctx->buffer[offset], 0xff, size);

    return 0;
}

const mtd_desc_t fake_mtd_driver = {
    .init = _init,
    .read_page = _read_page,
    .write_page = _write_page,
    .erase_sector = _erase_sector,
};

/** @} */
