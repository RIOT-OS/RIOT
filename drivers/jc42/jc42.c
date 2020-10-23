/*
 * Copyright (C) 2017 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_jc42
 * @{
 *
 * @file
 * @brief       Device driver implementation for the JEDEC jc42.4 compliant temperature sensors
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */


#include "periph/i2c.h"
#include "byteorder.h"

#include "jc42.h"
#include "jc42_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int jc42_get_register(const jc42_t* dev, uint8_t reg, uint16_t* data)
{
    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, reg, data, 2, 0) != 0) {
        DEBUG("[jc42] Problem reading register 0x%x\n", reg);
        i2c_release(dev->i2c);
        return JC42_NODEV;
    }
    i2c_release(dev->i2c);
    return JC42_OK;
}

static int jc42_set_register(const jc42_t* dev, uint8_t reg, uint16_t* data)
{
    i2c_acquire(dev->i2c);
    if (i2c_write_regs(dev->i2c, dev->addr, reg, data, 2, 0) != 0) {
        DEBUG("[jc42] Problem writing to register 0x%x\n", reg);
        i2c_release(dev->i2c);
        return JC42_NODEV;
    }
    i2c_release(dev->i2c);

    return JC42_OK;
}

int jc42_get_config(const jc42_t* dev, uint16_t* data)
{
    return jc42_get_register(dev, JC42_REG_CONFIG, data);
}

int jc42_set_config(const jc42_t* dev, uint16_t data)
{
    return jc42_set_register(dev, JC42_REG_CONFIG, &data);
}

int jc42_get_temperature(const jc42_t* dev, int16_t* temperature)
{
    struct { signed int x:12;} s;
    uint16_t tmp;

    /* Read temperature */
    if (jc42_get_register(dev, JC42_REG_TEMP, &tmp) != 0) {
        return JC42_NODEV;
    }
    tmp = ntohs(tmp);
    /* Convert fixed point to uint16_t */
    *temperature = ((s.x = tmp)*100)>>4;
    return JC42_OK;
}

int jc42_init(jc42_t* dev, const jc42_params_t* params)
{
    uint16_t config;
    dev->i2c = params->i2c;
    dev->addr = params->addr;

    /* Poll the device, fail if unavailable */
    if (jc42_get_config(dev, &config) != 0) {
        return JC42_NODEV;
    }

    DEBUG("[jc42] Config: 0x%x\n", config);

    return JC42_OK;
}
