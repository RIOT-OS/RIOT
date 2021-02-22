/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mtd_flashpage
 * @brief       Driver for internal flash devices implementing flashpage interface
 *
 * @{
 *
 * @file
 * @brief       Implementation for the flashpage memory driver
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include <string.h>
#include <errno.h>
#include <assert.h>

#include "architecture.h"
#include "cpu.h"
#include "cpu_conf.h"
#include "mtd_flashpage.h"
#include "periph/flashpage.h"

#define MTD_FLASHPAGE_END_ADDR     ((uint32_t) CPU_FLASH_BASE + (FLASHPAGE_NUMOF * FLASHPAGE_SIZE))

static int _init(mtd_dev_t *dev)
{
    (void)dev;
    assert(dev->pages_per_sector * dev->page_size == FLASHPAGE_SIZE);
    return 0;
}

static int _read(mtd_dev_t *dev, void *buf, uint32_t addr, uint32_t size)
{
    assert(addr < MTD_FLASHPAGE_END_ADDR);

    (void)dev;

    uword_t dst_addr = addr;
    memcpy(buf, (void *)dst_addr, size);

    return 0;
}

static int _write(mtd_dev_t *dev, const void *buf, uint32_t addr, uint32_t size)
{
    (void)dev;

#ifndef CPU_HAS_UNALIGNED_ACCESS
    if ((uintptr_t)buf % sizeof(uword_t)) {
        return -EINVAL;
    }
#endif
    if (addr % FLASHPAGE_WRITE_BLOCK_ALIGNMENT) {
        return -EINVAL;
    }
    if (size % FLASHPAGE_WRITE_BLOCK_SIZE) {
        return -EOVERFLOW;
    }
    if (addr + size > MTD_FLASHPAGE_END_ADDR) {
        return -EOVERFLOW;
    }

    uword_t dst_addr = addr;
    flashpage_write((void *)dst_addr, buf, size);

    return 0;
}

int _erase(mtd_dev_t *dev, uint32_t addr, uint32_t size)
{
    size_t sector_size = dev->page_size * dev->pages_per_sector;

    if (size % sector_size) {
        return -EOVERFLOW;
    }
    if (addr + size > MTD_FLASHPAGE_END_ADDR) {
        return -EOVERFLOW;
    }
    if (addr % sector_size) {
        return -EOVERFLOW;
    }

    uword_t dst_addr = addr;

    for (size_t i = 0; i < size; i += sector_size) {
        flashpage_erase(flashpage_page((void *)(dst_addr + i)));
    }

    return 0;
}

const mtd_desc_t mtd_flashpage_driver = {
    .init = _init,
    .read = _read,
    .write = _write,
    .erase = _erase,
};
