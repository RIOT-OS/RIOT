/*
 * SPDX-FileCopyrightText: 2024 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_soft_i2c  Software I2C
 * @ingroup     drivers_soft_periph
 * @ingroup     drivers_periph_i2c
 * @brief       Software implemented I2C
 *
 * This module provides a software implemented I2C bus. It depends on the
 * @ref drivers_periph_gpio_ll and either uses open drain (if available) or
 * `gpio_ll_switch_dir` feature when not.
 *
 * It enabled the integrated pull ups on both SCL and SDA, if available.
 * External pull ups are generally recommended, for MCUs that do not support
 * internal pulls ups, they are mandatory.
 *
 * @{
 *
 * @file
 * @brief       Software I2C bus API
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#include <stdbool.h>

#include "periph/gpio.h"
#include "periph/i2c.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default I2C device access macro
 * @{
 */
#ifndef SOFT_I2C_DEV
#  define SOFT_I2C_DEV(x)   (x)
#endif
/** @} */

/**
 * @brief   Default I2C undefined value
 * @{
 */
#ifndef SOFT_I2C_UNDEF
#  define SOFT_I2C_UNDEF    (UINT_FAST8_MAX)
#endif
/** @} */


/**
 * @brief Software I2C bus descriptor
 */
typedef struct {
    gpio_t scl_pin;         /**< SCL pin */
    gpio_t sda_pin;         /**< SDA pin */
    i2c_speed_t speed;      /**< I2C bus speed */
    bool enable_internal_pull_ups;  /**< `true` means internal pull ups will be enabled */
} soft_i2c_conf_t;

/**
 * @brief   Software I2C type
 */
typedef uint_fast8_t soft_i2c_t;

/**
 * @brief   Initialize the given software I2C bus
 *
 * The given software I2C device will be initialized with the parameters as
 * specified in `soft_i2c_params.h`.
 *
 * @param[in] dev       the device to initialize
 */
void soft_i2c_init(soft_i2c_t dev);

/**
 * @brief   Reinitialize the used software I2C bus pins
 *
 * In case the I2C bus pins have been abused for other stuff, this can
 * revert the state to what the soft I2C bus implementation expects. Do not
 * use this function unless you know what you are doing.
 *
 * @param[in] dev       Software I2C device of which the GPIO pin configuratios
 *                      need to be restored.
 */
void soft_i2c_init_pins(soft_i2c_t dev);

/**
 * @brief   "Detach" the GPIO pins from the software I2C bus
 *
 * If you want to do nasty things with the software I2C bus pins, call this
 * first. It will block I2C transfers until calling @ref soft_i2c_init_pins
 *
 * @warning     Don't do anything that resembles a start condition.
 *
 * @param[in] dev       Software I2C device of which the GPIO pins should be
 *                      abused
 */
void soft_i2c_deinit_pins(soft_i2c_t dev);

/**
 * @brief   Get the SDA pin of the given software I2C bus.
 *
 * @param[in] dev       The device to query
 * @return              The GPIO used for the I2C data line.
 */
gpio_t soft_i2c_pin_sda(soft_i2c_t dev);

/**
 * @brief   Get the SCL pin of the given software I2C bus.
 *
 * @param[in] dev       The device to query
 * @return              The GPIO used for the I2C clock line.
 */
gpio_t soft_i2c_pin_scl(soft_i2c_t dev);

/**
 * @brief   Get mutually exclusive access to the given I2C bus
 *
 * In case the I2C device is busy, this function will block until the bus is
 * free again.
 *
 * @pre     Given device is valid, otherwise an assertion blows up
 *          (if assertions are enabled).
 *
 * @param[in] dev           I2C device to access
 */
void soft_i2c_acquire(soft_i2c_t dev);

/**
 * @brief   Release the given I2C device to be used by others
 *
 * @param[in] dev           I2C device to release
 */
void soft_i2c_release(soft_i2c_t dev);

