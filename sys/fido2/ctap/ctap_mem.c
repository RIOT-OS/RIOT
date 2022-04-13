/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup fido2_ctap_mem
 * @{
 * @file
 *
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 * @}
 */

#include <string.h>

#include "mtd.h"
#include "mtd_flashpage.h"

#include "fido2/ctap/ctap_mem.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief Reserve flash memory to store CTAP data
 */
FLASH_WRITABLE_INIT(_backing_memory, CONFIG_FIDO2_CTAP_NUM_FLASHPAGES);

/**
 * @brief   MTD device descriptor initialized with flash-page driver
 */
static mtd_flashpage_t _mtd_flash_dev = MTD_FLASHPAGE_INIT_VAL(CTAP_FLASH_PAGES_PER_SECTOR);
static mtd_dev_t *_mtd_dev = &_mtd_flash_dev.base;

/**
 * @brief   Check if flash region is erased
 */
static bool _flash_is_erased(int page, int offset, size_t len);

/**
 * @brief   Get available amount of flashpages to store resident keys
 */
static unsigned _amount_flashpages_rk(void);

int fido2_ctap_mem_init(void)
{
    int ret = mtd_init(_mtd_dev);

    if (ret < 0) {
        return ret;
    }

    return CTAP2_OK;
}

static unsigned _amount_flashpages_rk(void)
{
    return _mtd_dev->sector_count * _mtd_dev->pages_per_sector;
}

int fido2_ctap_mem_read(void *buf, uint32_t page, uint32_t offset, uint32_t len)
{
    assert(buf);

    int ret;

    ret = mtd_read_page(_mtd_dev, buf, page, offset, len);

    if (ret < 0) {
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

int fido2_ctap_mem_write(const void *buf, uint32_t page, uint32_t offset, uint32_t len)
{
    assert(buf);

    int ret;

    if (!_flash_is_erased(page, offset, len)) {
        ret = mtd_write_page(_mtd_dev, buf, page, offset, len);

        if (ret < 0) {
            return CTAP1_ERR_OTHER;
        }
    }
    else {
        ret = mtd_write_page_raw(_mtd_dev, buf, page, offset, len);

        if (ret < 0) {
            return CTAP1_ERR_OTHER;
        }
    }

    return CTAP2_OK;
}

static bool _flash_is_erased(int page, int offset, size_t len)
{
    uint8_t *addr = ((uint8_t *)flashpage_addr(page) + offset);

    for (size_t i = 0; i < len; i++) {
        if (addr[i] != FLASHPAGE_ERASE_STATE) {
            return false;
        }
    }

    return true;
}

int fido2_ctap_mem_get_flashpage_number_of_rk(uint16_t rk_idx)
{
    uint16_t idx = 0;
    unsigned start = fido2_ctap_mem_flash_page() + CTAP_FLASH_RK_OFF;
    unsigned amount = _amount_flashpages_rk();

    for (unsigned i = start; i < start + amount; i++) {
        idx += flashpage_size(i) / CTAP_FLASH_RK_SZ;

        if (idx >= rk_idx) {
            return i;
        }
    }

    return -1;
}

int fido2_ctap_mem_get_offset_of_rk_into_flashpage(uint16_t rk_idx)
{
    uint16_t idx = 0;
    unsigned start = fido2_ctap_mem_flash_page() + CTAP_FLASH_RK_OFF;
    unsigned amount = _amount_flashpages_rk();

    for (unsigned i = start; i < start + amount; i++) {
        uint16_t old_idx = idx;
        idx += flashpage_size(i) / CTAP_FLASH_RK_SZ;

        if (idx >= rk_idx) {
            return CTAP_FLASH_RK_SZ * (rk_idx - old_idx);
        }
    }

    return -1;
}

unsigned fido2_ctap_mem_flash_page(void)
{
    return flashpage_page((void *)_backing_memory);
}

int fido2_ctap_mem_erase_flash(void)
{
    unsigned start = fido2_ctap_mem_flash_page();
    unsigned end = start + CONFIG_FIDO2_CTAP_NUM_FLASHPAGES;

    for (unsigned page = start; page < end; page++) {
        flashpage_erase(page);
    }

    return CTAP2_OK;
}
