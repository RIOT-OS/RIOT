/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_fe310
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <inttypes.h>
#include <assert.h>
#include <errno.h>

#include "cpu.h"
#include "mutex.h"

#include "periph/i2c.h"
#include "periph_conf.h"

#include "vendor/i2c.h"
#include "vendor/platform.h"
#include "vendor/prci_driver.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define I2C_BUSY_TIMEOUT    (0xffff)
static const uint16_t _fe310_i2c_speed[2] = { 100U, 400U };

static inline int _wait_busy(i2c_t dev, uint32_t max_timeout_counter);
static inline int _start(i2c_t dev, uint16_t address);
static inline int _read(i2c_t dev, uint8_t *data, int length, uint8_t stop);
static inline int _write(i2c_t dev, const uint8_t *data, int length,
                         uint8_t stop);

/**
 * @brief   Initialized bus locks
 */
static mutex_t locks[I2C_NUMOF];

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    /* Initialize mutex */
    mutex_init(&locks[dev]);

    /* Select IOF0 */
    GPIO_REG(GPIO_IOF_SEL) &=
        ~((1 << i2c_config[dev].scl) | (1 << i2c_config[dev].sda));
    /* Enable IOF */
    GPIO_REG(GPIO_IOF_EN) |=
        ((1 << i2c_config[dev].scl) | (1 << i2c_config[dev].sda));

    _REG32(i2c_config[dev].addr,
           I2C_CONTROL) &= ~(I2C_CONTROL_IE | I2C_CONTROL_EN);

    /* Compute prescale: presc = (CORE_CLOCK / (5 * I2C_SPEED)) - 1 */
    uint16_t presc =
        ((uint16_t)(cpu_freq() / 1000) /
         (5 * _fe310_i2c_speed[i2c_config[dev].speed])) - 1;

    DEBUG("[i2c] init: computed prescale: %i (0x%02X|0x%02X)\n", presc,
          (presc >> 8), (presc & 0xFF));

    _REG32(i2c_config[dev].addr, I2C_PRESCALE_LO) = (presc & 0xFF);
    _REG32(i2c_config[dev].addr, I2C_PRESCALE_HI) = (presc >> 8);

    DEBUG("[i2c] init: stored prescale: (0x%02X|0x%02X)\n",
          (uint8_t)(_REG32(i2c_config[dev].addr, I2C_PRESCALE_HI) & 0xFF),
          (uint8_t)(_REG32(i2c_config[dev].addr, I2C_PRESCALE_LO) & 0xFF));

    _REG32(i2c_config[dev].addr, I2C_CONTROL) = I2C_CONTROL_EN;

    DEBUG("[i2c] init: control reg 0x%08X)\n",
          (unsigned)_REG32(i2c_config[dev].addr, I2C_CONTROL));

    DEBUG("[i2c] initialization done\n");
}

int i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    mutex_lock(&locks[dev]);
    return 0;
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    mutex_unlock(&locks[dev]);
}

int i2c_read_bytes(i2c_t dev, uint16_t address, void *data, size_t length,
                   uint8_t flags)
{
    assert(length > 0);
    assert(dev < I2C_NUMOF);

    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }

    /* Check for wrong arguments given */
    if (data == NULL || length == 0) {
        return -EINVAL;
    }

    DEBUG("[i2c] read bytes\n");

    int ret = 0;

    if (!(flags & I2C_NOSTART)) {
        ret = _start(dev, ((address << 1) | I2C_READ));
        if (ret < 0) {
            DEBUG("[i2c] Error: start command failed\n");
            return ret;
        }
    }

    /* read data and issue stop if needed */
    ret = _read(dev, data, length, (flags & I2C_NOSTOP) ? 0 : 1);
    if (ret < 0) {
        DEBUG("[i2c] Error: read command failed\n");
        return ret;
    }

    _wait_busy(dev, I2C_BUSY_TIMEOUT);

    return length;
}

