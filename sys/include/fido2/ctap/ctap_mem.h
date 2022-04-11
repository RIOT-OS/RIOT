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
 * @brief Write to flash memory
 *
 * @param[in] buf       buffer to write
 * @param[in] page      page to write to
 * @param[in] offset    offset from the start of the page (in bytes)
 * @param[in] len       number of bytes to write
 *
 * @return @ref ctap_status_codes_t
 */
int fido2_ctap_mem_write(const void *buf, uint32_t page, uint32_t offset, uint32_t len);

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
 * @brief Get flashpage number resident key with index @p rk_idx.
 *
 * @param[in] rk_idx    index of resident key
 *
 * @return page number if no error
 * @return -1 if @p rk_idx is invalid
 */
int fido2_ctap_mem_get_flashpage_number_of_rk(uint16_t rk_idx);

/**
 * @brief Get offset of resident key into flashpage where flashpage =
 * fido2_ctap_mem_get_flashpage_number_of_r(i)
 *
 * @param[in] rk_idx    index of resident key
 *
 * @return page number if no error
 * @return -1 if @p rk_idx is invalid
 */
int fido2_ctap_mem_get_offset_of_rk_into_flashpage(uint16_t rk_idx);

/**
 * @brief Get page number for storing authenticator state information
 *
 * @return page number
 */
unsigned fido2_ctap_mem_flash_page(void);

/**
 * @brief Get start page for storing resident keys
 *
 * @return page number
 */
unsigned fido2_ctap_mem_get_rk_start_page(void);

/**
 * @brief Erase all flashpages containing CTAP data
 *
 * @return @ref ctap_status_codes_t
 */
int fido2_ctap_mem_erase_flash(void);

#ifdef __cplusplus
}
#endif
#endif /* FIDO2_CTAP_CTAP_MEM_H */
/** @} */
