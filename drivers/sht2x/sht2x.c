/*
 * Copyright (C) 2017 George Psimenos (gp7g14@soton.ac.uk)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sht2x
 * @{
 *
 * @file
 * @brief       Driver for the Sensirion SHT2x sensor series
 *
 * @author      George Psimenos <gp7g14@soton.ac.uk>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "assert.h"
#include "periph/i2c.h"
#include "xtimer.h"
#include "sht2x.h"
#include "sht2x_regs.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#define I2C_SPEED           I2C_SPEED_FAST
#define MAX_RETRIES         20

#define BUS                 (dev->params.i2c)
#define ADDR                (dev->params.addr)

int sht2x_init(sht2x_t *dev, const sht2x_params_t *params)
{
    uint8_t result;
    uint8_t user_reg;

    assert(dev && params);
    assert(params->resolution == SHT2x_RES_12_14BIT || 
           params->resolution == SHT2x_RES_8_12BIT || 
           params->resolution == SHT2x_RES_10_13BIT || 
           params->resolution == SHT2x_RES_11_11BIT );

    /* write device descriptor */
    DEBUG("[sht2x] init: copying dev descriptor\n");
    memcpy(dev, params, sizeof(sht2x_params_t));

    /* initialize the I2C bus */
    DEBUG("[sht2x] init: acquiring I2C\n");
    i2c_acquire(BUS);
    DEBUG("[sht2x] init: setting master mode\n");
    if (i2c_init_master(BUS, I2C_SPEED) < 0) {
        i2c_release(BUS);
        DEBUG("[sht2x] init - error: unable to initialize I2C bus\n");
        return SHT2X_NOI2C;
    }

    // reset the device - send a soft reset command
    DEBUG("[sht2x] init: issuing soft reset\n");
    result = i2c_write_byte(BUS, ADDR, SHT2X_SOFT_RESET);
    if (result < 1) {
        i2c_release(BUS);
        DEBUG("[sht2x] init - error: soft reset failed\n");
        return SHT2X_NODEV;
    }

    // wait 15 ms for device to reset
    DEBUG("[sht2x] init: waiting\n");
    i2c_release(BUS);
    xtimer_usleep(15 * 1000);
    i2c_acquire(BUS);

    // check device's responsiveness and identity by
    // reading the user register after the reset we did
    // and comparing with the default value which is 0x02
    DEBUG("[sht2x] init: reading user register\n");
    result = i2c_read_reg(BUS, ADDR, SHT2X_USER_REG_R, &user_reg);
    if (result < 1 || (user_reg & ~SHT2X_USER_RESERVED_MASK) != 0x02) {
        i2c_release(BUS);
        DEBUG("[sht2x] init - error: can't read user_reg or wrong value [0x%02x]\n", (int)user_reg);
        return SHT2X_NODEV;
    }

    // write desired resolution to user register
    // while keeping the rest of the register the same
    // (only if current resolution != desired resolution)
    DEBUG("[sht2x] init: setting resolution\n");
    if ((user_reg & SHT2X_USER_RESOLUTION_MASK) != dev->params.resolution) {
        result = i2c_write_reg(BUS, ADDR, SHT2X_USER_REG_W,
            (dev->params.resolution | (user_reg & ~SHT2X_USER_RESOLUTION_MASK)));
        if (result < 1) {
            i2c_release(BUS);
            DEBUG("[sht2x] init - error: user register write failed\n");
            return SHT2X_NODEV;
        }
    }

    DEBUG("[sht2x] init: releasing I2C\n");
    i2c_release(BUS);

    DEBUG("[sht2x] init: successful\n");

    return SHT2X_OK;
}

