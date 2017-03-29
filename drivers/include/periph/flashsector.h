/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_flashsector Flash sector driver
 * @ingroup     drivers_periph
 * @brief       Low-level flash sector interface
 *
 * @{
 * @file
 * @brief       Low-level flash sector peripheral driver interface
 *
 * This interface provides a very simple and straight forward way for writing
 * a MCU's internal flash. This interface is only capable of reading, verifying,
 * and writing complete flash sectors, it has no support for partial flash
 * access.
 * This enables for very slim and efficient implementations.
 *
 * A module for more fine-grained access of memory locations can easily be
 * programmed on top of this interface.
 *
 * TODO: add support for devices with dual bank memory, e.g. switch to dual bank
 * mode, if FLASHSECTOR_2ND_BANK_START is defined and specifies the address of
 * the first sector in the second bank
 *
 * @note        Flash memory has only a limited amount of erase cycles (mostly
 *              around 10K times), so using this interface in some kind of loops
 *              can damage you MCU!
 *
 * @author      Jannik Beyerstedt <jannik.beyerstedt@haw-hamburg.de>
 */

#ifndef FLASHSECTOR_H
#define FLASHSECTOR_H

#include <stdint.h>

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif


/* HAL Library macros */
#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))

/**
 * @brief   defines for flash programming parallelism
 */
#define FLASH_PSIZE_BYTE           0x00000000U
#define FLASH_PSIZE_HALF_WORD      0x00000100U
#define FLASH_PSIZE_WORD           0x00000200U
#define FLASH_PSIZE_DOUBLE_WORD    0x00000300U
#define CR_PSIZE_MASK              0xFFFFFCFFU

/**
 * @brief   flash keys form STM32F4xx
 */
#define RDP_KEY                  ((uint16_t)0x00A5)
#define FLASH_KEY1               0x45670123U
#define FLASH_KEY2               0xCDEF89ABU
#define FLASH_OPT_KEY1           0x08192A3BU
#define FLASH_OPT_KEY2           0x4C5D6E7FU

/**
 * @brief   Per default, we expect the internal flash to start at address 0
 */
#ifndef CPU_FLASH_BASE
#define CPU_FLASH_BASE      (0)
#endif

/**
 * @brief   Make sure the size of the smallest sector and the total bank size
 *          are defined. All sizes in KB.
 */
#ifndef FLASHSECTOR_SMALL_SECTOR
#error "periph/flashsector: FLASHSECTOR_SMALL_SECTOR not defined"
#endif
#ifndef FLASHSECTOR_BANK_SIZE
#error "periph/flashsector: FLASHSECTOR_BANK_SIZE not defined"
#endif

/**
 * @brief   Return values used in this interface
 */
enum {
    FLASHSECTOR_OK      =  0,     /**< everything succeeded */
    FLASHSECTOR_NOMATCH = -1      /**< sector differs from target data */
};

/**
 * @brief   Translate the given sector number into the sector's starting address
 *
 * @note    The given sector MUST be valid, otherwise the returned address points
 *          to an undefined memory location!
 *
 * @param[in] sector    sector number to get the address of
 *
 * @return              starting memory address of the given sector
 */
void *flashsector_addr(int sector);

/**
 * @brief   Translate the given address into the corresponding sector number
 *
 * The given address can be any address inside a sector.
 *
 * @note    The given address MUST be a valid flash address!
 *
 * @param[in] addr      address inside the targeted sector
 *
 * @return              sector containing the given address
 */
int flashsector_sector(void *addr);

/**
 * @brief   Write the given sector with the given data
 *
 * @param[in] sector    sector to write
 * @param[in] data      data to write to the sector, MUST be @p size byte.
 *                      Set to NULL for sector erase only.
 * @param[in] size      size of the data to write (in byte).
                        Ignored, if sector erase is requested
 */
void flashsector_write(int sector, void *data, int size);

/**
 * @brief   Write the given data to flash memory, but do not erase before
 *
 * @param[in] target    address to write data to
 * @param[in] data      data to write to the sector, MUST be @p size byte.
 *                      Set to NULL for sector erase only.
 * @param[in] size      size of the data to write (in byte).
                        Ignored, if sector erase is requested
 */
void flashsector_write_only(void *target, void *data, int size);

/**
 * @brief   Read the first part of given sector into the given memory location
 *
 * @param[in]  page     sector to read
 * @param[out] data     memory to write the page to, MUST be @p size byte
 * @param[in]  size     size of the memory location (in byte)
 */
void flashsector_read(int sector, void *data, int size);

/**
 * @brief   Verify the given sector against the given data
 *
 * @param[in] sector    sector to verify
 * @param[in] data      data to compare sector against, MUST be @p size byte
 * @param[in] size      size of the data to compare (in byte)
 *
 * @return              FLASHSECTOR_OK if data in the sector is identical to @p data
 * @return              FLASHSECTOR_NOMATCH if data and sector content diverge
 */
int flashsector_verify(int sector, void *data, int size);

/**
 * @brief   Write the given sector and verify the results
 *
 * This is a convenience function wrapping flashsector_write and flashsector_verify.
 *
 * @param[in] sector    sector to write
 * @param[in] data      data to write to the sector, MUST be @p size byte
 * @param[in] size      size of the data to compare (in byte)
 *
 * @return              FLASHSECTOR_OK on success
 * @return              FLASHSECTOR_NOMATCH if data and sector content diverge
 */
int flashsector_write_and_verify(int sector, void *data, int size);

#ifdef __cplusplus
}
#endif

#endif /* FLASHSECTOR_H */
/** @} */
