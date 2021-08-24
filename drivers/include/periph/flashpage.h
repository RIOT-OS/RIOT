/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 Inria
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
 * # (Low-) Power Implications
 *
 * The flashpage driver implementation **should** make sure, that the CPU uses
 * no additional energy while the flashpage driver is inactive. This means, that
 * any particular CPU peripherals used for reading and writing flash pages
 * **should** be disabled before the read and write functions return.
 *
 * If an implementation puts the calling thread to sleep for a duration of time,
 * the implementation might need to take care of blocking certain power modes.
 *
 * @{
 * @file
 * @brief       Low-level flash page peripheral driver interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Federico Pellegrin <fede@evolware.org>
 *
 */

#ifndef PERIPH_FLASHPAGE_H
#define PERIPH_FLASHPAGE_H

#include <stddef.h>
#include <stdint.h>

#include "cpu_conf.h"
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
 * @def     FLASHPAGE_WRITE_BLOCK_SIZE
 *
 * @brief   For raw writings to the flash, this constant must define the
 *          minimum write length allowed by the MCU.
 */
#ifdef DOXYGEN
#define FLASHPAGE_WRITE_BLOCK_SIZE
#endif

/**
 * @def     FLASHPAGE_WRITE_BLOCK_ALIGNMENT
 *
 * @brief   The buffers to be written to the flash MUST be aligned, as well as
 *          the address on which the buffer is written to the flash. This variable
 *          must be defined for that purpose, according to the MCU align
 *          requirements.
 */
#ifdef DOXYGEN
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT
#endif

/**
 * @def FLASHPAGE_ERASE_STATE
 *
 * @brief   State of an erased byte in memory
 */
#if defined(DOXYGEN) || !defined(FLASHPAGE_ERASE_STATE)
#define FLASHPAGE_ERASE_STATE           (0xFFU)
#endif

/**
 * @def     PERIPH_FLASHPAGE_CUSTOM_PAGESIZES
 *
 * @brief   Defined to signal that the peripheral has non-uniform flash page
 *          sizes. These devices do not define FLASHPAGE_SIZE and do not
 *          implement the pagewise api.
 */
#ifdef DOXYGEN
#define PERIPH_FLASHPAGE_CUSTOM_PAGESIZES
#endif

/**
 * @def     PERIPH_FLASHPAGE_NEEDS_FLASHPAGE_ADDR
 *
 * @brief   If non-uniform flash page sizes are required, defined to signal
 *          that the peripheral does not implement a custom @ref flashpage_addr
 *          function and instead relies on the generic helper function that
 *          relies on @ref flashpage_size.
 */
#ifdef DOXYGEN
#define PERIPH_FLASHPAGE_NEEDS_FLASHPAGE_ADDR
#endif

/**
 * @def     PERIPH_FLASHPAGE_NEEDS_FLASHPAGE_PAGE
 *
 * @brief   If non-uniform flash page sizes are required, defined to signal
 *          that the peripheral does not implement a custom @ref flashpage_page
 *          function and instead relies on the generic helper function that
 *          relies on @ref flashpage_size.
 */
#ifdef DOXYGEN
#define PERIPH_FLASHPAGE_NEEDS_FLASHPAGE_PAGE
#endif

/**
 * @brief   Return values used in this interface
 */
enum {
    FLASHPAGE_OK      =  0,     /**< everything succeeded */
    FLASHPAGE_NOMATCH = -1      /**< page differs from target data */
};

#if !defined(PERIPH_FLASHPAGE_CUSTOM_PAGESIZES) || defined(DOXYGEN)
/**
 * @def FLASHPAGE_SIZE
 *
 * @brief   Make sure the page size and the number of pages is defined
 */
#ifndef FLASHPAGE_SIZE
#error "periph/flashpage: FLASHPAGE_SIZE not defined"
#endif
#ifndef FLASHPAGE_NUMOF
#error "periph/flashpage: FLASHPAGE_NUMOF not defined"
#endif

/**
 * @brief   Get the page size of the given page number
 *
 * @param[in] page      page number to get the size for
 *
 * @return              Page size of the given page
 */
static inline size_t flashpage_size(unsigned page)
{
    (void)page;
    return FLASHPAGE_SIZE;
}

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
static inline void *flashpage_addr(unsigned page)
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
static inline unsigned flashpage_page(void *addr)
{
    return (((intptr_t)addr - CPU_FLASH_BASE) / FLASHPAGE_SIZE);
}

#else

/* Bare prototypes for the above functions. See above for the documentation */
size_t flashpage_size(unsigned page);
void *flashpage_addr(unsigned page);
unsigned flashpage_page(void *addr);

#endif

/**
 * @brief   Erase the given page
 *
 * @param[in] page      Page to erase
 */
void flashpage_erase(unsigned page);

/**
 * @brief   Write the given page with the given data
 *
 * @param[in] page      page to write
 * @param[in] data      data to write to the page, MUST be FLASHPAGE_SIZE
 *                      byte. Set to NULL for page erase only.
 */
void flashpage_write_page(unsigned page, const void *data);

/**
 * @brief   Write any number of data bytes to a given location in the
 *          flash memory
 *
 * @warning Make sure the targeted memory area is erased before calling
 *          this function
 *
 * Both target address and data address must be aligned to
 * FLASHPAGE_BLOCK_ALIGN. @p len must be a multiple of FLASHPAGE_WRITE_BLOCK_SIZE.
 * This function doesn't erase any area in flash, thus be sure the targeted
 * memory area is erased before writing on it (using the flashpage_write function).
 *
 * @param[in] target_addr   address in flash to write to. MUST be aligned
 *                          to FLASHPAGE_WRITE_BLOCK_ALIGNMENT.
 * @param[in] data          data to write to the address. MUST be aligned
 *                          to FLASHPAGE_WRITE_BLOCK_ALIGNMENT.
 * @param[in] len           length of the data to be written. It MUST be
 *                          multiple of FLASHPAGE_WRITE_BLOCK_SIZE. Also,
 *                          ensure it doesn't exceed the actual flash
 *                          memory size.
 */
void flashpage_write(void *target_addr, const void *data, size_t len);

/**
 * @brief   Read the given page into the given memory location
 *
 * @param[in]  page     page to read
 * @param[out] data     memory to write the page to, MUST be FLASHPAGE_SIZE
 *                      byte
 */
void flashpage_read(unsigned page, void *data);

/**
 * @brief   Verify the given page against the given data
 *
 * @param[in] page      page to verify
 * @param[in] data      data to compare page against, MUST be FLASHPAGE_SIZE
 *                      byte of data
 *
 * @return              FLASHPAGE_OK if data in the page is identical to @p data
 * @return              FLASHPAGE_NOMATCH if data and page content diverge
 */
int flashpage_verify(unsigned page, const void *data);

/**
 * @brief   Write the given page and verify the results
 *
 * This is a convenience function wrapping flashpage_write and flashpage_verify.
 *
 * @param[in] page      page to write
 * @param[in] data      data to write to the page, MUST be FLASHPAGE_SIZE
 *                      byte.
 *
 * @return              FLASHPAGE_OK on success
 * @return              FLASHPAGE_NOMATCH if data and page content diverge
 */
int flashpage_write_and_verify(unsigned page, const void *data);

/**
  *  @brief  Functions to support additional RWWEE flash section
  *
  *  Some CPUs (ie. SAML21, SAML11) have an additional separated flash section
  *  named RWWEE (Read While Write EEPROM Emulation) that can be accessed from
  *  RIOT in a similar fashion but with a set of functions ending with a
  *  _rwwee name variant.
  *
  *  The support of this feature is determined by the FLASHPAGE_RWWEE_NUMOF
  *  define that, if defined, contains the number of RWWEE sectors in the CPU.
  *  This is defined in the CPU specific headers (ie. for the ATMEL SAM family
  *  in cpu/sam0_common/include/cpu_conf.h)
  */
#ifdef FLASHPAGE_RWWEE_NUMOF

/**
 * @brief   Translate the given RWEE page number into the page's starting address
 *
 * @note    The given page MUST be valid, otherwise the returned address points
 *          to an undefined memory location!
 *
 * @param[in] page      RWWEE page number to get the address of
 *
 * @return              starting memory address of the given RWWEE page
 */
static inline void *flashpage_rwwee_addr(unsigned page)
{
    return (void *)(CPU_FLASH_RWWEE_BASE + (page * FLASHPAGE_SIZE));
}

/**
 * @brief   Translate the given address into the corresponding RWWEE page number
 *
 * The given address can be any address inside a RWWEE page.
 *
 * @note    The given address MUST be a valid RWWEE flash address!
 *
 * @param[in] addr      address inside the targeted RWWEE page
 *
 * @return              RWWEE page containing the given address
 */
static inline int flashpage_rwwee_page(void *addr)
{
    return (int)(((int)addr - CPU_FLASH_RWWEE_BASE) / FLASHPAGE_SIZE);
}

/**
 * @brief   Write the given RWWEE page with the given data
 *
 * @param[in] page      RWWEE page to write
 * @param[in] data      data to write to the RWWEE page, MUST be FLASHPAGE_SIZE
 *                      byte. Set to NULL for RWWEE page erase only.
 */
void flashpage_rwwee_write_page(unsigned page, const void *data);

/**
 * @brief   Write any number of data bytes to a given location in the
 *          RWWEE flash memory
 *
 * @warning Make sure the targeted memory area is erased before calling
 *          this function
 *
 * Both target address and data address must be aligned to
 * FLASHPAGE_BLOCK_ALIGN. @p len must be a multiple of FLASHPAGE_WRITE_BLOCK_SIZE.
 * This function doesn't erase any area in flash, thus be sure the targeted
 * memory area is erased before writing on it (using the flashpage_write function).
 *
 * @param[in] target_addr   RWWEE address in flash to write to. MUST be aligned
 *                          to FLASHPAGE_WRITE_BLOCK_ALIGNMENT.
 * @param[in] data          data to write to the address. MUST be aligned
 *                          to FLASHPAGE_WRITE_BLOCK_ALIGNMENT.
 * @param[in] len           length of the data to be written. It MUST be
 *                          multiple of FLASHPAGE_WRITE_BLOCK_SIZE. Also,
 *                          ensure it doesn't exceed the actual RWWEE flash
 *                          memory size.
 */
void flashpage_rwwee_write(void *target_addr, const void *data, size_t len);

/**
 * @brief   Read the given RWWEE page into the given memory location
 *
 * @param[in]  page     RWWEE page to read
 * @param[out] data     memory to write the RWWEE page to, MUST be FLASHPAGE_SIZE
 *                      byte
 */
void flashpage_rwwee_read(unsigned page, void *data);

/**
 * @brief   Verify the given RWWEE page against the given data
 *
 * @param[in] page      RWWEE page to verify
 * @param[in] data      data to compare RWWEE page against, MUST be FLASHPAGE_SIZE
 *                      byte of data
 *
 * @return              FLASHPAGE_OK if data in the RWWEE page is identical to @p data
 * @return              FLASHPAGE_NOMATCH if data and RWWEE page content diverge
 */
int flashpage_rwwee_verify(unsigned page, const void *data);

/**
 * @brief   Write the given RWWEE page and verify the results
 *
 * This is a convenience function wrapping flashpage_rwwee_write and
 * flashpage_rwwee_verify.
 *
 * @param[in] page      RWWEE page to write
 * @param[in] data      data to write to the RWWEE page, MUST be FLASHPAGE_SIZE
 *                      byte.
 *
 * @return              FLASHPAGE_OK on success
 * @return              FLASHPAGE_NOMATCH if data and RWWEE page content diverge
 */
int flashpage_rwwee_write_and_verify(unsigned page, const void *data);

#endif /* FLASHPAGE_RWWEE_NUMOF */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_FLASHPAGE_H */
/** @} */
