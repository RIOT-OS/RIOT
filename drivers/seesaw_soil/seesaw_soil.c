/*
 * Copyright (C) 2020 Viktor Gal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_seesaw_soil
 * @{
 *
 * @file
 * @brief       Driver for the Adafruit Seesaw Soil Moisture and Temperature Sensor.
 *
 * @author      Viktor Gal <viktor.gal@maeth.com>
 *
 * @}
 */

#include <string.h>

#include "byteorder.h"
#include "xtimer.h"
#include "periph/i2c.h"
#include "seesaw_soil.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _seesaw_read_regs(const seesaw_soil_t *dev, uint16_t reg_addr, void* data, size_t len) {
    int r;

    reg_addr = htons(reg_addr);
    /* send the 16bit register address we want to read with STOP bit */
    if ((r = i2c_write_bytes(dev->params.i2c, dev->params.addr,
                             &reg_addr, 2, 0)) < 0) {
        return r;
    }

    /* wait for the answer */
    xtimer_usleep(5000);

    /* and now we read the register value */
    if ((r = i2c_read_bytes(dev->params.i2c, dev->params.addr,
                           data, len, 0)) < 0) {
        return r;
    }

    return r;
}

static int _seesaw_read_reg(seesaw_soil_t *dev, uint16_t reg_addr, void* data) {
    return _seesaw_read_regs(dev, reg_addr, data, 1);
}

int seesaw_soil_init(seesaw_soil_t *dev, const seesaw_soil_params_t *params)
{
    uint8_t reg;

    /* write device descriptor */
    dev->params = *params;

    DEBUG("[SEESAW SOIL] Initializing seesaw soil device.\n");
    /* try if we can interact with the device by reading its manufacturer ID */
    i2c_acquire(dev->params.i2c);
    if (_seesaw_read_reg(dev, SEESAW_SOIL_MANUFACTURER_ID, &reg) < 0) {
        DEBUG("[SEESAW SOIL] Could not read manufacture id.\n");
        i2c_release(dev->params.i2c);
        return SEESAW_SOIL_NOBUS;
    }

    if (reg != SEESAW_SOIL_MID_VALUE) {
        DEBUG("[SEESAW SOIL] Wrong manufacture id: %x\n", reg);
        i2c_release(dev->params.i2c);
        return SEESAW_SOIL_NODEV;
    }
    i2c_release(dev->params.i2c);

    /* all set */
    return SEESAW_SOIL_OK;
}

int seesaw_soil_temperature(const seesaw_soil_t *dev, int16_t *temp)
{
    int status = SEESAW_SOIL_OK;
    uint32_t raw_temp;

    i2c_acquire(dev->params.i2c);
    if (_seesaw_read_regs(dev, SEESAW_SOIL_TEMPERATURE, &raw_temp, sizeof(raw_temp)) < 0) {
        status = SEESAW_SOIL_BUSERR;
    }
    i2c_release(dev->params.i2c);

    if (temp && (status == SEESAW_SOIL_OK)) {
        *temp = (ntohl(raw_temp) * 100) >> 16;
    }

    return status;
}

int seesaw_soil_moisture(const seesaw_soil_t *dev, uint16_t *moist)
{
    int status = SEESAW_SOIL_OK;
    uint16_t raw_moist;

    i2c_acquire(dev->params.i2c);
    if (_seesaw_read_regs(dev, SEESAW_SOIL_MOISTURE, &raw_moist, 2) < 0) {
        status = SEESAW_SOIL_BUSERR;
    }
    i2c_release(dev->params.i2c);

    if (moist && (status == SEESAW_SOIL_OK)) {
        *moist = ntohs(raw_moist);
    }
    return status;
}
