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
#include "mtd_flashpage.h"

#include "fido2/ctap/ctap_mem.h"
#include "fido2/ctap/ctap_utils.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static mtd_dev_t *_mtd_dev;

#ifdef BOARD_NATIVE
#include "mtd_default.h"
#endif

/**
 * @brief Write to flash memory
 */
static ctap_status_code_t _flash_write(const void *buf, uint32_t page, uint32_t off, size_t len);

ctap_status_code_t fido2_ctap_mem_init(void)
{
#ifdef BOARD_NATIVE
    _mtd_dev = mtd_default_get_dev(0);
#else
    _mtd_dev = mtd_aux;
#endif

    int ret = mtd_init(_mtd_dev);

    if (ret < 0) {
        DEBUG("%s, %d: mtd_init failed\n", RIOT_FILE_RELATIVE,
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

static ctap_status_code_t _flash_write(const void *buf, uint32_t page, uint32_t off, size_t len)
{
    assert(buf);
    int ret;

    ret = mtd_write_page(_mtd_dev, buf, page, off, len);

    if (ret < 0) {
        DEBUG("%s, %d: mtd_write_page failed\n", RIOT_FILE_RELATIVE,
              __LINE__);
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

ctap_status_code_t fido2_ctap_mem_erase_flash(ctap_state_t *state)
{
    /* Calculate total pages needed, rounding up */
    uint32_t used_pages = (CTAP_FLASH_STATE_SZ + state->rk_amount_stored * CTAP_FLASH_RK_SZ +
                           _mtd_dev->page_size - 1) / _mtd_dev->page_size;
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

/**
 * overwrite existing key if equal, else find free space.
 *
 * The current official CTAP spec does not have credential management yet
 * so rk's can't be deleted, only overwritten => we can be sure that there are
 * no holes when reading keys from flash memory
 */
ctap_status_code_t fido2_ctap_mem_write_rk_to_flash(ctap_resident_key_t *rk)
{
    int ret;
    uint16_t amt_stored = fido2_ctap_get_state()->rk_amount_stored;
    ctap_resident_key_t tmp = { 0 };
    bool equal = false;

    /* skip ctap_state_t struct */
    uint32_t off = CTAP_FLASH_STATE_SZ;
    uint32_t page = off / _mtd_dev->page_size;
    uint32_t page_off = off % _mtd_dev->page_size;

    for (uint16_t i = 0; i < amt_stored; i++) {
        ret = mtd_read_page(_mtd_dev, &tmp, page, page_off, sizeof(ctap_resident_key_t));

        if (ret < 0) {
            DEBUG("%s, %d: mtd_read failed\n", RIOT_FILE_RELATIVE,
                  __LINE__);
            return false;
        }

        if (fido2_ctap_utils_ks_equal(&tmp, rk)) {
            equal = true;
            break;
        }

        off += CTAP_FLASH_RK_SZ;
        page = off / _mtd_dev->page_size;
        page_off = off % _mtd_dev->page_size;
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

    return _flash_write(rk, page, page_off, CTAP_FLASH_RK_SZ);
}

ctap_status_code_t fido2_ctap_mem_write_state_to_flash(ctap_state_t *state)
{
    return _flash_write(state, 0, 0, CTAP_FLASH_STATE_SZ);
}

ctap_status_code_t fido2_ctap_mem_read_rk_from_flash(ctap_resident_key_t *key,
                                                     const uint8_t *rp_id_hash,
                                                     uint32_t *off)
{
    uint16_t end;
    uint16_t amt_stored = fido2_ctap_get_state()->rk_amount_stored;

    /* some error checks for weird offsets that indicate something went wrong */
    if (*off != 0x0 && *off < CTAP_FLASH_STATE_SZ) {
        DEBUG("%s, %d: Incorrect offset detected\n", RIOT_FILE_RELATIVE,
              __LINE__);

        return CTAP1_ERR_OTHER;
    }

    if (*off > CTAP_FLASH_STATE_SZ && (*off - CTAP_FLASH_STATE_SZ) % CTAP_FLASH_RK_SZ != 0) {
        DEBUG("%s, %d: Incorrect offset detected\n", RIOT_FILE_RELATIVE,
              __LINE__);

        return CTAP1_ERR_OTHER;
    }

    if (*off == 0x0) {
        end = amt_stored;
        /* skip ctap_state_t struct */
        *off = CTAP_FLASH_STATE_SZ;
    }
    else {
        uint16_t rks_read = (*off - CTAP_FLASH_STATE_SZ) / CTAP_FLASH_RK_SZ;

        if (rks_read > amt_stored) {
            return CTAP1_ERR_OTHER;
        }

        end = amt_stored - rks_read;
    }

    for (uint16_t i = 0; i < end; i++) {
        uint32_t page = *off / _mtd_dev->page_size;
        uint32_t page_off = *off % _mtd_dev->page_size;

        int ret = mtd_read_page(_mtd_dev, key, page, page_off, sizeof(ctap_resident_key_t));

        if (ret < 0) {
            DEBUG("%s, %d: mtd_read failed", RIOT_FILE_RELATIVE,
                  __LINE__);
            return CTAP1_ERR_OTHER;
        }

        *off += CTAP_FLASH_RK_SZ;

        if (memcmp(key->rp_id_hash, rp_id_hash, SHA256_DIGEST_LENGTH) == 0) {
            return CTAP2_OK;
        }
    }

    return CTAP1_ERR_OTHER;
}
