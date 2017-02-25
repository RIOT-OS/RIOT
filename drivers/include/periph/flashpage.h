/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_flashpage Flash page driver
 * @ingroup     drivers_periph
 * @brief       Low-level flash page interface
 *
 * @{
 * @file
 * @brief       Low-level flash page peripheral driver interface
 *
 * This interface provides a very simple and straight forward way for writing
 * a MCU's internal flash. This interface is only capable of reading, verifying,
 * and writing complete flash pages, it has no support for partial flash access.
 * This enables for very slim and efficient implementations.
 *
 * A module for more fine-grained access of memory locations can easily be
 * programmed on top of this interface.
 *
 * @note        Flash memory has only a limited amount of erase cycles (mostly
 *              around 10K times), so using this interface in some kind of loops
 *              can damage you MCU!
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef FLASHPAGE_H
#define FLASHPAGE_H

#include <stdint.h>

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Per default, we expect the internal flash to start at address 0
 */
#ifndef CPU_FLASH_BASE
#define CPU_FLASH_BASE      (0)
#endif

/**
 * @brief   Make sure the page size and the number of pages is defined
 */
#ifndef FLASHPAGE_SIZE
#error "periph/flashpage: FLASHPAGE_SIZE not defined"
#endif
#ifndef FLASHPAGE_NUMOF
#error "periph/flashpage: FLASHPAGE_NUMOF not defined"
#endif

/**
 * @brief   Return values used in this interface
 */
enum {
    FLASHPAGE_OK      =  0,     /**< everything succeeded */
    FLASHPAGE_NOMATCH = -1      /**< page differs from target data */
};

/**
 * @brief   Translate the given page number into the page's starting address
 *
 * @note    The given page MUST be valid, otherwise the returned address points
 *          to an undefined memory location!
 *
 * @param[in] page      page number to get the address of
 *
 * @return              starting memory address of the given page
 */
static inline void *flashpage_addr(int page)
{
    return (void *)(CPU_FLASH_BASE + (page * FLASHPAGE_SIZE));
}

/**
 * @brief   Translate the given address into the corresponding page number
 *
 * The given address can be any address inside a page.
 *
 * @note    The given address MUST be a valid flash address!
 *
 * @param[in] addr      address inside the targeted page
 *
 * @return              page containing the given address
 */
static inline int flashpage_page(void *addr)
{
    return (int)(((int)addr - CPU_FLASH_BASE) / FLASHPAGE_SIZE);
}

/**
 * @brief   Write the given page with the given data
 *
 * @param[in] page      page to write
 * @param[in] data      data to write to the page, MUST be @p FLASHPAGE_SIZE
 *                      byte. Set to NULL for page erase only.
 */
void flashpage_write(int page, void *data);

/**
 * @brief   Read the given page into the given memory location
 *
 * @param[in]  page     page to read
 * @param[out] data     memory to write the page to, MUST be @p FLASHPAGE_SIZE
 *                      byte
 */
void flashpage_read(int page, void *data);

/**
 * @brief   Verify the given page against the given data
 *
 * @param[in] page      page to verify
 * @param[in] data      data to compare page against, MUST be @p FLASHPAGE_SIZE
 *                      byte of data
 *
 * @return              FLASHPAGE_OK if data in the page is identical to @p data
 * @return              FLASHPAGE_NOMATCH if data and page content diverge
 */
int flashpage_verify(int page, void *data);

/**
 * @brief   Write the given page and verify the results
 *
 * This is a convenience function wrapping flashpage_write and flashpage_verify.
 *
 * @param[in] page      page to write
 * @param[in] data      data to write to the page, MUST be @p FLASHPAGE_SIZE
 *                      byte.
 *
 * @return              FLASHPAGE_OK on success
 * @return              FLASHPAGE_NOMATCH if data and page content diverge
 */
int flashpage_write_and_verify(int page, void *data);

#ifdef __cplusplus
}
#endif

#endif /* FLASHPAGE_H */
/** @} */
