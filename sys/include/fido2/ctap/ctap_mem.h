/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    fido2_ctap_mem FIDO2 CTAP flash
 * @ingroup     fido2_ctap
 * @brief       FIDO2 CTAP flash memory helper
 *
 * @{
 *
 * @file
 * @brief       Definitions for CTAP flash memory helper functions
 *
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 */

#ifndef FIDO2_CTAP_CTAP_MEM_H
#define FIDO2_CTAP_CTAP_MEM_H

#include <stdint.h>

#include "fido2/ctap/ctap.h"
#include "periph/flashpage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   MAX function for internal use
 * @{
 */
#ifndef _MAX
#define _MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
/** @} */

/**
 * @brief   Default amount of flashpages to use
 */
#ifndef CONFIG_FIDO2_CTAP_NUM_FLASHPAGES
#define CONFIG_FIDO2_CTAP_NUM_FLASHPAGES 4
#endif

#if CONFIG_FIDO2_CTAP_NUM_FLASHPAGES < 2
#error "ctap_mem.h: Configured number of flashpages is invalid"
#endif

/**
 * @brief Calculate padding needed to align struct size for saving to flash
 */
#define CTAP_FLASH_ALIGN_PAD(x) (sizeof(x) % FLASHPAGE_WRITE_BLOCK_SIZE == \
                                 0 ? \
                                 0 : FLASHPAGE_WRITE_BLOCK_SIZE - \
                                 sizeof(x) % FLASHPAGE_WRITE_BLOCK_SIZE)

/**
 * @brief Resident key size with alignment padding
 */
#define CTAP_FLASH_RK_SZ (sizeof(ctap_resident_key_t) + \
                          CTAP_FLASH_ALIGN_PAD(ctap_resident_key_t))

/**
 * @brief State struct size with alignment padding
 */
#define CTAP_FLASH_STATE_SZ (sizeof(ctap_state_t) + \
                             CTAP_FLASH_ALIGN_PAD(ctap_state_t))

/**
 * @brief Max amount of resident keys that can be stored on device
 */
#define CTAP_FLASH_MAX_NUM_RKS ((CONFIG_FIDO2_CTAP_NUM_FLASHPAGES - 1) * \
                                FLASHPAGE_SIZE / CTAP_FLASH_RK_SZ)

/**
 * @brief Minimum flash sector size needed to hold CTAP related data
 *
 * This is needed to ensure that the MTD work_area buffer is big enough
 */
#define CTAP_FLASH_MIN_SECTOR_SZ _MAX(CTAP_FLASH_STATE_SZ, CTAP_FLASH_RK_SZ)

/**
 * @brief Pages per sector needed
 */
#define CTAP_FLASH_PAGES_PER_SECTOR ((CTAP_FLASH_MIN_SECTOR_SZ / FLASHPAGE_SIZE) + 1)

/**
 * Offset of flashpage for storing resident keys
 *
 * The offset is in units of flashpages from the beginning of the flash memory
 * area dedicated for storing CTAP data.
 */
#define CTAP_FLASH_RK_OFF 0x1

/**
 * @brief Initialize memory helper
 *
 * @return @ref ctap_status_codes_t
 */
int fido2_ctap_mem_init(void);

/**
 * @brief Read from flash memory
 *
 * @param[out] buf        buffer to fil in
 * @param[in]  page       page to read from
 * @param[in]  offset     offset from the start of the page (in bytes)
 * @param[in] len       number of bytes to write
 *
 * @return @ref ctap_status_codes_t
 */
int fido2_ctap_mem_read(void *buf, uint32_t page, uint32_t offset, uint32_t len);

/**
 * @brief Erase all flashpages containing CTAP data
 *
 * @return @ref ctap_status_codes_t
 */
int fido2_ctap_mem_erase_flash(void);

/**
 * @brief Read authenticator state from flash
 *
 * @param[in]  state       pointer to authenticator state
 *
 * @return @ref ctap_status_codes_t
 */
int fido2_ctap_mem_read_state_from_flash(ctap_state_t *state);

/**
 * @brief Write authenticator state to flash
 *
 * @param[in]  state       pointer to authenticator state
 *
 * @return @ref ctap_status_codes_t
 */
int fido2_ctap_mem_write_state_to_flash(ctap_state_t *state);

/**
 * @brief Find resident credential for @p rp_id_hash in flash
 *
 * The function stores the flash address of the next credential in @p addr.
 * This allows for consecutive calls of the function in order to find all
 * stored credentials stored for the relying party identified by
 * @p rp_id_hash.
 *
 * @param[in]  key       pointer to authenticator state
 * @param[in]   rp_id_hash pointer to hash of rp domain string
 * @param[in] addr pointer to address where to read from
 *
 * @return @ref ctap_status_codes_t
 */
int fido2_ctap_mem_read_rk_from_flash(ctap_resident_key_t *key, uint8_t *rp_id_hash,
                                      uint32_t *addr);

/**
 * @brief Write resident credential to flash
 *
 * @param[in]  rk      pointer to resident credential
 *
 * @return @ref ctap_status_codes_t
 */
int fido2_ctap_mem_write_rk_to_flash(ctap_resident_key_t *rk);

#ifdef __cplusplus
}
#endif
#endif /* FIDO2_CTAP_CTAP_MEM_H */
/** @} */
