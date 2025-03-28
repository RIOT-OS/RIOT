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
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 * @}
 */
#include "fido2/ctap.h"
#include "fido2/ctap/ctap.h"
#include "mtd.h"
#include "mtd_flashpage.h"

#include "fido2/ctap/ctap_mem.h"
#include "fido2/ctap/ctap_utils.h"
#include "macros/math.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static mtd_dev_t *_mtd_dev;

typedef struct {
    uint32_t off;
    ctap_resident_key_t rk;
} _rk_flash_entry_t;

static _rk_flash_entry_t _rk_iter_state;

/**
 * @brief Write to flash memory
 */
static ctap_status_code_t _flash_write(const void *buf, uint32_t off, size_t len);

/**
 * @brief Advance resident key iterator by one
 */
static _rk_flash_entry_t * _flash_rk_iter_next(const _rk_flash_entry_t *last);

/**
 * @brief Sanity check resident key flash offset
 */
static bool _flash_rk_check_offset(uint32_t off);

ctap_status_code_t fido2_ctap_mem_init(void)
{
    _mtd_dev = CONFIG_FIDO2_MTD_DEV;
    int ret = mtd_init(_mtd_dev);

    if (ret < 0) {
        DEBUG("%s:%d: Failed to initialize MTD device\n", __FILE__,
              __LINE__);
        return ret;
    }

    return CTAP2_OK;
}

