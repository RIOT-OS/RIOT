/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_i2c I2C
 * @ingroup     drivers_periph
 * @brief       Low-level I2C peripheral driver
 *
 * @{
 * @file
 * @brief       Low-level I2C peripheral driver interface definition
 *
 * The I2C signal lines SDA/SCL need external pull-up resistors which connect
 * the lines to the positive voltage supply Vcc. The I2C driver implementation
 * should enable the pin's internal pull-up resistors. There are however some
 * use cases for which the internal pull resistors are not strong enough and the
 * I2C bus will show faulty behavior. This can for example happen when
 * connecting a logic analyzer which will raise the capacitance of the bus. In
 * this case you should make sure you connect external pull-up resistors to both
 * I2C bus lines.
 *
 * The minimum and maximum resistances are computed by:
 * \f{eqnarray*}{
 * R_{min} &=& \frac{V_{DD} - V_{OL(max)}} {I_{OL}}\\
 * R_{max} &=& \frac{t_r} {(0.8473 \cdot C_b)}
 * \f}<br>
 * where:<br>
 * \f$ V_{DD} =\f$ Supply voltage,
 * \f$ V_{OL(max)} =\f$ Low level voltage,
 * \f$ I_{OL} =\f$ Low level output current,
 * \f$ t_r =\f$ Signal rise time,
 * \f$ C_b =\f$ Bus capacitance <br>
 * <br>The pull-up resistors depend on the bus speed.
 * Some typical values are:<br>
 * Normal mode:       10k&Omega;<br>
 * Fast mode:          2k&Omega;<br>
 * Fast plus mode:     2k&Omega;
 *
 * For more details refer to section 7.1 in:<br>
 * http://www.nxp.com/documents/user_manual/UM10204.pdf
 *
 * @note        The current version of this interface only supports the
                7-bit addressing mode.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef PERIPH_I2C_H
#define PERIPH_I2C_H

#include <stdint.h>
#include <limits.h>

#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Flag signaling a write operation on the bus
 */
#define I2C_FLAG_WRITE      0

/**
 * @brief   Flag signaling a read operation on the bus
 */
#define I2C_FLAG_READ       1

/**
 * @brief   Default I2C device access macro
 * @{
 */
#ifndef I2C_DEV
#define I2C_DEV(x)          (x)
#endif
/** @} */

/**
 * @brief   Default I2C undefined value
 * @{
 */
#ifndef I2C_UNDEF
#define I2C_UNDEF           (UINT_MAX)
#endif
/** @} */

/**
 * @brief   Default i2c_t type definition
 * @{
 */
#ifndef HAVE_I2C_T
typedef unsigned int i2c_t;
#endif
/**  @} */

/**
 * @brief   Default mapping of I2C bus speed values
 * @{
 */
#ifndef HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW = 0,      /**< low speed mode:    ~10kbit/s */
    I2C_SPEED_NORMAL,       /**< normal mode:       ~100kbit/s */
    I2C_SPEED_FAST,         /**< fast mode:         ~400kbit/sj */
    I2C_SPEED_FAST_PLUS,    /**< fast plus mode:    ~1Mbit/s */
    I2C_SPEED_HIGH,         /**< high speed mode:   ~3.4Mbit/s */
} i2c_speed_t;
#endif
/** @} */

/**
 * @brief   I2C transfer results
 */
enum {
    /**
     * @brief   All bytes were transferred successfully
     */
    I2C_ACK       = 0,
    /**
     * @brief   NACK when transferring the address byte
     *
     * After the address + the read/write bit were send, we got an NACK as
     * response. This means most probably, that there is no slave with the used
     * address on the bus, or the slave did just not respond for some reason.
     */
    I2C_ADDR_NACK = -1
    /**
     * @brief   NACK while writing data bytes
     *
     * The slave responded to a data byte written to it with a NACK.
     */
    I2C_DATA_NACK = -2       /**< data error */
     /**
      * @brief   Internal error
      *
      * This status code is returned, on any other internal error that might
      * have occurred.
      */
    I2C_ERR = -3;
};

/**
 * @brief   Initialize the given I2C bus
 *
 * The given I2C device will be initialized with the parameters as specified in
 * the boards periph_conf.h, using the pins and the speed value given there.
 *
 * @param[in] dev       the device to initialize
 *
 * @return                  0 on successful initialization
 * @return                  -1 on undefined device given
 */
int i2c_init(i2c_t dev);

