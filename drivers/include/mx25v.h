/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mx25v MX25V
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the MX25V
 * @{
 *
 * @file
 * @brief       Device driver interface for the MX25V flash memory
 *
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 */

#ifndef MX25V_H
#define MX25V_H

#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MX25V_MANUFACTURER_ID     (uint8_t)(0xC2) /**< MACRONIX ID */
#define MX25V_MEMORY_ID           (uint8_t)(0x20)
#define MX25V_DEVICE_ID           (uint8_t)(0x17)

/**
 * @brief MX25V8006E Related Parameter Definition
 * @{
 */
#define MX25V_FLASH_SIZE    (0x100000)      /**< 1024 KB flash size */
#define MX25V_BLOCK_SIZE    (0x10000)       /**<   64 KB per block  */
#define MX25V_SECTOR_SIZE   (0x1000)        /**<    4 KB per sector */
#define MX25V_PAGE_SIZE     (0x0100)        /**<   256 B per page   */
#define MX25V_BLOCK_NUM     (MX25V_FLASH_SIZE / MX25V_BLOCK_SIZE)
#define MX25V_SECTOR_NUM    (MX25V_FLASH_SIZE / MX25V_SECTOR_SIZE)
#define MX25V_PAGE_NUM      (MX25V_FLASH_SIZE / MX25V_PAGE_SIZE)
/** @} */


/**
 * @brief bit mask definition
 * @{
 */
#define MX25V_WIP           (0x01)     /**< write in progress bit */
#define MX25V_WEL           (0x02)     /**< write enable latch bit */
#define MX25V_BP0           (0x04)     /**< block protect BP0 bit */
#define MX25V_BP1           (0x08)     /**< block protect BP1 bit */
#define MX25V_BP2           (0x10)     /**< block protect BP2 bit */
#define MX25V_SRWD          (0x80)     /**< status register write disable bit */
#define MX25V_READ_STATUS   (0x9F)     /**< read status register mask */
/** @} */

 /**
 * @brief Protect Level
 * @{
 */
#define MX25V_PROTECT_LEVEL_0    (0x00 << 2)
#define MX25V_PROTECT_LEVEL_1    (0x01 << 2)
#define MX25V_PROTECT_LEVEL_2    (0x02 << 2)
#define MX25V_PROTECT_LEVEL_3    (0x03 << 2)
#define MX25V_PROTECT_LEVEL_4    (0x04 << 2)
#define MX25V_PROTECT_LEVEL_5    (0x05 << 2)
#define MX25V_PROTECT_LEVEL_6    (0x06 << 2)
#define MX25V_PROTECT_LEVEL_7    (0x07 << 2)
#define MX25V_PROTECT_LEVEL_MASK (0x07 << 2)
/** @} */

/**
* @brief MX25V device structure
* @{
*/
typedef struct {
    spi_t spi_dev;              /**< SPI device used */
    spi_conf_t conf;            /**< SPI clock phase and clock polarity */
    spi_speed_t speed;          /**< SPI clock speed */
    gpio_t cs_pin;              /**< chip select pin */
    gpio_t hold_pin;            /**< hold pin */
    gpio_t wp_pin;              /**< wp pin */
} mx25v_t;
/** @} */

/**
* @brief MX25V identification
* @{
*/
typedef struct {
    uint8_t manufacturer_id;    /**< manufacturer ID */
    uint8_t memory_id;          /**< memory ID */
    uint8_t device_id;          /**< device ID */
} mx25v_id_t;
/** @} */

/**
 * @brief Get default SPI interface configuration.
 *
 * This function initializes the SPI configuration struct with default settings
 * that should work with Serial Flash devices.
 *
 * @param[out] config Address of config struct to initialize
 *
 */
int mx25v_init(mx25v_t *mx25v_dev);

/**
 * @brief Read Serial Flash identification.
 * Read ID information with command RDID, RES and REMS.
 *
 * @param[in]  mx25v_dev    Pointer to mx25v device
 * @param[out] id           Pointer to mx25v id
 *
 * @return                  0 on operation succeeded
 * @return                  -1 on operation failed
 */
int mx25v_read_id(mx25v_t *mx25v_dev, mx25v_id_t *id);

/**
 * @brief Read Status Register.
 *
 * The RDSR instruction is for reading Status Register Bits.
 *
 * @param[in]  mx25v_dev    Pointer to mx25v device
 * @param[out] value        Pointer to status buffer
 *
 * @return                  0 on operation succeeded
 * @return                  -1 on operation failed
 */
int mx25v_read_status(mx25v_t *mx25v_dev, char *value);

/**
 * @brief Write Status Register.
 *
 * The WRSR instruction is for changing the values of Status Register Bits
 * (and configuration register).
 *
 * @param[in]  mx25v_dev    Pointer to mx25v device
 * @param[out] value        Value to be write
 *
 * @return                  0 on operation succeeded
 * @return                  -1 on operation failed
 * @return                  -3 if flash in busy
 * @return                  -4 if flash not ready
 */
int mx25v_write_status(mx25v_t *mx25v_dev, char value);

/**
 * @brief Read data from Serial Flash device.
 *
 * This function reads data from the Serial Flash device, into a buffer.
 *
 * @param[in]  mx25v_dev    Pointer to mx25v device
 * @param[in]  address      SerialFlash internal address to start reading from
 * @param[out] data         Buffer to write data into
 * @param[in]  length       Number of bytes to read
 *
 * @return                  0 on operation succeeded
 * @return                  -1 on operation failed
 * @return                  -2 if address and/or length is out of bounds
 */
int mx25v_read_buffer(mx25v_t *mx25v_dev, uint32_t address, char *data,
                      uint32_t length);

/**
 * @brief Write data to Serial Flash device.
 *
 * This function writes data to the Serial Flash device, from a buffer.
 *
 * @note Please erase related memory area before write operation.
 *
 * @param[in] mx25v_dev     Pointer to mx25v device
 * @param[in] address       SerialFlash internal address to start writing to
 * @param[in] data          Buffer to read data from
 * @param[in] length        Number of bytes to write
 *
 * @return                  0 on operation succeeded
 * @return                  -1 on operation failed
 * @return                  -2 if address and/or length is out of bounds
 * @return                  -3 if flash in busy
 * @return                  -4 if flash not ready
 */
int mx25v_write_buffer(mx25v_t *mx25v_dev, uint32_t address, const unsigned char *data,
                       uint32_t length);

/**
 * @brief Erase sector of Serial Flash device.
 *
 * The SE instruction is for erasing the data of the chosen
 * sector (4KB) to be "1".
 *
 * @param[in] mx25v_dev     Pointer to mx25v device
 * @param[in] address       Serial flash internal address in the sector
 *
 * @return                  0 on operation succeeded
 * @return                  -1 on operation failed
 * @return                  -2 if address and/or length is out of bounds
 * @return                  -3 if flash in busy
 * @return                  -4 if flash not ready
 */
int mx25v_erase_sector(mx25v_t *mx25v_dev, uint32_t address);

/**
 * @brief Erase block of Serial Flash device.
 *
 * The BE instruction is for erasing the data of the chosen
 * sector (64KB) to be "1".
 *
 * @param[in] mx25v_dev     Pointer to mx25v device
 * @param[in] address       Serial flash internal address in the block
 *
 * @return                  0 on operation succeeded
 * @return                  -1 on operation failed
 * @return                  -2 if address and/or length is out of bounds
 * @return                  -3 if flash in busy
 * @return                  -4 if flash not ready
 */
int mx25v_erase_block(mx25v_t *mx25v_dev, uint32_t address);

/**
 * @brief Erase all chip of Serial Flash device.
 *
 * The CE instruction is for erasing the data of the whole chip to be "1".
 *
 * @param[in] mx25v_dev     Pointer to mx25v device
 *
 * @return                  0 on operation succeeded
 * @return                  -1 on operation failed
 * @return                  -3 if flash in busy
 * @return                  -4 if flash not ready
 */
int mx25v_erase_chip(mx25v_t *mx25v_dev);

/**
 * @brief Enter deep power down mode.
 *
 * The DP instruction is for setting the device on the minimizing
 * the power consumption.
 *
 * @param[in] mx25v_dev     Pointer to mx25v device
 *
 * @return                  0 on operation succeeded
 * @return                  -1 on operation failed
 */
int mx25v_enter_deep_powerdown(mx25v_t *mx25v_dev);

/**
 * @brief Exit deep power down mode.
 *
 * The Release from RDP instruction is putting the device in the
 * Stand-by Power mode.
 *
 * @param[in] mx25v_dev     Pointer to mx25v device
 *
 * @return                  0 on operation succeeded
 * @return                  -1 on operation failed
 */
int mx25v_exit_deep_powerdown(mx25v_t *mx25v_dev);

#ifdef __cplusplus
}
#endif
/** @} */

#endif /* MX25V_H */