/**
 * @brief   Convenience function for reading one byte from a given register
 *          address
 *
 * @note    This function is using a repeated start sequence for reading from
 *          the specified register address.
 *
 * @pre     soft_i2c_acquire() must be called before accessing the bus
 *
 * @param[in]  dev          software I2C device
 * @param[in]  reg          register address to read from (8- or 16-bit,
 *                          right-aligned)
 * @param[in]  addr         7-bit or 10-bit device address (right-aligned)
 * @param[out] data         memory location to store received data
 * @param[in]  flags        optional flags (see @ref i2c_flags_t)
 *
 * @retval                  0 on success
 * @retval                  -EIO when the slave device doesn't ACK the byte
 * @retval                  -ENXIO when no devices respond on the address sent on the bus
 * @retval                  -ETIMEDOUT when timeout occurs before device's response
 * @retval                  -EINVAL when an invalid argument is given
 * @retval                  -EOPNOTSUPP when the MCU driver doesn't support the flag operation
 * @retval                  -EAGAIN when a lost bus arbitration occurs
 */

int soft_i2c_read_reg(soft_i2c_t dev, uint16_t addr, uint16_t reg,
                      void *data, uint8_t flags);

/**
 * @brief   Convenience function for reading several bytes from a given
 *          register address
 *
 * @note    This function is using a repeated start sequence for reading from
 *          the specified register address.
 *
 * @pre     soft_i2c_acquire() must be called before accessing the bus
 *
 * @param[in]  dev          software I2C device
 * @param[in]  reg          register address to read from (8- or 16-bit,
 *                          right-aligned)
 * @param[in]  addr         7-bit or 10-bit device address (right-aligned)
 * @param[out] data         memory location to store received data
 * @param[in]  len          the number of bytes to read into @p data
 * @param[in]  flags        optional flags (see @ref i2c_flags_t)
 *
 * @retval                  0 on success
 * @retval                  -EIO when the slave device doesn't ACK the byte
 * @retval                  -ENXIO when no devices respond on the address sent on the bus
 * @retval                  -ETIMEDOUT when timeout occurs before device's response
 * @retval                  -EINVAL when an invalid argument is given
 * @retval                  -EOPNOTSUPP when the MCU driver doesn't support the flag operation
 * @retval                  -EAGAIN when a lost bus arbitration occurs
 */
int soft_i2c_read_regs(soft_i2c_t dev, uint16_t addr, uint16_t reg,
                       void *data, size_t len, uint8_t flags);

/**
 * @brief   Convenience function for reading one byte from a device
 *
 * @note    This function is using a repeated start sequence for reading from
 *          the specified register address.
 *
 * @pre     soft_i2c_acquire() must be called before accessing the bus
 *
 * @param[in]  dev          software I2C device
 * @param[in]  addr         7-bit or 10-bit device address (right-aligned)
 * @param[out] data         memory location to store received data
 * @param[in]  flags        optional flags (see @ref i2c_flags_t)
 *
 * @retval                  0 on success
 * @retval                  -EIO when the slave device doesn't ACK the byte
 * @retval                  -ENXIO when no devices respond on the address sent on the bus
 * @retval                  -ETIMEDOUT when timeout occurs before device's response
 * @retval                  -EINVAL when an invalid argument is given
 * @retval                  -EOPNOTSUPP when the MCU driver doesn't support the flag operation
 * @retval                  -EAGAIN when a lost bus arbitration occurs
 */

int soft_i2c_read_byte(soft_i2c_t dev, uint16_t addr, void *data, uint8_t flags);

/**
 * @brief   Convenience function for reading bytes from a device
 *
 * @note    This function is using a repeated start sequence for reading from
 *          the specified register address.
 *
 * @pre     soft_i2c_acquire() must be called before accessing the bus
 *
 * @param[in]  dev          software I2C device
 * @param[in]  addr         7-bit or 10-bit device address (right-aligned)
 * @param[out] data         memory location to store received data
 * @param[in]  len          the number of bytes to read into @p data
 * @param[in]  flags        optional flags (see @ref i2c_flags_t)
 *
 * @retval                  0 on success
 * @retval                  -EIO when the slave device doesn't ACK the byte
 * @retval                  -ENXIO when no devices respond on the address sent on the bus
 * @retval                  -ETIMEDOUT when timeout occurs before device's response
 * @retval                  -EINVAL when an invalid argument is given
 * @retval                  -EOPNOTSUPP when the MCU driver doesn't support the flag operation
 * @retval                  -EAGAIN when a lost bus arbitration occurs
 */

int soft_i2c_read_bytes(soft_i2c_t dev, uint16_t addr, void *data, size_t len,
                        uint8_t flags);

