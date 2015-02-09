/*
 * Copyright (C) 2014 Frank Holtz
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_periph_flash Flash interface
 * @ingroup     driver_periph
 * @brief       Low-level access to internal flash
 *
 * @{
 * @file
 * @brief       Low-level access to internal flash
 *
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 */

#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>
#include "cpu-conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Available error codes
 *
 */
enum flash_errors {
    FLASH_ERROR_SUCCESS         = 0,  /**< Success */
    FLASH_ERROR_BROWNOUT        = 1,  /**< Voltage to low */
    FLASH_ERROR_FB_CONFIG       = 2,  /**< Forbidden by MCU configuration/flags */
    FLASH_ERROR_LOCKED          = 3,  /**< Page or flash is locked by other operation */
    FLASH_ERROR_TIMEOUT         = 4,  /**< Timeout */
    FLASH_ERROR_ALIGNMENT       = 5,  /**< Misalligned access */
    FLASH_ERROR_VERIFY          = 6,  /**< Data not written correctly */
    FLASH_ERROR_ADDR_RANGE      = 7,  /**< Address out of flash area */
};

/**
 * @brief Type definition for page size
 *
 * This allows page sizes >64k in the future
 *
 */
#ifndef FLASH_PAGE_SIZE
#error Flash is not suppored or not implemented for this CPU
#elif FLASH_PAGE_SIZE>=256
typedef uint16_t flash_page_size_t;
#else
typedef uint8_t flash_page_size_t;
#endif

/**
 * @brief Type definition for page number
 *
 * This allows more then 64k pages in the future
 *
 */
#if FLASH_NUM_PAGES>=256
typedef uint16_t flash_page_number_t;
#else
typedef uint8_t flash_page_number_t;
#endif

/**
 * @brief Type definition for data word for alignment
 */
#if FLASH_WRITE_ALIGN == 1
typedef uint8_t flash_data_word_t;
#elif FLASH_WRITE_ALIGN == 2
typedef uint16_t flash_data_word_t;
#elif FLASH_WRITE_ALIGN == 4
typedef uint32_t flash_data_word_t;
#elif FLASH_WRITE_ALIGN == 8
typedef uint64_t flash_data_word_t;
#endif

/**
 * @brief Initialize flash memory access
 *
 * If needed flash memory initialization can be performed.
 *
 * @return	Error Code
 */
uint8_t flash_init(void);

/**
 * @brief Translates an address to page number
 *
 * This function is needed to calculate the page number
 * for erase operation
 *
 * @param[in]  address Pointer/Memory address
 * @param[out] page_offset  Address location in page or NULL
 *
 * @return             Flash page number
 */
flash_page_number_t flash_get_page_number(void *address, flash_page_size_t *page_offset);

/**
 * @brief Translates an page number to pointer
 *
 * This function is needed to calculate the memory address
 * of an flash page
 *
 * @param[in] page     Page number
 *
 * @return             Pointer to page or NULL
 */
void *flash_get_address(flash_page_number_t page);

/**
 * @brief Write data to flash
 *
 * This function works like memcpy. It can be used to write
 * a part of a flash page or an single flash page or multiple
 * flash pages at once. Written data are reread to check if
 * data are stored correctly. Optionally a written word can
 * modified to set only changed bits to reach more write cycles.
 * 
 * This function can be used with aligned and unaligned addresses.
 *
 * The return code should by checked to handle errors.
 *
 * @param[in] dest   Address of flash memory page
 * @param[in] src    Address of source data
 * @param[in] n      Number of bytes to write
 *
 * @return    FLASH_ERROR_SUCCESS
 * @return    FLASH_ERROR_BROWNOUT
 * @return    FLASH_ERROR_FB_CONFIG
 * @return    FLASH_ERROR_LOCKED
 * @return    FLASH_ERROR_TIMEOUT
 * @return    FLASH_ERROR_VERIFY
 * @return    FLASH_ERROR_ADDR_RANGE
 */
uint8_t flash_memcpy(void *dest, const void *src, size_t n);

/**
 * @brief Write data to flash without checks
 *
 * This function works like memcpy. It can be used to write
 * a part of a flash page or an single flash page or multiple
 * flash pages at once. There are no address or correct write checks,
 * so write to protected flash regions are possible.
 * 
 * To support all plattforms do an alignment of src and dest addresses.
 * 
 * This function is used by tests/periph_flash to check if
 * alignment check is implemented correctly
 *
 * The return code should by checked to handle errors.
 *
 * @param[in] dest   Aligned address of flash memory page
 * @param[in] src    Aligned address of source data
 * @param[in] n      Aligned number of bytes to write
 *
 * @return    FLASH_ERROR_SUCCESS
 * @return    FLASH_ERROR_BROWNOUT
 * @return    FLASH_ERROR_FB_CONFIG
 * @return    FLASH_ERROR_LOCKED
 * @return    FLASH_ERROR_TIMEOUT
 * @return    FLASH_ERROR_ALIGNMENT
 */
uint8_t flash_memcpy_fast(void *dest, const void *src, size_t n);


/**
 * @brief Erase an flash page by page number
 *
 * @param[in] page     Page number
 *
 * @return    FLASH_ERROR_SUCCESS
 * @return    FLASH_ERROR_BROWNOUT
 * @return    FLASH_ERROR_FB_CONFIG
 * @return    FLASH_ERROR_LOCKED
 * @return    FLASH_ERROR_TIMEOUT
 * @return    FLASH_ERROR_ADDR_RANGE
 */
uint8_t flash_erase_page(flash_page_number_t page);

/**
 * @brief Erase an flash page by address
 *
 * @param[in] address  Page address
 *
 * @return    FLASH_ERROR_SUCCESS
 * @return    FLASH_ERROR_BROWNOUT
 * @return    FLASH_ERROR_FB_CONFIG
 * @return    FLASH_ERROR_LOCKED
 * @return    FLASH_ERROR_TIMEOUT
 * @return    FLASH_ERROR_ADDR_RANGE
 */
uint8_t flash_erase_page_by_address(void *address);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_H */
/** @} */
