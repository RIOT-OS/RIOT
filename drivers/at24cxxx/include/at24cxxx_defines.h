/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_at24cxxx
 * @{
 *
 * @file
 * @brief       Constants for various I2C EEPROM devices.
 *
 * All the devices listed below are accessible as pseudomodules.
 *
 * @note Even though the library is called "AT24CXXX", the support for
 * I2C EEPROMs is not limited to Atmel/Microchip devices.
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Delay between two poll attempts
 */
#define AT24CXXX_POLL_DELAY_US          (1000UL)
/**
 * @brief   Clear byte
 */
#define AT24CXXX_CLEAR_BYTE             (0x00)
/**
 * @brief   AT24CXXX default device address
 *
 * Different AT24CXXX types may have a different address byte
 * format. Some may include a portion of the data word address.
 * Some may have a 0, 1, 2 or 3 bit wide address space.
 * But all types have a 7 bit I2C address which starts with
 * 1010. [1, 0, 1, 0, ?, ?, ?, r/w]
 *        \__7 bit address__/
 */
#define AT24CXXX_DEF_DEV_ADDR           (0x50)

/**
 * @name AT24C01A constants
 * @{
 */
/**
 * @brief   128 byte memory
 */
#define AT24C01A_EEPROM_SIZE            (128U)
/**
 * @brief   16 pages of 8 bytes each
 */
#define AT24C01A_PAGE_SIZE              (8U)
/**
 * @brief Delay to complete write operation
 */
#define AT24C01A_PAGE_WRITE_DELAY_US    (5000U)
/**
 * @brief Number of poll attempts
 */
#define AT24C01A_MAX_POLLS              (1 + (AT24C01A_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24C02 constants
 * @{
 */
/**
 * @brief   256 byte memory
 */
#define AT24C02_EEPROM_SIZE             (256U)
/**
 * @brief   32 pages of 8 bytes
 */
#define AT24C02_PAGE_SIZE               (8U)
/**
 * @brief   Delay to complete write operation
 */
#define AT24C02_PAGE_WRITE_DELAY_US     (5000U)
/**
 * @brief   Number of poll attempts
 */
#define AT24C02_MAX_POLLS               (1 + (AT24C02_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24C04 constants
 * @{
 */
/**
 * @brief   512 byte memory
 */
#define AT24C04_EEPROM_SIZE             (512U)
/**
 * @brief   32 pages of 16 bytes each
 */
#define AT24C04_PAGE_SIZE               (16U)
/**
 * @brief   Delay to complete write operation
 */
#define AT24C04_PAGE_WRITE_DELAY_US     (5000U)
/**
 * @brief   Number of poll attempts
 */
#define AT24C04_MAX_POLLS               (1 + (AT24C04_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24C08A constants
 * @{
 */
/**
 * @brief   1 kiB memory
 */
#define AT24C08A_EEPROM_SIZE            (1024U)
/**
 * @brief   64 pages of 16 bytes each
 */
#define AT24C08A_PAGE_SIZE              (16U)
/**
 * @brief   Delay to complete write operation
 */
#define AT24C08A_PAGE_WRITE_DELAY_US    (5000U)
/**
 * @brief Number of poll attempts
 */
#define AT24C08A_MAX_POLLS              (1 + (AT24C08A_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24C16A constants
 * @{
 */
/**
 * @brief 2 kiB memory
 */
#define AT24C16A_EEPROM_SIZE            (2048U)
/**
 * @brief   128 pages of 16 bytes each
 */
#define AT24C16A_PAGE_SIZE              (16U)
/**
 * @brief Delay to complete write operation
 */
#define AT24C16A_PAGE_WRITE_DELAY_US    (5000U)
/**
 * @brief Number of poll attempts
 */
#define AT24C16A_MAX_POLLS              (1 + (AT24C16A_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24C32 constants
 * @{
 */
/**
 * @brief   4 kiB memory
 */
#define AT24C32_EEPROM_SIZE             (4096U)
/**
 * @brief   256 pages of 32 bytes each
 */
#define AT24C32_PAGE_SIZE               (32U)
/**
 * @brief   Delay to complete write operation
 */
#define AT24C32_PAGE_WRITE_DELAY_US     (10000U)
/**
 * @brief   Number of poll attempts
 */
#define AT24C32_MAX_POLLS               (1 + (AT24C32_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24C64 constants
 * @{
 */
/**
 * @brief 8 kiB memory
 */
#define AT24C64_EEPROM_SIZE             (8192U)
/**
 * @brief   256 pages of 32 bytes each
 */
#define AT24C64_PAGE_SIZE               (32U)
/**
 * @brief   Delay to complete write operation
 */
#define AT24C64_PAGE_WRITE_DELAY_US     (10000U)
/**
 * @brief   Number of poll attempts
 */
#define AT24C64_MAX_POLLS               (1 + (AT24C64_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24C128 constants
 * @{
 */
/**
 * @brief 16 kiB memory
 */
#define AT24C128_EEPROM_SIZE            (16384U)
/**
 * @brief   256 pages of 64 bytes each
 */
#define AT24C128_PAGE_SIZE              (64U)
/**
 * @brief   Delay to complete write operation
 */
#define AT24C128_PAGE_WRITE_DELAY_US    (5000U)
/**
 * @brief   Number of poll attempts
 */
#define AT24C128_MAX_POLLS              (1 + (AT24C128_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24C256 constants
 * @{
 */
/**
 * @brief 32 kiB memory
 */
#define AT24C256_EEPROM_SIZE            (32768U)
/**
 * @brief   512 pages of 64 bytes each
 */
#define AT24C256_PAGE_SIZE              (64U)
/**
 * @brief   Delay to complete write operation
 */
#define AT24C256_PAGE_WRITE_DELAY_US    (5000U)
/**
 * @brief   Number of poll attempts
 */
#define AT24C256_MAX_POLLS              (1 + (AT24C256_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24C512 constants
 * @{
 */
/**
 * @brief 64 kiB memory
 */
#define AT24C512_EEPROM_SIZE            (65536U)
/**
 * @brief   512 pages of 128 bytes each
 */
#define AT24C512_PAGE_SIZE              (128U)
/**
 * @brief   Delay to complete write operation
 */
#define AT24C512_PAGE_WRITE_DELAY_US    (5000U)
/**
 * @brief   Number of poll attempts
 */
#define AT24C512_MAX_POLLS              (1 + (AT24C512_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24CS04 constants
 * @{
 */
/**
 * @brief 512 Byte memory
 */
#define AT24CS04_EEPROM_SIZE            (512U)
/**
 * @brief   32 pages of 16 bytes each
 */
#define AT24CS04_PAGE_SIZE              (16U)
/**
 * @brief   Delay to complete write operation
 */
#define AT24CS04_PAGE_WRITE_DELAY_US    (5000U)
/**
 * @brief   Number of poll attempts
 */
#define AT24CS04_MAX_POLLS              (1 + (AT24CS04_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24CS08 constants
 * @{
 */
/**
 * @brief 1 kiB memory
 */
#define AT24CS08_EEPROM_SIZE            (1024U)
/**
 * @brief   64 pages of 16 bytes each
 */
#define AT24CS08_PAGE_SIZE              (16U)
/**
 * @brief   Delay to complete write operation
 */
#define AT24CS08_PAGE_WRITE_DELAY_US    (5000U)
/**
 * @brief   Number of poll attempts
 */
#define AT24CS08_MAX_POLLS              (1 + (AT24CS08_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24C1024 constants
 * @{
 */
/**
 * @brief 128 kiB memory
 */
#define AT24C1024_EEPROM_SIZE           (131072U)
/**
 * @brief   512 pages of 256 bytes each
 */
#define AT24C1024_PAGE_SIZE             (256U)
/**
 * @brief   Delay to complete write operation
 */
#define AT24C1024_PAGE_WRITE_DELAY_US   (5000U)
/**
 * @brief   Number of poll attempts
 */
#define AT24C1024_MAX_POLLS             (1 + (AT24C1024_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name AT24MAC402/602 constants
 * @{
 */
/**
 * @brief   256 byte memory
 */
#define AT24MAC_EEPROM_SIZE             (256U)
/**
 * @brief   16 pages of 16 bytes each
 */
#define AT24MAC_PAGE_SIZE               (16U)
/**
 * @brief   Delay to complete write operation
 */
#define AT24MAC_PAGE_WRITE_DELAY_US     (5000U)
/**
 * @brief   Number of poll attempts
 */
#define AT24MAC_MAX_POLLS               (1 + (AT24MAC_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name M24C01 constants
 * @{
 */
/**
 * @brief   128 byte memory
 */
#define M24C01_EEPROM_SIZE              (128U)
/**
 * @brief   16 pages of 16 bytes each
 */
#define M24C01_PAGE_SIZE                (16U)
/**
 * @brief   Delay to complete write operation
 */
#define M24C01_PAGE_WRITE_DELAY_US      (5000U)
/**
 * @brief   Number of poll attempts
 */
#define M24C01_MAX_POLLS                (1 + (M24C01_PAGE_WRITE_DELAY_US \
                                         / AT24CXXX_POLL_DELAY_US))
/** @} */

/**
 * @name Set constants depending on module
 * @{
 */
#if IS_USED(MODULE_AT24C1024)
#define AT24CXXX_EEPROM_SIZE            (AT24C1024_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24C1024_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24C1024_MAX_POLLS)
#elif IS_USED(MODULE_AT24C512)
#define AT24CXXX_EEPROM_SIZE            (AT24C512_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24C512_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24C512_MAX_POLLS)
#elif IS_USED(MODULE_AT24C256)
#define AT24CXXX_EEPROM_SIZE            (AT24C256_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24C256_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24C256_MAX_POLLS)
#elif IS_USED(MODULE_AT24C128)
#define AT24CXXX_EEPROM_SIZE            (AT24C128_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24C128_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24C128_MAX_POLLS)
#elif IS_USED(MODULE_AT24C64)
#define AT24CXXX_EEPROM_SIZE            (AT24C64_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24C64_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24C64_MAX_POLLS)
#elif IS_USED(MODULE_AT24C32)
#define AT24CXXX_EEPROM_SIZE            (AT24C32_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24C32_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24C32_MAX_POLLS)
#elif IS_USED(MODULE_AT24C16A)
#define AT24CXXX_EEPROM_SIZE            (AT24C16A_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24C16A_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24C16A_MAX_POLLS)
#elif IS_USED(MODULE_AT24C08A)
#define AT24CXXX_EEPROM_SIZE            (AT24C08A_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24C08A_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24C08A_MAX_POLLS)
#elif IS_USED(MODULE_AT24C04)
#define AT24CXXX_EEPROM_SIZE            (AT24C04_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24C04_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24C04_MAX_POLLS)
#elif IS_USED(MODULE_AT24C02)
#define AT24CXXX_EEPROM_SIZE            (AT24C02_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24C02_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24C02_MAX_POLLS)
#elif IS_USED(MODULE_AT24C01A)
#define AT24CXXX_EEPROM_SIZE            (AT24C01A_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24C01A_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24C01A_MAX_POLLS)
#elif IS_USED(MODULE_AT24CS04)
#define AT24CXXX_EEPROM_SIZE            (AT24CS04_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24CS04_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24CS04_MAX_POLLS)
#elif IS_USED(MODULE_AT24CS08)
#define AT24CXXX_EEPROM_SIZE            (AT24CS08_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24CS08_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24CS08_MAX_POLLS)
#elif IS_USED(MODULE_AT24MAC)
#define AT24CXXX_EEPROM_SIZE            (AT24MAC_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (AT24MAC_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (AT24MAC_MAX_POLLS)
#elif IS_USED(MODULE_M24C01)
#define AT24CXXX_EEPROM_SIZE            (M24C01_EEPROM_SIZE)
#define AT24CXXX_PAGE_SIZE              (M24C01_PAGE_SIZE)
#define AT24CXXX_MAX_POLLS              (M24C01_MAX_POLLS)
#else /* minimal */
#define AT24CXXX_EEPROM_SIZE            (128U)  /**< EEPROM size */
#define AT24CXXX_PAGE_SIZE              (4U)    /**< page size */
#define AT24CXXX_MAX_POLLS              (6U)    /**< maximum poll attempts */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
