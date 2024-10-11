/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_at24cxxx AT24CXXX EEPROM unit
 * @ingroup     drivers_misc
 * @brief       Device driver interface for the AT24CXXX EEPROM units
 *
 * @section overview Overview
 * Various manufacturers such as Atmel/Microchip or ST offer small I2C EEPROMs which usually
 * come in 8-pin packages and are used for persistent data storage of settings, counters, etc.
 * This driver adds support for these devices with direct read and write functions.
 *
 * The high level wrapper for RIOTs MTD interface to utilize the I2C EEPROMs as MTD storage
 * is described in drivers_mtd_at24cxxx.
 *
 * A list of supported devices can be found in the at24cxxx_defines.h file.
 *
 * @section usage Usage
 *
 * The preconfigured devices in the at24cxxx_defines.h file devices are easily
 * accessible as pseudomodules and can be added to the Makefile of your project:
 *
 *      USEMODULE += at24c02
 *
 * When using one of the pseudomodules, the configuration of the device is already
 * predefined in the AT24CXXX_PARAMS macro and can be used for the
 * initialization:
 *
 *      at24cxxx_t eeprom_dev;
 *      at24cxxx_params_t eeprom_params = AT24CXXX_PARAMS;
 *
 *      at24cxxx_init(&eeprom_dev, &eeprom_params);
 *
 * \n
 * For other devices that are not yet part of the library, the generic module
 * has to be added:
 *
 *      USEMODULE += at24cxxx
 *
 * The predefined macro can not be used in this case, so the parameters of the
 * device have to be added to the at24cxxx_params_t structure manually with
 * the values from the corresponding datasheet:
 *
 *      at24cxxx_t eeprom_dev;
 *      at24cxxx_params_t eeprom_params = {
 *          .i2c = I2C_DEV(0), \
 *          ...
 *      };
 *
 *      at24cxxx_init(&eeprom_dev, &eeprom_params);
 *
 *
 * @{
 *
 * @file
 * @brief       Device driver interface for AT24CXXX EEPROM units.
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 */

#ifndef AT24CXXX_H
#define AT24CXXX_H

#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Return values
 */
enum {
    AT24CXXX_OK,
    AT24CXXX_I2C_ERROR
};

/**
 * @brief   Struct that holds initialization parameters
 */
typedef struct at24cxxx_params {
    i2c_t i2c;                      /**< I2C bus number */
    gpio_t pin_wp;                  /**< write protect pin */
    uint32_t eeprom_size;           /**< EEPROM memory capacity */
    uint8_t dev_addr;               /**< I2C device address */
    uint8_t page_size;              /**< page size */
    uint8_t max_polls;              /**< number of ACK poll attempts */
} at24cxxx_params_t;

/**
 * @brief   Struct that represents an AT24CXXX device
 */
typedef struct {
    at24cxxx_params_t params;     /**< parameters */
} at24cxxx_t;

/**
 * @brief   Initialize an AT24CXXX device handle with AT24CXXX parameters
 *
 * @param[in, out]  dev     AT24CXXX device handle
 * @param[in]       params  AT24CXXX parameters
 *
 * @return          AT24CXXX_OK on success
 * @return          -AT24CXXX_I2C_ERROR if i2c could not be acquired
 * @return          -EINVAL if input parameters are NULL
 */
int at24cxxx_init(at24cxxx_t *dev, const at24cxxx_params_t *params);

/**
 * @brief   Read a byte at a given position @p pos
 *
 * @param[in]       dev      AT24CXXX device handle
 * @param[in]       pos      Position in EEPROM memory
 * @param[out]      dest     Read byte
 *
 * @return          AT24CXXX_OK on success
 * @return          -ERANGE if @p pos is out of bounds
 * @return          @see i2c_read_regs
 */
int at24cxxx_read_byte(const at24cxxx_t *dev, uint32_t pos, void *dest);

/**
 * @brief Sequentially read @p len bytes from a given position @p pos
 *
 * @param[in]       dev     AT24CXXX device handle
 * @param[in]       pos     Position in EEPROM memory
 * @param[out]      data    Read buffer
 * @param[in]       len     Requested length to be read
 *
 * @return          AT24CXXX_OK on success
 * @return          -ERANGE if @p pos + @p len is out of bounds
 * @return          @see i2c_read_regs
 */
int at24cxxx_read(const at24cxxx_t *dev, uint32_t pos, void *data, size_t len);

/**
 * @brief   Write a byte at a given position @p pos
 *
 * @param[in]       dev      AT24CXXX device handle
 * @param[in]       pos      Position in EEPROM memory
 * @param[in]       data     Value to be written
 *
 * @return          AT24CXXX_OK on success
 * @return          -ERANGE if @p pos is out of bounds
 * @return          @see i2c_write_regs
 */
int at24cxxx_write_byte(const at24cxxx_t *dev, uint32_t pos, uint8_t data);

/**
 * @brief   Sequentially write @p len bytes from a given position @p pos
 *
 * Writing is performed in chunks of size AT24CXXX_PAGE_SIZE.
 *
 * @param[in]       dev       AT24CXXX device handle
 * @param[in]       pos       Position in EEPROM memory
 * @param[in]       data      Write buffer
 * @param[in]       len       Requested length to be written
 *
 * @return          AT24CXXX_OK on success
 * @return          -ERANGE if @p pos + @p len is out of bounds
 * @return          @see i2c_write_regs
 */
int at24cxxx_write(const at24cxxx_t *dev, uint32_t pos, const void *data,
                   size_t len);

/**
 * @brief   Set @p len bytes from a given position @p pos to the
 * value @p val
 *
 * Writing is performed in chunks of size AT24CXXX_SET_BUFFER_SIZE.
 *
 * @param[in]       dev       AT24CXXX device handle
 * @param[in]       pos       Position in EEPROM memory
 * @param[in]       val       Value to be set
 * @param[in]       len       Requested length to be written
 *
 * @return          AT24CXXX_OK on success
 * @return          -ERANGE if @p pos + @p len is out of bounds
 * @return          @see i2c_write_byte
 */
int at24cxxx_set(const at24cxxx_t *dev, uint32_t pos, uint8_t val,
                 size_t len);

/**
 * @brief Set @p len bytes from position @p pos to
 * AT24CXXX_CLEAR_BYTE
 *
 * This is a wrapper around @see at24cxxx_set.
 *
 * @param[in]       dev       AT24CXXX device handle
 * @param[in]       pos       Position in EEPROM memory
 * @param[in]       len       Requested length to be written
 *
 * @return          @see at24cxxx_set
 */
int at24cxxx_clear(const at24cxxx_t *dev, uint32_t pos, size_t len);

/**
 * @brief Set the entire EEPROM memory to AT24CXXX_CLEAR_BYTE
 *
 * This is a wrapper around @see at24cxxx_clear.
 *
 * @param[in]       dev       AT24CXXX device handle
 *
 * @return          @see at24cxxx_set
 */
int at24cxxx_erase(const at24cxxx_t *dev);

/**
 * @brief Enable write protection
 *
 * @param[in]       dev       AT24CXXX device handle
 *
 * @return          AT24CXXX_OK on success
 * @return          -ENOTSUP if pin_wp was initialized with GPIO_UNDEF
 */
int at24cxxx_enable_write_protect(const at24cxxx_t *dev);

/**
 * @brief Disable write protection
 *
 * @param[in]       dev       AT24CXXX device handle
 *
 * @return          AT24CXXX_OK on success
 * @return          -ENOTSUP if pin_wp was initialized with GPIO_UNDEF
 */
int at24cxxx_disable_write_protect(const at24cxxx_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* AT24CXXX_H */