/**
 * @brief   Get mutually exclusive access to the given I2C bus
 *
 * In case the I2C device is busy, this function will block until the bus is
 * free again.
 *
 * @param[in] dev           I2C device to access
 *
 * @return                  0 on success
 */
int i2c_acquire(i2c_t dev);

/**
 * @brief   Release the given I2C device to be used by others
 *
 * @param[in] dev           I2C device to release
 */
void i2c_release(i2c_t dev);

/**
 * @brief   Read data from an I2C device with the given address
 *
 * @param[in]  dev          I2C peripheral device
 * @param[in]  addr         7-bit device address (right-aligned)
 * @param[out] data         array holding the received bytes
 * @param[in]  length       the number of bytes to read into `data`
 *
 * @return                  I2C_ACK on successful transfer of @p len byte
 * @return                  I2C_ADDR_NACK if response to address byte was NACK
 * @return                  I2C_ERR for any other error
 */
int i2c_read(i2c_t dev, uint8_t addr, uint8_t *data, size_t len);

/**
 * @brief   Read data from a specified register
 *
 * @param[in]  dev          I2C peripheral device
 * @param[in]  addr         7-bit device address (right-aligned)
 * @param[in]  reg          the register address on the targeted I2C device
 * @param[out] data         array holding the received bytes
 * @param[in]  len          number of bytes to read into `data
 *
 * @return                  I2C_ACK on successful transfer of @p len byte
 *                          to @p reg
 * @return                  I2C_ADDR_NACK if response to address byte was NACK
 * @return                  I2C_ERR for any other error
 */
int i2c_read_reg(i2c_t dev, uint8_t addr, uint8_t reg,
                 uint8_t *data, size_t len);

/**
 * @brief   Write one byte to an I2C device with the given address
 *
 * @param[in] dev           I2C peripheral device
 * @param[in] addr          7-bit device address (right-aligned)
 * @param[in] data          byte to write to the device
 *
 * @return                  I2C_ACK on successful transfer of @p data
 * @return                  I2C_ADDR_NACK if response to address byte was NACK
 * @return                  I2C_DATA_NACK if response to the data byte was NACK
 * @return                  I2C_ERR for any other error
 */
int i2c_write_byte(i2c_t dev, uint8_t addr, uint8_t data);

/**
 * @brief   Write multiple bytes to an I2C device with the given address
 *
 * @param[in] dev           I2C peripheral device
 * @param[in] addr          7-bit device address (right-aligned)
 * @param[in] data          array with bytes to write to the target device
 * @param[in] len           number of bytes to write to the target device
 *
 * @return                  I2C_ACK on successful transfer of @p len byte
 * @return                  I2C_ADDR_NACK if response to address byte was NACK
 * @return                  I2C_DATA_NACK if response to any data byte was NACK
 * @return                  I2C_ERR for any other error
 */
int i2c_write_bytes(i2c_t dev, uint8_t addr, uint8_t *data, size_t len);

/**
 * @brief   Write one byte to a register at the I2C slave with the given address
 *
 * @param[in] dev           I2C peripheral device
 * @param[in] addr          7-bit device address (right-aligned)
 * @param[in] reg           the register address on the targeted I2C device
 * @param[in] data          byte to write to the device
 *
 * @return                  I2C_ACK on successful transfer of @p data to @p reg
 * @return                  I2C_ADDR_NACK if response to address byte was NACK
 * @return                  I2C_DATA_NACK if response to the addr or the data
 *                          byte was NACK
 * @return                  I2C_ERR for any other error
 */
int i2c_write_reg(i2c_t dev, uint8_t addr, uint8_t reg, uint8_t data);

/**
 * @brief   Write multiple bytes to a register at the I2C slave with the given
 *          address
 *
 * @param[in] dev           I2C peripheral device
 * @param[in] addr          7-bit device address (right-aligned)
 * @param[in] reg           the register address on the targeted I2C device
 * @param[in] data          array with bytes to write to the target device
 * @param[in] len           number of bytes to write to the target device
 *
 * @return                  I2C_ACK on successful transfer of @p len byte
 *                          to @p reg
 * @return                  I2C_ADDR_NACK if response to address byte was NACK
 * @return                  I2C_DATA_NACK if response to the addr or to any data
 *                          byte was NACK
 * @return                  I2C_ERR for any other error
 */
int i2c_write_regs(i2c_t dev, uint8_t addr, uint8_t reg,
                   uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_I2C_H */
/** @} */
