/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_fido2_ctap_mem
 * @{
 * @file
 *
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 * @}
 */

#include <string.h>

#include "bitarithm.h"

#include "mtd.h"
#include "mtd_flashpage.h"

#include "fido2/ctap/ctap_mem.h"
#include "fido2/ctap/ctap_utils.h"

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
static bool _flash_is_erased(uint32_t addr, size_t len);

/**
 * @brief   Get available amount of flashpages to store resident keys
 */
static unsigned _amount_flashpages_rk(void);

/**
 * @brief Write to flash memory
 */
static int _flash_write(const void *buf, uint32_t addr, size_t len);

/**
 * @brief Get start address of reserved flash memory region
 */
static unsigned _flash_start(void);

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

static int _flash_write(const void *buf, uint32_t addr, size_t len)
{
    assert(buf);
    int ret;

    if (!_flash_is_erased(addr, len)) {
        /* page size is always a power of two */
        const uint32_t page_shift = bitarithm_msb(_mtd_dev->page_size);
        const uint32_t page_mask = _mtd_dev->page_size - 1;

        ret = mtd_write_page(_mtd_dev, buf, addr >> page_shift, addr & page_mask, len);

        if (ret < 0) {
            return CTAP1_ERR_OTHER;
        }
    }
    else {
        ret = mtd_write(_mtd_dev, buf, addr, len);

        if (ret < 0) {
            return CTAP1_ERR_OTHER;
        }
    }

    return CTAP2_OK;
}

static bool _flash_is_erased(uint32_t addr, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (*(uint32_t *)(addr + i) != FLASHPAGE_ERASE_STATE) {
            return false;
        }
    }

    return true;
}

static unsigned _flash_start(void)
{
    return flashpage_page((void *)_backing_memory);
}

int fido2_ctap_mem_erase_flash(void)
{
    unsigned start = _flash_start();
    unsigned end = start + CONFIG_FIDO2_CTAP_NUM_FLASHPAGES;

    for (unsigned page = start; page < end; page++) {
        flashpage_erase(page);
    }

    return CTAP2_OK;
}

/**
 * CTAP state information is stored at flashpage 0 of the memory area
 * dedicated for storing CTAP data
 */
int fido2_ctap_mem_read_state_from_flash(ctap_state_t *state)
{
    uint32_t addr = (uint32_t)flashpage_addr(_flash_start());

    return mtd_read(_mtd_dev, state, addr, sizeof(ctap_state_t));
}

/**
 * overwrite existing key if equal, else find free space.
 *
 * The current official CTAP spec does not have credential management yet
 * so rk's can't be deleted, only overwritten => we can be sure that there are
 * no holes when reading keys from flash memory
 */
int fido2_ctap_mem_write_rk_to_flash(ctap_resident_key_t *rk)
{
    int ret;
    uint32_t addr = (uint32_t)flashpage_addr(_flash_start() + CTAP_FLASH_RK_OFF);
    uint16_t amt_stored = fido2_ctap_get_state()->rk_amount_stored;
    ctap_resident_key_t tmp = { 0 };
    bool equal = false;

    for (uint16_t i = 0; i < amt_stored; i++) {
        ret = mtd_read(_mtd_dev, &tmp, addr, sizeof(ctap_resident_key_t));

        if (ret < 0) {
            DEBUG("%s, %d: mtd_read failed", RIOT_FILE_RELATIVE,
                  __LINE__);
            return false;
        }

        if (fido2_ctap_utils_ks_equal(&tmp, rk)) {
            equal = true;
            break;
        }

        addr += CTAP_FLASH_RK_SZ;
    }

    if (!equal) {
        if (amt_stored >= CTAP_FLASH_MAX_NUM_RKS) {
            return CTAP2_ERR_KEY_STORE_FULL;
        }

        ctap_state_t *state = fido2_ctap_get_state();
        state->rk_amount_stored++;
        ret = fido2_ctap_mem_write_state_to_flash(state);

        if (ret != CTAP2_OK) {
            return ret;
        }
    }

    return _flash_write(rk, addr, CTAP_FLASH_RK_SZ);
}

int fido2_ctap_mem_write_state_to_flash(ctap_state_t *state)
{
    uint32_t addr = (uint32_t)flashpage_addr(_flash_start());

    return _flash_write(state, addr, CTAP_FLASH_STATE_SZ);
}

int fido2_ctap_mem_read_rk_from_flash(ctap_resident_key_t *key, uint8_t *rp_id_hash, uint32_t *addr)
{
    uint16_t end;
    uint16_t amt_stored = fido2_ctap_get_state()->rk_amount_stored;

    if (*addr == 0x0) {
        end = amt_stored;
        *addr = (uint32_t)flashpage_addr(_flash_start() + CTAP_FLASH_RK_OFF);
    }
    else {
        uint32_t start_addr = (uint32_t)flashpage_addr(_flash_start() + CTAP_FLASH_RK_OFF);
        uint16_t rks_read = (*addr - start_addr) / CTAP_FLASH_RK_SZ;

        if (rks_read > amt_stored) {
            return CTAP1_ERR_OTHER;
        }

        end = amt_stored - rks_read;
    }

    for (uint16_t i = 0; i < end; i++) {
        int ret = mtd_read(_mtd_dev, key, *addr, sizeof(ctap_resident_key_t));

        if (ret < 0) {
            DEBUG("%s, %d: mtd_read failed", RIOT_FILE_RELATIVE,
                  __LINE__);
            return CTAP1_ERR_OTHER;
        }

        *addr += CTAP_FLASH_RK_SZ;

        if (memcmp(key->rp_id_hash, rp_id_hash, SHA256_DIGEST_LENGTH) == 0) {
            return CTAP2_OK;
        }
    }

    return CTAP1_ERR_OTHER;
}