// Returns °C (x100)
int16_t sht2x_read_temp(sht2x_t *dev)
{
    uint8_t result;
    char buffer[3];
    
    uint16_t st;
    float coeff;
    float temp;

    assert(dev);

    // acquire I2C bus
    DEBUG("[sht2x] read_temp: acquiring I2C\n");
    i2c_acquire(BUS);

    // trigger measurement
    DEBUG("[sht2x] read_temp: triggering no hold master temp read\n");
    result = i2c_write_byte(BUS, ADDR, SHT2X_TRIG_T_MEASUREMENT_POLL);
    if (result < 1) {
        i2c_release(BUS);
        DEBUG("[sht2x] read_temp - error: i2c write failed\n");
        return SHT2X_NODEV;
    }

    // sleep for the typical time it takes to complete the measurement
    // this depends on the resolution and is taken from the datasheet
    switch(dev->params.resolution) {
        case SHT2x_RES_12_14BIT:
            xtimer_usleep(66 * 1000);
            break;
        case SHT2x_RES_8_12BIT:
            xtimer_usleep(17 * 1000);
            break;
        case SHT2x_RES_10_13BIT:
            xtimer_usleep(33 * 1000);
            break;
        case SHT2x_RES_11_11BIT:
            xtimer_usleep(9 * 1000);
            break;
        default:
            // this case is impossible
            i2c_release(BUS);
            DEBUG("[sht2x] read_temp - error: you what???\n");
            return SHT2X_OTHERERROR;
    }
    
    uint8_t retries = 0;
    result = i2c_read_bytes(BUS, ADDR, buffer, 3);
    while (result < 3) {
        xtimer_usleep(1000);
        retries++;
        if (retries > MAX_RETRIES) {
            i2c_release(BUS);
            DEBUG("[sht2x] read_temp - error: sensor read timed out\n");
            return SHT2X_READERROR;
        }
        result = i2c_read_bytes(BUS, ADDR, buffer, 3);
    }

    DEBUG("[sht2x] read_temp: releasing I2C\n");
    i2c_release(BUS);

    // ** TODO **
    // verify checksum

    st = buffer[0] << 8 | buffer[1];
    coeff = st / 65536.0;

    temp = -46.85 + 175.72*coeff;

    return (int16_t)(temp * 100);
}

// Returns % RH (x100)
int16_t sht2x_read_humidity(sht2x_t *dev)
{
    uint8_t result;
    char buffer[3];
    
    uint16_t srh;
    float coeff;
    float humidity;

    assert(dev);

    // acquire I2C bus
    DEBUG("[sht2x] read_humidity: acquiring I2C\n");
    i2c_acquire(BUS);

    // trigger measurement
    DEBUG("[sht2x] read_humidity: triggering no hold master humidity read\n");
    result = i2c_write_byte(BUS, ADDR, SHT2X_TRIG_RH_MEASUREMENT_POLL);
    if (result < 1) {
        i2c_release(BUS);
        DEBUG("[sht2x] read_humidity - error: i2c write failed\n");
        return SHT2X_NODEV;
    }

    // sleep for the typical time it takes to complete the measurement
    // this depends on the resolution and is taken from the datasheet
    switch(dev->params.resolution) {
        case SHT2x_RES_12_14BIT:
            xtimer_usleep(22 * 1000);
            break;
        case SHT2x_RES_8_12BIT:
            xtimer_usleep(3 * 1000);
            break;
        case SHT2x_RES_10_13BIT:
            xtimer_usleep(7 * 1000);
            break;
        case SHT2x_RES_11_11BIT:
            xtimer_usleep(12 * 1000);
            break;
        default:
            // this case is impossible
            i2c_release(BUS);
            DEBUG("[sht2x] read_humidity - error: you what???\n");
            return SHT2X_OTHERERROR;
    }
    
    uint8_t retries = 0;
    result = i2c_read_bytes(BUS, ADDR, buffer, 3);
    while (result < 3) {
        xtimer_usleep(1000);
        retries++;
        if (retries > MAX_RETRIES) {
            i2c_release(BUS);
            DEBUG("[sht2x] read_humidity - error: sensor read timed out\n");
            return SHT2X_READERROR;
        }
        result = i2c_read_bytes(BUS, ADDR, buffer, 3);
    }

    DEBUG("[sht2x] read_humidity: releasing I2C\n");
    i2c_release(BUS);

    // ** TODO **
    // verify checksum

    srh = buffer[0] << 8 | buffer[1];
    coeff = srh / 65536.0;

    humidity = -6.0 + 125.0*coeff;

    return (int16_t)(humidity * 100);
}