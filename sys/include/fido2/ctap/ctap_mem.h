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

#include "assert.h"
#include <stdint.h>
#include <stddef.h>

#include "fido2/ctap/ctap.h"

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
 * @brief Ensure CONFIG_SLOT_AUX_LEN is defined
 */
#ifndef CONFIG_SLOT_AUX_LEN
    #define CONFIG_SLOT_AUX_LEN 0
#endif

/**
 * @brief Calculate padding needed to align struct size for saving to flash
 */
#define CTAP_FLASH_ALIGN_PAD(x) (sizeof(x) % FLASHPAGE_WRITE_BLOCK_ALIGNMENT == \
                                 0 ?                               \
                                 0 :                                   \
                                 FLASHPAGE_WRITE_BLOCK_ALIGNMENT -          \
                                 sizeof(x) % FLASHPAGE_WRITE_BLOCK_ALIGNMENT)

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
#define CTAP_FLASH_MAX_NUM_RKS ((CONFIG_SLOT_AUX_LEN - CTAP_FLASH_STATE_SZ) / \
                                CTAP_FLASH_RK_SZ)

/* SLOT_AUX_LEN must be large enough to store at least one resident key and the CTAP state */
static_assert(CONFIG_SLOT_AUX_LEN >= (CTAP_FLASH_RK_SZ + CTAP_FLASH_STATE_SZ),
              "SLOT_AUX_LEN is too small or not configured");

/**
 * @brief Initialize memory helper
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_mem_init(void);

/**
 * @brief Read from flash memory
 *
 * @param[out] buf        buffer to fil in
 * @param[in]  page       page to read from
 * @param[in]  offset     offset from the start of the page (in bytes)
 * @param[in] len       number of bytes to write
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_mem_read(void *buf, uint32_t page, uint32_t offset, uint32_t len);

/**
 * @brief Erase all flashpages containing CTAP data
 *
 * @param[in]  state       pointer to authenticator state
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_mem_erase_flash(ctap_state_t *state);

/**
 * @brief Read authenticator state from flash
 *
 * @param[in]  state       pointer to authenticator state
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_mem_read_state_from_flash(ctap_state_t *state);

/**
 * @brief Write authenticator state to flash
 *
 * @param[in]  state       pointer to authenticator state
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_mem_write_state_to_flash(ctap_state_t *state);

/**
 * @brief Find resident credential for @p rp_id_hash in flash memory
 *
 * This function searches for a resident credential associated with the
 * relying party identifier hash ( @p rip_id_hash ) in the flash memory region
 * used by the FIDO2 implementation. The function updates the @p absolute_offset
 * parameter to indicate the total offset from the beginning of the flash
 * memory region where the next credential is stored. This allows for consecutive
 * calls of the function in order to find all credentials stored for the
 * relying party.
 *
 * @param[in]  key       pointer to authenticator state
 * @param[in]   rp_id_hash pointer to hash of rp domain string
 * @param[in] absolute_offset pointer to a variable holding the total offset from the
 *                            start of the flash memory region used by the FIDO2
 *                            implementation. Updated by the function to indicate
 *                            the location of the next credential.
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_mem_read_rk_from_flash(ctap_resident_key_t *key,
                                                     const uint8_t *rp_id_hash,
                                                     uint32_t *absolute_offset);

/**
 * @brief Write resident credential to flash
 *
 * @param[in]  rk      pointer to resident credential
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_mem_write_rk_to_flash(ctap_resident_key_t *rk);

#ifdef __cplusplus
}
#endif
#endif /* FIDO2_CTAP_CTAP_MEM_H */
/** @} */
