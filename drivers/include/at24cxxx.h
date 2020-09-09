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
int at24cxxx_read(const at24cxxx_t *dev, uint32_t pos, void *data,
                  size_t len);

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
 * @brief Sequentially write @p len bytes to a given @p page.
 *        The function will write up to the page boundary and then return
 *        the number of bytes written up to that.
 *
 * @param[in] dev       AT24CXXX device handle
 * @param[in] page      page of EEPROM memory
 * @param[in] offset    offset from the start of the page, must be < page size
 * @param[in] data      write buffer
 * @param[in] len       requested length to be written
 *
 * @return    number of bytes written on success
 * @return    error on failure
 */
int at24cxxx_write_page(const at24cxxx_t *dev, uint32_t page, uint32_t offset,
                        const void *data, size_t len);

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

#endif /* AT24CXXX_H */
/** @} */