int i2c_write_bytes(i2c_t dev, uint16_t address, const void *data,
                    size_t length,
                    uint8_t flags)
{
    assert(dev < I2C_NUMOF);

    int ret = 0;

    /* Check for unsupported operations */
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }
    /* Check for wrong arguments given */
    if (data == NULL || length == 0) {
        return -EINVAL;
    }

    DEBUG("[i2c] write bytes\n");

    if (!(flags & I2C_NOSTART)) {
        ret = _start(dev, (address << 1));
        if (ret < 0) {
            DEBUG("[i2c] error: start command failed\n");
            return ret;
        }
    }

    ret = _write(dev, data, length, (flags & I2C_NOSTOP) ? 0 : 1);
    if (ret < 0) {
        DEBUG("[i2c] error: write command failed\n");
        return ret;
    }

    return 0;
}

static inline int _wait_busy(i2c_t dev, uint32_t max_timeout_counter)
{
    uint32_t timeout_counter = 0;

    DEBUG("[i2c] wait for transfer\n");
    while (_REG32(i2c_config[dev].addr, I2C_STATUS) & I2C_STATUS_TIP) {
        if (++timeout_counter >= max_timeout_counter) {
            DEBUG("[i2c] transfer timeout\n");
            return -ETIMEDOUT;
        }
        else if ((_REG32(i2c_config[dev].addr,
                         I2C_STATUS) & I2C_STATUS_ALOST) == I2C_STATUS_ALOST) {
            /* Arbitration lost */
            DEBUG("[i2c] error: Arbitration lost\n");
            return -EAGAIN;
        }
    }
    return 0;
}

static inline int _start(i2c_t dev, uint16_t address)
{
    _wait_busy(dev, I2C_BUSY_TIMEOUT);

    /* start transmission */
    DEBUG("[i2c] write slave address, 0x%02X\n", address);
    _REG32(i2c_config[dev].addr, I2C_DATA) = address;

    DEBUG("[i2c] send start condition\n");
    _REG32(i2c_config[dev].addr, I2C_CMD) = (I2C_CMD_STA | I2C_CMD_WR);

    /* Ensure all bytes has been read */
    int ret = _wait_busy(dev, I2C_BUSY_TIMEOUT);

    if (ret < 0) {
        return ret;
    }

    return 0;
}

static inline int _read(i2c_t dev, uint8_t *data, int length, uint8_t stop)
{
    uint8_t count = 0;

    /* Read data buffer. */
    while (length--) {
        uint8_t command = I2C_CMD_RD;

        /* Wait for hardware module to sync */
        int ret = _wait_busy(dev, I2C_BUSY_TIMEOUT);
        if (ret < 0) {
            return ret;
        }

        if (length == 0) {
            /* Send NACK before STOP */
            command |= I2C_CMD_ACK;
            /* Prepare stop command */
            if (stop) {
                command |= I2C_CMD_STO;
            }
        }

        _REG32(i2c_config[dev].addr, I2C_CMD) = command;

        /* Wait for response on bus. */
        ret = _wait_busy(dev, I2C_BUSY_TIMEOUT);
        if (ret < 0) {
            return ret;
        }

        data[count] = (uint32_t)(_REG32(i2c_config[dev].addr, I2C_DATA));
        DEBUG("[i2c] read byte #%i, 0x%02X\n", count, data[count]);

        count++;
    }
    return 0;
}

static inline int _write(i2c_t dev, const uint8_t *data, int length,
                         uint8_t stop)
{
    uint8_t count = 0;

    /* Write data buffer until the end. */
    while (length--) {
        /* Wait for hardware module to sync */
        int ret = _wait_busy(dev, I2C_BUSY_TIMEOUT);
        if (ret < 0) {
            return ret;
        }

        uint8_t command = I2C_CMD_WR;

        DEBUG("[i2c] write byte #%i, 0x%02X\n", count, data[count]);
        _REG32(i2c_config[dev].addr, I2C_DATA) = data[count++];

        /* Check if this is the last byte to read */
        if ((length == 0) && stop) {
            command |= I2C_CMD_STO;
        }

        _REG32(i2c_config[dev].addr, I2C_CMD) = command;

        ret = _wait_busy(dev, I2C_BUSY_TIMEOUT);
        if (ret < 0) {
            return ret;
        }
    }

    return 0;
}
