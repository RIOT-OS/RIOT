/*
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sensors
 * @{
 * @file
 * @brief       Device driver for Texas Instruments INA226 High-Side or Low-Side Measurement,
 *              Bi-Directional Current and Power Monitor with I2C Compatible Interface
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @}
 */

#include <stdint.h>

#include "ina226.h"
#include "ina226-regs.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "byteorder.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"


int ina226_read_reg(ina226_t *dev, uint8_t reg, uint16_t *out)
{
    union {
        char c[2];
        uint16_t u16;
    } tmp = { .u16 = 0 };
    int status = 0;

    status = i2c_read_regs(dev->i2c, dev->addr, reg, &tmp.c[0], 2);

    if (status != 2) {
        return -1;
    }

    *out = NTOHS(tmp.u16);
    return 0;
}

int ina226_write_reg(ina226_t *dev, uint8_t reg, uint16_t in)
{
    union {
        char c[2];
        uint16_t u16;
    } tmp = { .u16 = 0 };
    int status = 0;

    tmp.u16 = HTONS(in);

    status = i2c_write_regs(dev->i2c, dev->addr, reg, &tmp.c[0], 2);

    if (status != 2) {
        return -1;
    }

    return 0;
}

int ina226_init(ina226_t *dev, i2c_t i2c, uint8_t address)
{
    /* write device descriptor */
    dev->i2c = i2c;
    dev->addr = address;
    return 0;
}

int ina226_activate_int(ina226_t *dev, uint16_t me_config, gpio_t pin, gpio_cb_t callback)
{
    if (ina226_write_reg(dev, INA226_REG_MASK_ENABLE, me_config) == 0 ) {
        return gpio_init_int(pin, GPIO_IN_PU, GPIO_FALLING, callback, dev);
    }
    else {
        return -1;
    }
}
