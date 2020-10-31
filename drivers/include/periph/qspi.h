/*
 * Copyright (c) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_qspi Quad SPI peripheral
 * @ingroup     drivers_periph
 * @brief       Low-level QSPI peripheral driver
 */

#ifndef PERIPH_QSPI_H
#define PERIPH_QSPI_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Serial Flash JEDEC commands
 */
typedef enum {
    SFLASH_CMD_READ = 0x03,                 /**< Single Read   */
    SFLASH_CMD_FAST_READ = 0x0B,            /**< Fast Read     */
    SFLASH_CMD_QUAD_READ = 0x6B,            /**< 1 line address, 4 line data         */

    SFLASH_CMD_READ_JEDEC_ID = 0x9f,        /**< Read Chip ID  */

    SFLASH_CMD_PAGE_PROGRAM = 0x02,         /**< 1 line address, 1 line data         */
    SFLASH_CMD_QUAD_PAGE_PROGRAM = 0x32,    /**< 1 line address, 4 line data         */

    SFLASH_CMD_READ_STATUS = 0x05,          /**< Read first status register          */
    SFLASH_CMD_READ_STATUS2 = 0x35,         /**< Read second status register         */

    SFLASH_CMD_WRITE_STATUS = 0x01,         /**< Write first status register         */
    SFLASH_CMD_WRITE_STATUS2 = 0x31,        /**< Write second status register        */

    SFLASH_CMD_ENABLE_RESET = 0x66,         /**< Must be issued before reset command */
    SFLASH_CMD_RESET = 0x99,                /**< Reset device state                  */

    SFLASH_CMD_WRITE_ENABLE = 0x06,         /**< Must be issued before write command */
    SFLASH_CMD_WRITE_DISABLE = 0x04,        /**< Revoke Write Enable                 */

    SFLASH_CMD_ERASE_SECTOR = 0x20,         /**< sector (4k) erase                   */
    SFLASH_CMD_ERASE_BLOCK = 0xD8,          /**< block (64k) erase                   */
    SFLASH_CMD_ERASE_CHIP = 0xC7,           /**< whole chip erase                    */

    SFLASH_CMD_4_BYTE_ADDR = 0xB7,          /**< enable 32 bit addressing */
    SFLASH_CMD_3_BYTE_ADDR = 0xE9,          /**< enable 24 bit addressing */
} spi_flash_cmd_t;

/**
 * @brief   Default SPI device access macro
 */
#ifndef QSPI_DEV
#define QSPI_DEV(x)     (x)
#endif

/**
 * @brief   Define global value for undefined QSPI device
 */
#ifndef QSPI_UNDEF
#define QSPI_UNDEF      (UINT_MAX)
#endif

/**
 * @brief   Default type for QSPI devices
 */
#ifndef HAVE_QSPI_T
typedef unsigned int qspi_t;
#endif

/**
 * @brief   Available QSPI modes, defining the configuration of clock polarity
 *          and clock phase
 *
 * RIOT is using the mode numbers as commonly defined by most vendors
 * (https://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus#Mode_numbers):
 *
 * - MODE_0: CPOL=0, CPHA=0 - The first data bit is sampled by the receiver on
 *           the first SCK rising SCK edge (this mode is used most often).
 * - MODE_1: CPOL=0, CPHA=1 - The first data bit is sampled by the receiver on
 *           the second rising SCK edge.
 * - MODE_2: CPOL=1, CPHA=0 - The first data bit is sampled by the receiver on
 *           the first falling SCK edge.
 * - MODE_3: CPOL=1, CPHA=1 - The first data bit is sampled by the receiver on
 *           the second falling SCK edge.
 */
#ifndef HAVE_QSPI_MODE_T
typedef enum {
    QSPI_MODE_0 = 0,        /**< CPOL=0, CPHA=0 */
    QSPI_MODE_1,            /**< CPOL=0, CPHA=1 */
    QSPI_MODE_2,            /**< CPOL=1, CPHA=0 */
    QSPI_MODE_3             /**< CPOL=1, CPHA=1 */
} qspi_mode_t;
#endif

/**
 * @brief   Standard QSPI erase block sizes
 */
#ifndef HAVE_QSPI_ERASE_SIZE_T
typedef enum {
    QSPI_ERASE_4K   = SFLASH_CMD_ERASE_SECTOR,
    QSPI_ERASE_64K  = SFLASH_CMD_ERASE_BLOCK,
    QSPI_ERASE_CHIP = SFLASH_CMD_ERASE_CHIP,
} qspi_erase_size_t;
#endif