/**
 * @brief   Convenience function for writing a single byte onto the bus
 *
 * @pre     soft_i2c_acquire() must be called before accessing the bus
 *
 * @param[in] dev           software I2C device
 * @param[in] addr          7-bit or 10-bit device address (right-aligned)
 * @param[in] data          byte to write to the device
 * @param[in] flags         optional flags (see @ref i2c_flags_t)
 *
 * @retval                  0 on success
 * @retval                  -EIO when the slave device doesn't ACK the byte
 * @retval                  -ENXIO when no devices respond on the address sent on the bus
 * @retval                  -ETIMEDOUT when timeout occurs before device's response
 * @retval                  -EINVAL when an invalid argument is given
 * @retval                  -EOPNOTSUPP when the MCU driver doesn't support the flag operation
 * @retval                  -EAGAIN when a lost bus arbitration occurs
 */
int soft_i2c_write_byte(soft_i2c_t dev, uint16_t addr, uint8_t data,
                        uint8_t flags);

/**
 * @brief   Convenience function for writing several bytes onto the bus
 *
 * @pre     soft_i2c_acquire() must be called before accessing the bus
 *
 * @param[in] dev           software I2C device
 * @param[in] addr          7-bit or 10-bit device address (right-aligned)
 * @param[in] data          array holding the bytes to write to the device
 * @param[in] len           the number of bytes to write
 * @param[in] flags         optional flags (see @ref i2c_flags_t)
 *
 * @retval                  0 on success
 * @retval                  -EIO when the slave device doesn't ACK the byte
 * @retval                  -ENXIO when no devices respond on the address sent on the bus
 * @retval                  -ETIMEDOUT when timeout occurs before device's response
 * @retval                  -EINVAL when an invalid argument is given
 * @retval                  -EOPNOTSUPP when the MCU driver doesn't support the flag operation
 * @retval                  -EAGAIN when a lost bus arbitration occurs
 */
int soft_i2c_write_bytes(soft_i2c_t dev, uint16_t addr, const void *data,
                         size_t len, uint8_t flags);

/**
 * @brief   Convenience function for writing one byte to a given
 *          register address
 *
 * @note    This function is using a continuous sequence for writing to the
 *          specified register address. First it writes the register then data.
 *
 * @pre     soft_i2c_acquire() must be called before accessing the bus
 *
 * @param[in]  dev          software I2C device
 * @param[in]  reg          register address to read from (8- or 16-bit,
 *                          right-aligned)
 * @param[in]  addr         7-bit or 10-bit device address (right-aligned)
 * @param[in]  data         byte to write
 * @param[in]  flags        optional flags (see @ref i2c_flags_t)
 *
 * @retval                  0 on success
 * @retval                  -EIO when the slave device doesn't ACK the byte
 * @retval                  -ENXIO when no devices respond on the address sent on the bus
 * @retval                  -ETIMEDOUT when timeout occurs before device's response
 * @retval                  -EINVAL when an invalid argument is given
 * @retval                  -EOPNOTSUPP when the MCU driver doesn't support the flag operation
 * @retval                  -EAGAIN when a lost bus arbitration occurs
 */
int soft_i2c_write_reg(soft_i2c_t dev, uint16_t addr, uint16_t reg,
                       uint8_t data, uint8_t flags);

/**
 * @brief   Convenience function for writing data to a given register address
 *
 * @note    This function is using a continuous sequence for writing to the
 *          specified register address. First it writes the register then the data.
 *
 * @pre     soft_i2c_acquire() must be called before accessing the bus
 *
 * @param[in]  dev          software I2C device
 * @param[in]  reg          register address to read from (8- or 16-bit,
 *                          right-aligned)
 * @param[in]  addr         7-bit or 10-bit device address (right-aligned)
 * @param[out] data         memory location to store received data
 * @param[in]  len          the number of bytes to write
 * @param[in]  flags        optional flags (see @ref i2c_flags_t)
 *
 * @retval                  0 on success
 * @retval                  -EIO when the slave device doesn't ACK the byte
 * @retval                  -ENXIO when no devices respond on the address sent on the bus
 * @retval                  -ETIMEDOUT when timeout occurs before device's response
 * @retval                  -EINVAL when an invalid argument is given
 * @retval                  -EOPNOTSUPP when the MCU driver doesn't support the flag operation
 * @retval                  -EAGAIN when a lost bus arbitration occurs
 */
int soft_i2c_write_regs(soft_i2c_t dev, uint16_t addr, uint16_t reg,
                        const void *data, size_t len, uint8_t flags);

#ifdef __cplusplus
}
#endif
/** @} */
