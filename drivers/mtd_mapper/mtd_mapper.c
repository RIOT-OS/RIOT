/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mtd_mapper
 * @{
 *
 * @file
 * @brief       Driver for flash memory remap support
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <errno.h>

#include "kernel_defines.h"
#include "mtd.h"
#include "mtd_mapper.h"
#include "mutex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _unlock(mtd_mapper_region_t *region)
{
    mutex_unlock(&region->parent->lock);
}

static void _lock(mtd_mapper_region_t *region)
{
    mutex_lock(&region->parent->lock);
}

static uint32_t _region_size(mtd_mapper_region_t *region)
{
    return region->mtd.page_size * region->mtd.pages_per_sector *
           region->mtd.sector_count;
}

static uint32_t _byte_offset(mtd_mapper_region_t *region)
{
    return region->mtd.pages_per_sector * region->mtd.page_size *
           region->sector;
}

static uint32_t _page_offset(mtd_mapper_region_t *region)
{
    return region->mtd.pages_per_sector * region->sector;
}

static int _init_target(mtd_mapper_region_t *region)
{
    mtd_mapper_parent_t *parent = region->parent;

    if (!parent->init) {
        parent->init = true;
        return mtd_init(parent->mtd);
    }
    return 0;
}

static int _init(mtd_dev_t *mtd)
{

    mtd_mapper_region_t *region = container_of(mtd, mtd_mapper_region_t, mtd);
    mtd_dev_t *backing_mtd = region->parent->mtd;

    /* inherit physical properties */
    if (region->mtd.page_size == 0) {
        region->mtd.page_size = backing_mtd->page_size;
    }
    if (region->mtd.pages_per_sector == 0) {
        region->mtd.pages_per_sector = backing_mtd->pages_per_sector;
    }
    region->mtd.write_size = backing_mtd->write_size;

    /* Configuration sanity check */
    assert(backing_mtd->page_size >= region->mtd.page_size);
    assert(backing_mtd->write_size <= region->mtd.page_size);
    assert(region->mtd.page_size * region->mtd.pages_per_sector
        == backing_mtd->page_size * backing_mtd->pages_per_sector);
    assert(backing_mtd->sector_count >= region->mtd.sector_count);

    /* offset + region size must not exceed the backing device */
    assert(region->sector + region->mtd.sector_count <= backing_mtd->sector_count);

    /* avoid unused variable warning if compiled with NDEBUG */
    (void)backing_mtd;

    _lock(region);
    int res = _init_target(region);
    _unlock(region);
    return res;
}

static int _write_page(mtd_dev_t *mtd, const void *src, uint32_t page,
                       uint32_t offset, uint32_t count)
{
    mtd_mapper_region_t *region = container_of(mtd, mtd_mapper_region_t, mtd);

    _lock(region);
    int res = mtd_write_page_raw(region->parent->mtd, src,
                                 page + _page_offset(region),
                                 offset, count);
    _unlock(region);

    if (res < 0) {
        return res;
    }

    /* mtd_write_page_raw() returns 0 on success
     * but we are expected to return the written byte count */
    return count;
}

static int _read(mtd_dev_t *mtd, void *dest, uint32_t addr, uint32_t count)
{
    mtd_mapper_region_t *region = container_of(mtd, mtd_mapper_region_t, mtd);

    if (addr + count > _region_size(region)) {
        return -EOVERFLOW;
    }

    _lock(region);
    int res = mtd_read(region->parent->mtd, dest, addr + _byte_offset(region), count);
    _unlock(region);
    return res;
}

static int _read_page(mtd_dev_t *mtd, void *dest, uint32_t page,
                      uint32_t offset, uint32_t count)
{
    mtd_mapper_region_t *region = container_of(mtd, mtd_mapper_region_t, mtd);

    _lock(region);
    int res = mtd_read_page(region->parent->mtd, dest,
                            page + _page_offset(region),
                            offset, count);
    _unlock(region);

    if (res < 0) {
        return res;
    }

    /* mtd_read_page() returns 0 on success
     * but we are expected to return the read byte count */
    return count;
}

static int _erase(mtd_dev_t *mtd, uint32_t addr, uint32_t count)
{
    mtd_mapper_region_t *region = container_of(mtd, mtd_mapper_region_t, mtd);

    if (addr + count > _region_size(region)) {
        return -EOVERFLOW;
    }

    _lock(region);
    int res = mtd_erase(region->parent->mtd, addr + _byte_offset(region), count);
    _unlock(region);
    return res;
}

static int _erase_sector(mtd_dev_t *mtd, uint32_t sector, uint32_t count)
{
    mtd_mapper_region_t *region = container_of(mtd, mtd_mapper_region_t, mtd);

     _lock(region);
    int res = mtd_erase_sector(region->parent->mtd, region->sector + sector, count);
    _unlock(region);
    return res;
}

const mtd_desc_t mtd_mapper_driver = {
    .init = _init,
    .read = _read,
    .read_page = _read_page,
    .write_page = _write_page,
    .erase = _erase,
    .erase_sector = _erase_sector,
};
