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
 * @brief   MTD device descriptor initialized with flash-page driver
 */
static mtd_dev_t _mtd_dev = MTD_FLASHPAGE_INIT_VAL(CTAP_FLASH_PAGES_PER_SECTOR);

/**
 * @brief   Max amount of resident keys that can be stored
 */
static uint16_t _max_rk_amnt;

/**
 * @brief   Check if flash region is erased
 */
static bool _flash_is_erased(int page, int offset, size_t len);

/**
 * @brief   Get amount of flashpages
 */
static unsigned _amount_of_flashpages(void);

int fido2_ctap_mem_init(void)
{
    int ret;

    ret = mtd_init(&_mtd_dev);

    if (ret < 0) {
        return ret;
    }

    for (unsigned i = CTAP_FLASH_RK_START_PAGE; i < _amount_of_flashpages(); i++) {
        _max_rk_amnt += flashpage_size(i) / CTAP_FLASH_RK_SZ;
    }

    return CTAP2_OK;
}

static unsigned _amount_of_flashpages(void)
{
    return _mtd_dev.sector_count * _mtd_dev.pages_per_sector;
}

int fido2_ctap_mem_read(void *buf, uint32_t page, uint32_t offset, uint32_t len)
{
    assert(buf);

    int ret;

    ret = mtd_read_page(&_mtd_dev, buf, page, offset, len);

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
        ret = mtd_write_page(&_mtd_dev, buf, page, offset, len);

        if (ret < 0) {
            return CTAP1_ERR_OTHER;
        }
    }
    else {
        ret = mtd_write_page_raw(&_mtd_dev, buf, page, offset, len);

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

uint16_t fido2_ctap_mem_get_max_rk_amount(void)
{
    return _max_rk_amnt;
}

int fido2_ctap_mem_get_flashpage_number_of_rk(uint16_t rk_idx)
{
    uint16_t idx = 0;

    for (unsigned i = CTAP_FLASH_RK_START_PAGE; i < _amount_of_flashpages(); i++) {
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

    for (unsigned i = CTAP_FLASH_RK_START_PAGE; i < _amount_of_flashpages(); i++) {
        uint16_t old_idx = idx;
        idx += flashpage_size(i) / CTAP_FLASH_RK_SZ;

        if (idx >= rk_idx) {
            return CTAP_FLASH_RK_SZ * (rk_idx - old_idx);
        }
    }

    return -1;
}
