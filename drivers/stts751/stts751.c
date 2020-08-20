/*
 * Copyright (C) 2020 Université Grenoble Alpes
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_stts751
 * @{
 * @brief       Device driver for the STTS751 I2C Temperature Sensor
 * @author      Pierre Millot
 * @file        stts751.c
 * @}
 */

#include <stdio.h>

#include "stts751.h"
#include "stts751_regs.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Write data to a register.
 *
 * @param[in] dev       Device descriptor of the STTS751
 * @param[in] reg       Register address to write to
 * @param[in] data      Data to write
 *
 * @return  0 on success
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
int _write_reg(const stts751_t *dev, uint8_t reg, uint8_t data)
{
    i2c_t i2c_dev = dev->params.i2c_dev;

    if (i2c_acquire(i2c_dev) != 0) {
        return -1;
    }
    int rc = i2c_write_reg(i2c_dev, dev->params.i2c_addr, reg, data, 0);
    i2c_release(i2c_dev);

    return rc;
}

/**
 * @brief Read data from a register.
 *
 * @param[in] dev       Device descriptor of the STTS751
 * @param[in] reg       Register address to read from
 *
 * @return  0 on success
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
static int _read_reg(const stts751_t *dev, uint8_t reg)
{
    i2c_t i2c_dev = dev->params.i2c_dev;

    if (i2c_acquire(i2c_dev) != 0) {
        return -1;
    }
    uint8_t data;
    i2c_read_reg(i2c_dev, dev->params.i2c_addr, reg, &data, 0);
    i2c_release(i2c_dev);

    return data;
}

int stts751_init(stts751_t *dev, const stts751_params_t *params)
{
    assert(dev);
    assert(params);

    dev->params = *params;

    i2c_init(dev->params.i2c_dev);

    return 0;
}

double stts751_get_temperature(const stts751_t *dev)
{
    uint8_t high = _read_reg(dev, STTS751_REG_TEMP_H);
    uint8_t low = _read_reg(dev, STTS751_REG_TEMP_L);

    return ((high << 8) | low) / 256.0;
}