/**
 * @brief   Configuration Options for QSPI
 * @{
 */
#define QSPI_FLAG_1BIT          (0x0)   /**< single data line           */
#define QSPI_FLAG_2BIT          (0x1)   /**< two parallel data lines    */
#define QSPI_FLAG_4BIT          (0x2)   /**< four parallel data lines   */
#define QSPI_FLAG_8BIT          (0x3)   /**< eight parallel data lines  */

#define QSPI_FLAG_BIT_MASK      (0x3)   /**< mask to get data lines from flags */
#define QSPI_FLAG_BIT(flags)    ((flags) & QSPI_FLAG_BIT_MASK)

#define QSPI_FLAG_DDR           (0x4)   /**< use Double Data Rate mode  */

#define QSPI_FLAG_ADDR_32BIT    (0x8)   /**< use 32 bit addressing      */
/** @} */

/**
 * @brief   Basic initialization of the given QSPI bus
 *
 * @note    This function MUST not be called more than once per bus!
 *
 * @param[in] bus       QSPI device to initialize
 */
void qspi_init(qspi_t bus);

/**
 * @brief   Configure the QSPI peripheral settings
 *
 * @param[in] bus       QSPI device to configure
 * @param[in] mode      QSPI mode @see qspi_mode_t
 * @param[in] flags     QSPI Configuration Options
 * @param[in] clk_hz    QSPI frequency in Hz
 */
void qspi_configure(qspi_t bus, qspi_mode_t mode, uint32_t flags, uint32_t clk_hz);

/**
 * @brief   Start a new QSPI transaction
 *
 * Starting a new QSPI transaction will get exclusive access to the QSPI bus
 * and configure it according to the given values. If another QSPI transaction
 * is active when this function is called, this function will block until the
 * other transaction is complete (qspi_relase was called).
 *
 * @param[in] bus       QSPI device to access
 */
void qspi_acquire(qspi_t bus);

/**
 * @brief   Finish an ongoing QSPI transaction by releasing the given QSPI bus
 *
 * After release, the given SPI bus should be fully powered down until acquired
 * again.
 *
 * @param[in]   bus     QSPI device to release
 */
void qspi_release(qspi_t bus);

/**
 * @brief   Execute a command with response data e.g Read Status, Read JEDEC
 *
 * @param[in]   bus      QSPI device
 * @param[in]   command  The JEDEC command ID
 * @param[out]  response Buffer for command response
 * @param[in]   len      Size of the command response buffer
 *
 * @return      length of the response
 *              negative error
 */
ssize_t qspi_cmd_read(qspi_t bus, uint8_t command, void *response, size_t len);

/**
 * @brief   Execute a command with data e.g Write Status, Write Enable
 *
 * @param[in]   bus      QSPI device
 * @param[in]   command  The JEDEC command ID
 * @param[in]   data     Command parameter data, may be NULL
 * @param[in]   len      Size of the command response buffer
 *
 * @return      number of bytes written
 *              negative error
 */
ssize_t qspi_cmd_write(qspi_t bus, uint8_t command, const void *data, size_t len);

/**
 * @brief   Read data from external memory.
 *          Typically it is implemented by quad read command (`0x6B`).
 *
 * @param[in]   bus      QSPI device
 * @param[in]   address  Address to read from
 * @param[out]  data     Buffer to store the data
 * @param[in]   len      Number of byte to read
 *
 * @return      length of bytes read, may be smaller than @p len if the
 *              driver has a maximal transfer size.
 *              negative error
 */
ssize_t qspi_read(qspi_t bus, uint32_t addr, void *data, size_t len);

/**
 * @brief   Write data to external memory.
 *          Can only write 1 -> 0, so target region should erased first.
 *          Typically it uses quad write command (`0x32`).
 *
 * @param[in]   bus       QSPI device
 * @param[in]   address   Address to write to
 * @param[in]   data      Buffer to write
 * @param[in]   len       Number of byte to write
 *
 * @return      number of bytes written, may be smaller than @p len
 *              negative error
 */
ssize_t qspi_write(qspi_t bus, uint32_t addr, const void *data, size_t len);

/**
 * @brief   Erase external memory by address
 *
 * @param[in]   bus       QSPI device
 * @param[in]   address   The address of the block that will be erased
 * @param[in]   size      The erase block size to use
 */
void qspi_erase(qspi_t bus, uint32_t address, qspi_erase_size_t size);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_QSPI_H */
/** @} */
