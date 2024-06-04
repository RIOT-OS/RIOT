/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup pkg_flashdb
 * @{
 *
 * @file
 * @brief   MTD <-> FAL adapter
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @]
 */

#include "macros/utils.h"
#include "mtd.h"

#include <string.h>
#include <limits.h>
#include <fal.h>

static mtd_dev_t *_mtd;

static int _read(long offset, uint8_t *buf, size_t size)
{
    int res = mtd_read_page(_mtd, buf,
                            offset / _mtd->page_size,
                            offset % _mtd->page_size,
                            size);
    return res ? res : (int)size;
}

static int _write(long offset, const uint8_t *buf, size_t size)
{
    int res = mtd_write_page_raw(_mtd, buf,
                                 offset / _mtd->page_size,
                                 offset % _mtd->page_size, size);
    return res ? res : (int)size;
}

static int _erase(long offset, size_t size)
{
    int res = mtd_erase(_mtd, offset, size);
    return res ? res : (int)size;
}

static int _init(void)
{
    unsigned sector_size = _mtd->page_size * _mtd->pages_per_sector;

    mtd_flash0.len          = _mtd->sector_count * sector_size;
    mtd_flash0.blk_size     = sector_size;
    /* MTD ensures write_gran = 1 is possible */
    mtd_flash0.write_gran   = 1;

    return 1;
}

struct fal_flash_dev mtd_flash0 = {
    .name = "fal_mtd",
    .ops  = {
                .init  = _init,
                .read  = _read,
                .write = _write,
                .erase = _erase,
            },
};

void fdb_mtd_init(mtd_dev_t *mtd)
{
    assert(mtd);

    unsigned sector_size;
    if (_mtd) {
        return;
    }
    mtd_init(mtd);
    _mtd = mtd;

    /* long offset limits addressable MTD range */
    sector_size = _mtd->page_size * _mtd->pages_per_sector;
    _mtd->sector_count = MIN(_mtd->sector_count, LONG_MAX / sector_size);
}