ctap_status_code_t fido2_ctap_mem_read(void *buf, uint32_t page, uint32_t offset, uint32_t len)
{
    assert(buf);
    int ret;

    ret = mtd_read_page(_mtd_dev, buf, page, offset, len);

    if (ret < 0) {
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

static bool _flash_rk_check_offset(uint32_t off)
{
    /* Offset must skip ctap_state_t */
    if (off < CTAP_FLASH_STATE_SZ) {
        DEBUG("%s:%d: Incorrect offset detected\n", __FILE__,
              __LINE__);

        return false;
    }
    /* Must be aligned to CTAP_FLASH_RK_SZ */
    if ((off - CTAP_FLASH_STATE_SZ) % CTAP_FLASH_RK_SZ != 0) {
        DEBUG("%s:%d: Incorrect offset detected\n", __FILE__,
              __LINE__);

        return false;
    }

    return true;
}

static ctap_status_code_t _flash_write(const void *buf, uint32_t off, size_t len)
{
    assert(buf);

    uint32_t page = off / _mtd_dev->page_size;
    uint32_t page_off = off % _mtd_dev->page_size;

    int ret = mtd_write_page(_mtd_dev, buf, page, page_off, len);

    if (ret < 0) {
        DEBUG("%s:%d: mtd_write_page failed\n", __FILE__,
              __LINE__);
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

ctap_status_code_t fido2_ctap_mem_erase_flash(ctap_state_t *state)
{
    /* Calculate total pages needed, rounding up */
    uint32_t used_pages = DIV_ROUND_UP(CTAP_FLASH_STATE_SZ + state->rk_amount_stored * \
                                       CTAP_FLASH_RK_SZ, _mtd_dev->page_size);
    /* Calculate the number of sectors needed, rounding up */
    uint32_t sector_cnt = (used_pages + _mtd_dev->pages_per_sector - 1) /
                          _mtd_dev->pages_per_sector;

    int ret = mtd_erase_sector(_mtd_dev, 0, sector_cnt);

    return ret == 0 ? CTAP2_OK : CTAP1_ERR_OTHER;
}

/**
 * CTAP state information is stored at flashpage 0 of the memory area
 * dedicated for storing CTAP data
 */
ctap_status_code_t fido2_ctap_mem_read_state_from_flash(ctap_state_t *state)
{
    int ret = mtd_read_page(_mtd_dev, state, 0, 0, sizeof(ctap_state_t));

    return ret == 0 ? CTAP2_OK : CTAP1_ERR_OTHER;
}

static ctap_status_code_t _flash_read_rk(_rk_flash_entry_t *entry)
{
    if (!entry) {
        return CTAP1_ERR_OTHER;
    }

    uint32_t page = entry->off / _mtd_dev->page_size;
    uint32_t page_off = entry->off % _mtd_dev->page_size;

    int ret = mtd_read_page(_mtd_dev, &entry->rk, page, page_off, sizeof(ctap_resident_key_t));
    if (ret < 0) {
        DEBUG("%s:%d: mtd_read failed\n", __FILE__,
              __LINE__);
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

static _rk_flash_entry_t * _flash_rk_iter_next(const _rk_flash_entry_t *last_entry)
{
    uint16_t amt_stored = fido2_ctap_get_state()->rk_amount_stored;
    /**
     * If last_entry == NULL, skip ctap_state_t struct since this function only deals
     * with resident keys. Otherwise, continue iterating from the current resident
     * key offset.
     */
    uint32_t off = (last_entry) ? last_entry->off + CTAP_FLASH_RK_SZ : CTAP_FLASH_STATE_SZ;
    uint32_t page = off / _mtd_dev->page_size;
    uint32_t page_off = off % _mtd_dev->page_size;

    uint16_t rks_read = (off - CTAP_FLASH_STATE_SZ) / CTAP_FLASH_RK_SZ;

    if (rks_read > amt_stored) {
        return NULL;
    }

    _rk_iter_state.off = off;
    return &_rk_iter_state;
}

/**
 * overwrite existing key if equal, else find free space.
 *
 * The current official CTAP spec does not have credential management yet
 * so rk's can't be deleted, only overwritten => we can be sure that there are
 * no holes when reading keys from flash memory
 */
ctap_status_code_t fido2_ctap_mem_write_rk_to_flash(ctap_resident_key_t *rk)
{
    uint16_t amt_stored = fido2_ctap_get_state()->rk_amount_stored;
    _rk_flash_entry_t *rk_iter = NULL;

    while ((rk_iter = _flash_rk_iter_next(rk_iter)) != NULL) {
        int ret = _flash_read_rk(rk_iter);
        if (ret != CTAP2_OK) {
            return ret;
        }

        if (fido2_ctap_utils_ks_equal(&rk_iter->rk, rk)) {
            break;
        }
    }

    uint32_t off = (rk_iter) ? rk_iter->off : amt_stored * CTAP_FLASH_RK_SZ + CTAP_FLASH_STATE_SZ;

    /* Write new resident key to flash */
    if (!rk_iter) {
        if (amt_stored > CTAP_FLASH_MAX_NUM_RKS) {
            return CTAP2_ERR_KEY_STORE_FULL;
        }

        ctap_state_t *state = fido2_ctap_get_state();
        state->rk_amount_stored++;
        int ret = fido2_ctap_mem_write_state_to_flash(state);

        if (ret != CTAP2_OK) {
            return ret;
        }

        return _flash_write(rk, off, CTAP_FLASH_RK_SZ);
    }

    /* Overwrite existing resident key */
    return _flash_write(rk, off, CTAP_FLASH_RK_SZ);
}

ctap_status_code_t fido2_ctap_mem_write_state_to_flash(ctap_state_t *state)
{
    return _flash_write(state, 0, CTAP_FLASH_STATE_SZ);
}

ctap_status_code_t fido2_ctap_mem_rk_iter(ctap_resident_key_t *key, void **state)
{
    _rk_flash_entry_t *rk_iter = *state;

    if (rk_iter && !_flash_rk_check_offset(rk_iter->off)) {
        return CTAP1_ERR_OTHER;
    }

    if ((rk_iter = _flash_rk_iter_next(rk_iter)) != NULL) {
        int ret = _flash_read_rk(rk_iter);
        if (ret != CTAP2_OK) {
            return ret;
        }

        memcpy(key, &rk_iter->rk, sizeof(ctap_resident_key_t));
        *state = rk_iter;
        return CTAP2_OK;
    }

    return CTAP2_ERR_NO_CREDENTIALS;
}
