/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_at24xxxx
 * @{
 *
 * @file
 * @brief       MTD EEPROM driver implementation for at24xxxx EEPROM
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <assert.h>

#include "mtd_at24xxxx.h"

#define DEV(mtd_ptr)        (((mtd_at24xxxx_t *)(mtd_ptr))->at24xxxx_eeprom)
#define PARAMS(mtd_ptr)     (((mtd_at24xxxx_t *)(mtd_ptr))->params)

static int _mtd_at24xxxx_init(mtd_dev_t *mtd)
{
    assert(mtd);
    assert(mtd->driver == &mtd_at24xxxx_driver);
    assert(DEV(mtd));
    assert(PARAMS(mtd));
    int init = at24xxxx_init(DEV(mtd), PARAMS(mtd));
    if (init != AT24XXXX_OK) {
        return init;
    }
    mtd->page_size = DEV(mtd)->params.page_size;
    mtd->pages_per_sector = 1;
    mtd->sector_count = DEV(mtd)->params.eeprom_size /
                        DEV(mtd)->params.page_size;
    return 0;
}

static int _mtd_at24xxxx_read(mtd_dev_t *mtd, void *dest, uint32_t addr,
                              uint32_t size)
{
    return at24xxxx_read(DEV(mtd), addr, dest, size) == AT24XXXX_OK ? 0 : -EIO;
}

static int _mtd_at24xxxx_write(mtd_dev_t *mtd, const void *src, uint32_t addr,
                               uint32_t size)
{
    return at24xxxx_write(DEV(mtd), addr, src, size) == AT24XXXX_OK ? 0 : -EIO;
}

static int mtd_at24xxxx_write_page(mtd_dev_t *mtd, const void *src, uint32_t page,
                                   uint32_t offset, uint32_t size)
{
    return at24xxxx_write_page(DEV(mtd), page, offset, src, size);
}

static int _mtd_at24xxxx_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t size)
{
    return at24xxxx_clear(DEV(mtd), addr, size) == AT24XXXX_OK ? 0 : -EIO;
}

static int _mtd_at24xxxx_power(mtd_dev_t *mtd, enum mtd_power_state power)
{
    (void)mtd;
    (void)power;
    return -ENOTSUP;
}

const mtd_desc_t mtd_at24xxxx_driver = {
    .init = _mtd_at24xxxx_init,
    .read = _mtd_at24xxxx_read,
    .write = _mtd_at24xxxx_write,
    .write_page = mtd_at24xxxx_write_page,
    .erase = _mtd_at24xxxx_erase,
    .power = _mtd_at24xxxx_power,
    .flags = MTD_DRIVER_FLAG_DIRECT_WRITE,
};
