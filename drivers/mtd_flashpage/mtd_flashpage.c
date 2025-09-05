/*
 * SPDX-FileCopyrightText: 2018 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author      Fabian Hüßler <fabian.huessler@st.ovgu.de>
 * @}
 */

#include <string.h>
#include <errno.h>
#include <assert.h>

#include "architecture.h"
#include "cpu.h"
#include "cpu_conf.h"
#include "macros/utils.h"
#include "mtd_flashpage.h"
#include "periph/flashpage.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define MTD_FLASHPAGE_END_ADDR     ((uintptr_t) CPU_FLASH_BASE + (FLASHPAGE_NUMOF * FLASHPAGE_SIZE))

static int _init(mtd_dev_t *dev)
{
    mtd_flashpage_t *super = container_of(dev, mtd_flashpage_t, base);
    (void)super;
    assert(dev->pages_per_sector * dev->page_size == FLASHPAGE_SIZE);
    assert(!(super->offset % dev->pages_per_sector));

    /* Use separate variable to avoid '>= 0 is always true' warning */
    #ifndef NDEBUG
    static const uintptr_t cpu_flash_base = CPU_FLASH_BASE;
    #endif
    assert((uintptr_t)flashpage_addr(super->offset / dev->pages_per_sector) >= cpu_flash_base);
    assert((uintptr_t)flashpage_addr(super->offset / dev->pages_per_sector)
           + dev->pages_per_sector * dev->page_size * dev->sector_count <= MTD_FLASHPAGE_END_ADDR);
    assert((uintptr_t)flashpage_addr(super->offset / dev->pages_per_sector)
           + dev->pages_per_sector * dev->page_size * dev->sector_count > cpu_flash_base);
    return 0;
}

static int _read_page(mtd_dev_t *dev, void *buf, uint32_t page,
                      uint32_t offset, uint32_t size)
{
    mtd_flashpage_t *super = container_of(dev, mtd_flashpage_t, base);

    assert(page + super->offset >= page);
    page += super->offset;

    /* mtd flashpage maps multiple pages to one virtual sector for unknown reason */
    uint32_t fpage = page / dev->pages_per_sector;
    offset += (page % dev->pages_per_sector) * dev->page_size;
    uintptr_t addr = (uintptr_t)flashpage_addr(fpage);

    addr += offset;

    DEBUG("flashpage: read %"PRIu32" bytes from %p to %p\n", size, (void *)addr, buf);

#ifndef CPU_HAS_UNALIGNED_ACCESS
    if (addr % sizeof(uword_t)) {
        uword_t tmp;

        offset = addr % sizeof(uword_t);
        size = MIN(size, sizeof(uword_t) - offset);

        DEBUG("flashpage: read %"PRIu32" unaligned bytes\n", size);

        memcpy(&tmp, (uint8_t *)addr - offset, sizeof(tmp));
        memcpy(buf, (uint8_t *)&tmp + offset, size);
        return size;
    }
#endif

    memcpy(buf, (void *)addr, size);
    return size;
}

static int _write_page(mtd_dev_t *dev, const void *buf, uint32_t page, uint32_t offset,
                       uint32_t size)
{
    mtd_flashpage_t *super = container_of(dev, mtd_flashpage_t, base);

    assert(page + super->offset >= page);

    page += super->offset;

    /* mtd flashpage maps multiple pages to one virtual sector for unknown reason */
    uint32_t fpage = page / dev->pages_per_sector;
    offset += (page % dev->pages_per_sector) * dev->page_size;
    uintptr_t addr = (uintptr_t)flashpage_addr(fpage);

    addr += offset;

    DEBUG("flashpage: write %"PRIu32" bytes from %p to %p\n", size, buf, (void *)addr);

    size = MIN(flashpage_size(fpage) - offset, size);

    if ((addr % FLASHPAGE_WRITE_BLOCK_ALIGNMENT) || (size < FLASHPAGE_WRITE_BLOCK_SIZE) ||
        ((uintptr_t)buf % FLASHPAGE_WRITE_BLOCK_ALIGNMENT)) {
        uint8_t tmp[FLASHPAGE_WRITE_BLOCK_SIZE]
                __attribute__ ((aligned (FLASHPAGE_WRITE_BLOCK_ALIGNMENT)));

        offset = addr % FLASHPAGE_WRITE_BLOCK_ALIGNMENT;
        size = MIN(size, FLASHPAGE_WRITE_BLOCK_SIZE - offset);

        DEBUG("flashpage: write %"PRIu32" at %p - ""%"PRIu32"\n", size, (void *)addr, offset);

        memcpy(&tmp[0], (uint8_t *)addr - offset, sizeof(tmp));
        memcpy(&tmp[offset], buf, size);

        flashpage_write((uint8_t *)addr - offset, tmp, sizeof(tmp));

        return size;
    }

    /* don't write less than the write block size */
    size &= ~(FLASHPAGE_WRITE_BLOCK_SIZE - 1);

    flashpage_write((void *)addr, buf, size);
    return size;
}

static int _erase_sector(mtd_dev_t *dev, uint32_t sector, uint32_t count)
{
    mtd_flashpage_t *super = container_of(dev, mtd_flashpage_t, base);

    if (sector + (super->offset / dev->pages_per_sector) < sector) {
        return -EOVERFLOW;
    }
    sector += (super->offset / dev->pages_per_sector);

    while (count--) {
        DEBUG("flashpage: erase sector %"PRIu32"\n", sector);
        flashpage_erase(sector++);
    }

    return 0;
}

const mtd_desc_t mtd_flashpage_driver = {
    .init = _init,
    .read_page = _read_page,
    .write_page = _write_page,
    .erase_sector = _erase_sector,
};

#if CONFIG_SLOT_AUX_LEN
mtd_flashpage_t mtd_flash_aux_slot = MTD_FLASHPAGE_AUX_INIT_VAL(CONFIG_SLOT_AUX_OFFSET,
                                                                CONFIG_SLOT_AUX_LEN);
MTD_XFA_ADD(mtd_flash_aux_slot, CONFIG_SLOT_AUX_MTD_OFFSET);
mtd_dev_t *mtd_aux = &mtd_flash_aux_slot.base;

static_assert(CONFIG_SLOT_AUX_OFFSET % FLASHPAGE_SIZE == 0, "AUX slot must align with page");
static_assert(CONFIG_SLOT_AUX_LEN % FLASHPAGE_SIZE == 0, "AUX slot must align with page");
#endif
