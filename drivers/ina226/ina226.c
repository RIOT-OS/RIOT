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

/** @brief Read one 16 bit register from a INA226 device and swap byte order, if necessary. */
static int ina226_read_reg(ina226_t *dev, uint8_t reg, uint16_t *out)
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

/** @brief Write one 16 bit register to a INA226 device and swap byte order, if necessary. */
static int ina226_write_reg(ina226_t *dev, uint8_t reg, uint16_t in)
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

int ina226_read_calibration(ina226_t *dev, uint16_t *calibration)
{
    return ina226_read_reg(dev, INA226_REG_CALIBRATION, calibration);
}

int ina226_write_calibration(ina226_t *dev, uint16_t calibration)
{
    return ina226_write_reg(dev, INA226_REG_CALIBRATION, calibration);
}

int ina226_read_config(ina226_t *dev, uint16_t *config)
{
    return ina226_read_reg(dev, INA226_REG_CONFIGURATION, config);   
}

int ina226_write_config(ina226_t *dev, uint16_t config)
{
    return ina226_write_reg(dev, INA226_REG_CONFIGURATION, config);
}

int ina226_read_shunt(ina226_t *dev, int16_t *voltage)
{
    return ina226_read_reg(dev, INA226_REG_SHUNT_VOLTAGE, (uint16_t *)voltage);
}

int ina226_read_bus(ina226_t *dev, int16_t *voltage)
{
    return ina226_read_reg(dev, INA226_REG_BUS_VOLTAGE, (uint16_t *)voltage);
}

int ina226_read_current(ina226_t *dev, int16_t *current)
{
    return ina226_read_reg(dev, INA226_REG_CURRENT, (uint16_t *)current);
}

int ina226_read_power(ina226_t *dev, int16_t *power)
{
    return ina226_read_reg(dev, INA226_REG_POWER, (uint16_t *)power);
}

int ina226_read_die_id(ina226_t *dev, uint16_t *id)
{
    return ina226_read_reg(dev, INA226_REG_DIE_ID, id);
}

int ina226_read_manufacturer_id(ina226_t *dev, uint16_t *id)
{
    return ina226_read_reg(dev, INA226_REG_MANUFACTURER_ID, id);
}

int ina226_read_mask_enable(ina226_t *dev, uint16_t *me_val)
{
    return ina226_read_reg(dev, INA226_REG_MASK_ENABLE, me_val);
}

int ina226_write_mask_enable(ina226_t *dev, uint16_t me_val)
{
    return ina226_write_reg(dev, INA226_REG_MASK_ENABLE, me_val);
}

int ina226_read_alert_limit(ina226_t *dev, uint16_t *alert_limit)
{
    return ina226_read_reg(dev, INA226_REG_ALERT_LIMIT, alert_limit);
}

int ina226_write_alert_limit(ina226_t *dev, uint16_t alert_limit)
{
    return ina226_write_reg(dev, INA226_REG_ALERT_LIMIT, alert_limit);
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
