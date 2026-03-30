/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mtd_eeprom
 * @{
 *
 * @file
 * @brief       MTD Driver for internal EEPROM
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "board.h"
#include "cpu.h"
#include "cpu_conf.h"
#include "mtd_eeprom.h"
#include "periph/eeprom.h"
#include "limits.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _init(mtd_dev_t *dev)
{
    assert(EEPROM_SIZE % dev->page_size == 0);
    assert(EEPROM_SIZE / dev->sector_count == dev->pages_per_sector * dev->page_size);
    return 0;
}

static int _read_page(mtd_dev_t *dev, void *buf, uint32_t page,
                      uint32_t offset, uint32_t size)
{
    assert(size <= INT_MAX);
    eeprom_read(dev->page_size * page + offset, buf, size);
    return (int)size;
}

static int _write_page(mtd_dev_t *dev, const void *buf, uint32_t page, uint32_t offset,
                       uint32_t size)
{
    assert(size <= INT_MAX);
    eeprom_write(dev->page_size * page + offset, buf, size);
    return (int)size;
}

static int _erase(mtd_dev_t *dev, uint32_t addr, uint32_t count)
{
    (void)dev;
    if (count > UINT32_MAX - addr) {
        return -EOVERFLOW;
    }
    if (addr + count > EEPROM_SIZE) {
        return -EOVERFLOW;
    }
    eeprom_clear(addr, count);
    return 0;
}

const mtd_desc_t mtd_eeprom_driver = {
    .init = _init,
    .read_page = _read_page,
    .write_page = _write_page,
    .erase = _erase,
    .flags = MTD_DRIVER_FLAG_DIRECT_WRITE,
};
