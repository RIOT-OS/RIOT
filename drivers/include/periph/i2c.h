/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_periph_i2c I2C
 * @ingroup     driver_periph
 * @brief       Low-level I2C peripheral driver
 * @{
 *
 * @file
 * @brief       Low-level I2C peripheral driver interface definition
 *
 * @note The current version of this interface only supports the 7-bit addressing mode.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __I2C_H
#define __I2C_H

#include <stdint.h>

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* guard this file in case no I2C device is defined */
#if I2C_NUMOF

/** @brief Flag to use for reading from the I2C bus */
#define I2C_FLAG_WRITE      0
/** @brief Flag to use for writing to the I2C bus */
#define I2C_FLAG_READ       1

/**
 * @brief define I2C device identifiers
 */
typedef enum {
#if I2C_0_EN
    I2C_0 = 0,              /**< I2C device 0 */
#endif
#if I2C_1_EN
    I2C_1,                  /**< I2C device 1 */
#endif
#if I2C_2_EN
    I2C_2,                  /**< I2C device 2 */
#endif
#if I2C_3_EN
    I2C_3,                  /**< I2C device 3 */
#endif
} i2c_t;

/**
 * @brief define I2C bus speed values
 */
typedef enum {
    I2C_SPEED_LOW = 0,      /**< low speed mode:    ~10kbit/s */
    I2C_SPEED_NORMAL,       /**< normal mode:       ~100kbit/s */
    I2C_SPEED_FAST,         /**< fast mode:         ~400kbit/sj */
    I2C_SPEED_FAST_PLUS,    /**< fast plus mode:    ~1Mbit/s */
    I2C_SPEED_HIGH,         /**< high speed mode:   ~3.4Mbit/s */
} i2c_speed_t;

/**
 * @brief Initialize an I2C device to run as bus master
 *
 * @param[in] dev           the device to initialize
 * @param[in] speed         the selected bus speed
 *
 * @return                  0 on successful initialization
 * @return                  -1 on undefined device given
 * @return                  -2 on unsupported speed value
 */
int i2c_init_master(i2c_t dev, i2c_speed_t speed);

/**
 * @brief Initialize an I2C device to run in slave mode
 *
 * @param[in] dev           the device to initialize
 * @param[in] address       the devices I2C address
 *
 * @return                  0 on success
 * @return                  -1 on undefined device given
 */
int i2c_init_slave(i2c_t dev, uint8_t address);

/**
 * @brief Read one byte from an I2C device with the given address
 *
 * @param[in]  dev          I2C peripheral device
 * @param[in]  address      bus address of the target device
 * @param[out] data         the result that was read
 *
 * @return                  the number of bytes that were read
 * @return                  -1 on undefined device given
 * @return                  -2 on invalid address
 */
int i2c_read_byte(i2c_t dev, uint8_t address, char *data);

/**
 * @brief Read multiple bytes from an I2C device with the given address
 *
 * @param[in]  dev          I2C peripheral device
 * @param[in]  address      bus address of the target device
 * @param[out] data         array holding the received bytes
 * @param[in]  length       the number of bytes to read into `data`
 *
 * @return                  the number of bytes that were read
 * @return                  -1 on undefined device given
 */
int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length);

/**
 * @brief Read one byte from a register at the I2C slave with the given address
 *
 * @param[in]  dev          I2C peripheral device
 * @param[in]  address      bus address of the target device
 * @param[in]  reg          the register address on the targeted I2C device
 * @param[out] data         the result that was read
 *
 * @return                  the number of bytes that were read
 * @return                  -1 on undefined device given
 */
int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, char *data);

/**
 * @brief Read multiple bytes from a register at the I2C slave with the given address
 *
 * @param[in]  dev          I2C peripheral device
 * @param[in]  address      bus address of the target device
 * @param[in]  reg          the register address on the targeted I2C device
 * @param[out] data         array holding the received bytes
 * @param[in]  length       the number of bytes to read into `data`
 *
 * @return                  the number of bytes that were read
 * @return                  -1 on undefined device given
 */
int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length);

/**
 * @brief Write one byte to an I2C device with the given address
 *
 * @param[in] dev           I2C peripheral device
 * @param[in] address       bus address of the target device
 * @param[in] data          byte to write to the device
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int i2c_write_byte(i2c_t dev, uint8_t address, char data);

/**
 * @brief Write multiple bytes to an I2C device with the given address
 *
 * @param[in] dev           I2C peripheral device
 * @param[in] address       bus address of the target device
 * @param[in] data          array with bytes to write to the target device
 * @param[in] length        number of bytes to write to the target device
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length);

/**
 * @brief Write one byte to a register at the I2C slave with the given address
 *
 * @param[in] dev           I2C peripheral device
 * @param[in] address       bus address of the target device
 * @param[in] reg           the register address on the targeted I2C device
 * @param[in] data          byte to write to the device
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, char data);

/**
 * @brief Write multiple bytes to a register at the I2C slave with the given address
 *
 * @param[in] dev           I2C peripheral device
 * @param[in] address       bus address of the target device
 * @param[in] reg           the register address on the targeted I2C device
 * @param[in] data          array with bytes to write to the target device
 * @param[in] length        number of bytes to write to the target device
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length);

/**
 * @brief Power on the given I2C peripheral
 *
 * @param[in] dev           the I2C device to power on
 */
void i2c_poweron(i2c_t dev);

/**
 * @brief Power off the given I2C peripheral
 *
 * @param[in] dev           the I2C device to power off
 */
void i2c_poweroff(i2c_t dev);

#endif /* I2C_NUMOF */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H */
/** @} */
