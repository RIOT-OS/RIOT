/*
 * Copyright (C) Daniel Krebs
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Driver for the AT30TSE75x temperature sensor with serial EEPROM
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 */


#include "periph/i2c.h"
#include "xtimer.h"

#include "at30tse75x.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static inline float temperature_to_float(uint16_t temp)
{
    /* Integer part is 8-bit signed */
    int8_t temp_int = (temp & AT30TSE75X_INTEGER_MASK) >>
                        AT30TSE75X_INTEGER_SHIFT;
    /* Fractional part is multiple of 0.0625 */
    uint8_t frac_multiplier = (temp & AT30TSE75X_FRACTIONAL_MASK) >>
                        AT30TSE75X_FRACTIONAL_SHIFT;

    return temp_int + (frac_multiplier * AT30TSE75X_FRACTIONAL_BASE);
}

static int at30tse75x_get_register(at30tse75x_t* dev, uint8_t reg, uint16_t* data)
{
    i2c_acquire(dev->i2c);
    xtimer_spin(AT30TSE75X_BUS_FREE_TIME_US);
    if(i2c_read_regs(dev->i2c, dev->addr, reg, (char*) data, 2) <= 0) {
        DEBUG("[at30tse75x] Can't read register 0x%x\n", reg);
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}

static int at30tse75x_set_register(at30tse75x_t* dev, uint8_t reg, uint16_t* data)
{
    i2c_acquire(dev->i2c);
    xtimer_spin(AT30TSE75X_BUS_FREE_TIME_US);
    if(i2c_write_regs(dev->i2c, dev->addr, reg, (char*) data, 2) <= 0) {
        DEBUG("[at30tse75x] Can't write to register 0x%x\n", reg);
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}

static int at30tse75x_reset(at30tse75x_t* dev)
{
    i2c_acquire(dev->i2c);
    xtimer_spin(AT30TSE75X_BUS_FREE_TIME_US);
    if(i2c_write_byte(dev->i2c, 0x00, AT30TSE75X_CMD__GENERAL_CALL_RESET) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);
    /* Wait for reset to complete */
    xtimer_usleep(500);
    return 0;
}

int at30tse75x_get_config(at30tse75x_t* dev, uint8_t* data)
{
    i2c_acquire(dev->i2c);
    xtimer_spin(AT30TSE75X_BUS_FREE_TIME_US);
    if(i2c_read_reg(dev->i2c, dev->addr, AT30TSE75X_REG__CONFIG, (char*) data) <= 0) {
        DEBUG("[at30tse75x] Can't read CONFIG register\n");
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}

int at30tse75x_set_config(at30tse75x_t* dev, uint8_t data)
{
    i2c_acquire(dev->i2c);
    xtimer_spin(AT30TSE75X_BUS_FREE_TIME_US);
    if(i2c_write_reg(dev->i2c, dev->addr, AT30TSE75X_REG__CONFIG, (char) data) <= 0) {
        DEBUG("[at30tse75x] Can't write to CONFIG register\n");
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}

int at30tse75x_set_resolution(at30tse75x_t* dev, at30tse75x_resolution_t resolution)
{
    uint8_t config;

    if(resolution < AT30TSE75X_RESOLUTION_9BIT ||
       resolution > AT30TSE75X_RESOLUTION_12BIT) {
        return -2;
    }

    if(at30tse75x_get_config(dev, &config) != 0) {
        return -1;
    }

    config &= ~(AT30TSE75X_CONFIG__RESOLUTION_MASK);
    config |= resolution << AT30TSE75X_CONFIG__RESOLUTION_SHIFT;

    if(at30tse75x_set_config(dev, config) != 0) {
        return -1;
    }

    return 0;
}

int at30tse75x_set_mode(at30tse75x_t* dev, at30tse75x_mode_t mode)
{
    uint8_t config;
    if(at30tse75x_get_config(dev, &config) != 0) {
        return -1;
    }

    switch(mode) {
    case AT30TSE75X_MODE_ONE_SHOT:
        config |= AT30TSE75X_CONFIG__SHUTDOWN_BIT;
        /* Don't touch alarm mode */
        break;
    case AT30TSE75X_MODE_COMPARATOR:
        config &= ~(AT30TSE75X_CONFIG__SHUTDOWN_BIT);
        config &= ~(AT30TSE75X_CONFIG__ALARM_MODE_BIT);
        break;
    case AT30TSE75X_MODE_INTERRUPT:
        config &= ~(AT30TSE75X_CONFIG__SHUTDOWN_BIT);
        config |= AT30TSE75X_CONFIG__ALARM_MODE_BIT;
        break;
    default:
        return -2;
    }

    if(at30tse75x_set_config(dev, config) != 0) {
        return -1;
    }

    return 0;
}

int at30tse75x_set_alarm_polarity(at30tse75x_t* dev, at30tse75x_alarm_polatity_t polarity)
{
    uint8_t config;
    if(at30tse75x_get_config(dev, &config) != 0) {
        return -1;
    }

    switch(polarity) {
    case AT30TSE75X_ALARM_ACTIVE_HIGH:
        config |= AT30TSE75X_CONFIG__ALERT_POL_BIT;
        break;
    case AT30TSE75X_ALARM_ACTIVE_LOW:
        config &= ~(AT30TSE75X_CONFIG__ALERT_POL_BIT);
        break;
    default:
        return -2;
    }

    if(at30tse75x_set_config(dev, config) != 0) {
            return -1;
        }

    return 0;
}

int at30tse75x_set_fault_tolerance(at30tse75x_t* dev, at30tse75x_fault_tolerance_t tolerance)
{
    if(tolerance < AT30TSE75X_ALARM_AFTER_1 ||
       tolerance > AT30TSE75X_ALARM_AFTER_6) {
        return -2;
    }

    uint8_t config;
    if(at30tse75x_get_config(dev, &config) != 0) {
        return -1;
    }

    config &= ~(AT30TSE75X_CONFIG__FTQ_MASK);
    config |= tolerance << AT30TSE75X_CONFIG__FTQ_SHIFT;

    if(at30tse75x_set_config(dev, config) != 0) {
        return -1;
    }

    return 0;
}

int at30tse75x_set_limit_low(at30tse75x_t* dev, int8_t t_low)
{
    uint16_t tmp = (t_low << 8) | (0x00);
    return at30tse75x_set_register(dev, AT30TSE75X_REG__LIMIT_LOW, &tmp);
}

int at30tse75x_set_limit_high(at30tse75x_t* dev, int8_t t_high)
{
    uint16_t tmp = (t_high << 8) | (0x00);
    return at30tse75x_set_register(dev, AT30TSE75X_REG__LIMIT_HIGH, &tmp);
}

int at30tse75x_save_config(at30tse75x_t* dev)
{
    i2c_acquire(dev->i2c);
    xtimer_spin(AT30TSE75X_BUS_FREE_TIME_US);
    if(i2c_write_byte(dev->i2c, dev->addr, AT30TSE75X_CMD__SAVE_TO_NVRAM) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);
    /* Wait for copy to complete */
    xtimer_usleep(5000);
    return 0;
}

int at30tse75x_restore_config(at30tse75x_t* dev)
{
    i2c_acquire(dev->i2c);
    xtimer_spin(AT30TSE75X_BUS_FREE_TIME_US);
    if(i2c_write_byte(dev->i2c, dev->addr, AT30TSE75X_CMD__RESTORE_FROM_NVRAM) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);
    /* Wait for copy to complete */
    xtimer_usleep(200);
    return 0;
}

int at30tse75x_get_temperature(at30tse75x_t* dev, float* temperature)
{
    uint16_t tmp;
    uint8_t config;

    if(at30tse75x_get_config(dev, &config) != 0) {
        return -1;
    }

    /* If sensor is shutdown trigger One-Shot mode*/
    if(config & AT30TSE75X_CONFIG__SHUTDOWN_BIT) {

        config |= AT30TSE75X_CONFIG__OS_BIT;
        if(at30tse75x_set_config(dev, config) != 0) {
            return -1;
        }

        /* Use resolution to calculate conversion time */
         uint8_t resolution = (config & AT30TSE75X_CONFIG__RESOLUTION_MASK) >>
                                   AT30TSE75X_CONFIG__RESOLUTION_SHIFT;

        /* Wait until conversion is finished */
        xtimer_usleep((uint32_t)(25000 << resolution));
    }

    /* Read temperature */
    if(at30tse75x_get_register(dev, AT30TSE75X_REG__TEMPERATURE, &tmp) != 0) {
        return -1;
    }

    /* Convert fixed point to float */
    *temperature = temperature_to_float(tmp);

    return 0;
}

int at30tse75x_init(at30tse75x_t* dev, i2c_t i2c, i2c_speed_t speed, uint8_t addr)
{
    uint8_t config;

    dev->i2c = i2c;

    if( (addr < 0x48) || (addr > 0x4f) ) {
        DEBUG("[at30tse75x] Invalid address\n");
        return -2;
    }
    dev->addr = addr;

    i2c_acquire(dev->i2c);
    if(i2c_init_master(dev->i2c, speed) != 0) {
        DEBUG("[at30tse75x] Can't initialize I2C master\n");
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    /* Reset the device */
    if(at30tse75x_reset(dev) != 0) {
        return -1;
    }

    /* Poll the device, fail if unavailable */
    if(at30tse75x_get_config(dev, &config) != 0) {
        return -1;
    }

    DEBUG("[at30tse75x] Config: 0x%x\n", config);

    return 0;
}
