/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_at25xxx AT25xxx family of SPI-EEPROMs
 * @ingroup     drivers_misc
 *
 * @brief       This driver also support M95xxx, 25AAxxx, 25LCxxx, CAT25xxx & BR25Sxxx families
 * @{
 *
 * @file
 * @brief       Driver for the AT25xxx series of EEPROMs
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <stdint.h>
#include <stdbool.h>

#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   struct holding all params needed for device initialization
 */
typedef struct {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_clk_t spi_clk;      /**< SPI clock speed to use */
    spi_cs_t cs_pin;        /**< GPIO pin connected to chip select */
    gpio_t wp_pin;          /**< GPIO pin connected to the write-protect pin */
    gpio_t hold_pin;        /**< GPIO pin connected to the hold pin */
    uint32_t size;          /**< Size of the EEPROM in bytes */
    uint16_t page_size;     /**< Page Size of the EEPROM in bytes */
} at25xxx_params_t;

/**
 * @brief   struct that represents an AT25XXX device
 */
typedef struct {
    at25xxx_params_t params;    /**< parameters */
} at25xxx_t;

/**
 * @brief   Initialize an AT25XXX device handle with AT25XXX parameters
 *
 * @param[in, out] dev    AT25XXX device handle
 * @param[in] params      AT25XXX parameters
 *
 * @return    0 on success, -1 on failure
 *
 */
int at25xxx_init(at25xxx_t *dev, const at25xxx_params_t *params);

/**
 * @brief   Read a byte at a given position @p pos
 *
 * @param[in] dev      AT25XXX device handle
 * @param[in] pos      position in EEPROM memory
 *
 * @return    read byte
 */
uint8_t at25xxx_read_byte(const at25xxx_t *dev, uint32_t pos);

/**
 * @brief Sequentially read @p len bytes from a given position @p pos
 *
 * @param[in] dev       AT25XXX device handle
 * @param[in] pos       position in EEPROM memory
 * @param[out] data     read buffer
 * @param[in] len       requested length to be read
 *
 * @return    0 on success
 * @return    -ERANGE if pos + len > EEPROM size
 */
int at25xxx_read(const at25xxx_t *dev, uint32_t pos, void *data, size_t len);

/**
 * @brief   Write a byte at a given position @p pos
 *
 * @param[in] dev      AT25XXX device handle
 * @param[in] pos      position in EEPROM memory
 * @param[in] data     value to be written
 */
void at25xxx_write_byte(const at25xxx_t *dev, uint32_t pos, uint8_t data);

/**
 * @brief Sequentially write @p len bytes from a given position @p pos
 *
 * @param[in] dev       AT25XXX device handle
 * @param[in] pos       position in EEPROM memory
 * @param[in] data      write buffer
 * @param[in] len       requested length to be written
 *
 * @return    0 on success
 * @return    -ERANGE if pos + len > EEPROM size
 */
int at25xxx_write(const at25xxx_t *dev, uint32_t pos, const void *data, size_t len);

/**
 * @brief Sequentially write @p len bytes to a given @p page.
 *        The function will write up to the page boundary and then return.
 *
 * @param[in] dev       AT25XXX device handle
 * @param[in] page      page of EEPROM memory
 * @param[in] offset    offset from the start of the page, must be < page size
 * @param[in] data      write buffer
 * @param[in] len       requested length to be written
 *
 * @return    number of bytes written on success
 * @return    error on failure
 */
int at25xxx_write_page(const at25xxx_t *dev, uint32_t page, uint32_t offset,
                       const void *data, size_t len);

/**
 * @brief Set @p len bytes from a given position @p pos to the
 * value @p val
 *
 * @param[in] dev       AT25XXX device handle
 * @param[in] pos       position in EEPROM memory
 * @param[in] val       value to be set
 * @param[in] len       requested length to be written
 *
 * @return    0 on success
 * @return    -ERANGE if pos + len > EEPROM size
 */
int at25xxx_set(const at25xxx_t *dev, uint32_t pos, uint8_t val, size_t len);

/**
 * @brief Set @p len bytes from position @p pos to 0
 *
 * This is a wrapper around @see at25xxx_set.
 *
 * @param[in] dev       AT25XXX device handle
 * @param[in] pos       position in EEPROM memory
 * @param[in] len       requested length to be written
 *
 * @return    0 on success
 * @return    -ERANGE if pos + len > EEPROM size
 */
int at25xxx_clear(const at25xxx_t *dev, uint32_t pos, size_t len);

#ifdef __cplusplus
}
#endif
