/*
 * Copyright (C) 2017 HAW Hamburg
 *               2018 Freie Universität Berlin
 *               2018 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * @file
 * @brief       Low-level I2C (TWI) peripheral driver implementation
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <assert.h>
#include <string.h>
#include <errno.h>

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/**
 * @brief   If any of the 8 lower bits are set, the speed value is invalid
 */
#define INVALID_SPEED_MASK  (0xff)

/**
 * @brief   Initialized dev locks (we have a maximum of two devices...)
 */
static mutex_t locks[I2C_NUMOF];

static inline NRF_TWIM_Type *bus(i2c_t dev)
{
    return i2c_config[dev].dev;
}

static int finish(i2c_t dev)
{
    DEBUG("[i2c] waiting for STOPPED or ERROR event\n");
    while ((!(bus(dev)->EVENTS_STOPPED)) && (!(bus(dev)->EVENTS_ERROR))) {
        nrf52_sleep();
    }

    if ((bus(dev)->EVENTS_STOPPED)) {
        bus(dev)->EVENTS_STOPPED = 0;
        DEBUG("[i2c] finish: stop event occurred\n");
    }

    if (bus(dev)->EVENTS_ERROR) {
        bus(dev)->EVENTS_ERROR = 0;
        if (bus(dev)->ERRORSRC & TWIM_ERRORSRC_ANACK_Msk) {
            bus(dev)->ERRORSRC = TWIM_ERRORSRC_ANACK_Msk;
            DEBUG("[i2c] check_error: NACK on address byte\n");
            return -ENXIO;
        }
        if (bus(dev)->ERRORSRC & TWIM_ERRORSRC_DNACK_Msk) {
            bus(dev)->ERRORSRC = TWIM_ERRORSRC_DNACK_Msk;
            DEBUG("[i2c] check_error: NACK on data byte\n");
            return -EIO;
        }
    }

    return 0;
}

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    /* Initialize mutex */
    mutex_init(&locks[dev]);
    /* disable device during initialization, will be enabled when acquire is
     * called */
    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Disabled;
    /* configure pins */
    gpio_init(i2c_config[dev].scl, GPIO_IN_OD_PU);
    gpio_init(i2c_config[dev].sda, GPIO_IN_OD_PU);
    bus(dev)->PSEL.SCL = i2c_config[dev].scl;
    bus(dev)->PSEL.SDA = i2c_config[dev].sda;
    /* configure dev clock speed */
    bus(dev)->FREQUENCY = i2c_config[dev].speed;

    /* re-enable the device. We expect that the device was being acquired before
     * the i2c_init_master() function is called, so it should be enabled when
     * exiting this function. */
    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Enabled;
}

int i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&locks[dev]);
    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Enabled;

    DEBUG("[i2c] acquired dev %i\n", (int)dev);
    return 0;
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Disabled;
    mutex_unlock(&locks[dev]);

    DEBUG("[i2c] released dev %i\n", (int)dev);
}

int i2c_write_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                   const void *data, size_t len, uint8_t flags)
{
    assert((dev < I2C_NUMOF) && data && (len > 0) && (len < 255));

    if (flags & (I2C_NOSTART | I2C_REG16 | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }
    /* the nrf52's TWI device does not support to do two consecutive transfers
     * without a repeated start condition in between. So we have to put all data
     * to be transferred into a temporary buffer
     *
     * CAUTION: this might become critical when transferring large blocks of
     *          data as the temporary buffer is allocated on the stack... */
    uint8_t buf_tmp[len + 1];
    buf_tmp[0] = reg;
    memcpy(&buf_tmp[1], data, len);
    return i2c_write_bytes(dev, addr, buf_tmp, (len + 1), flags);
}

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data, size_t len,
                   uint8_t flags)
{
    assert((dev < I2C_NUMOF) && data && (len > 0) && (len < 256));

    if (flags & (I2C_NOSTART | I2C_REG16 | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }
    DEBUG("[i2c] read_bytes: %i bytes from addr 0x%02x\n", (int)len, (int)addr);

    bus(dev)->ADDRESS = addr;
    bus(dev)->RXD.PTR = (uint32_t)data;
    bus(dev)->RXD.MAXCNT = (uint8_t)len;

    if (!(flags & I2C_NOSTOP)) {
        bus(dev)->SHORTS = TWIM_SHORTS_LASTRX_STOP_Msk;
    }
    /* Start transmission */
    bus(dev)->TASKS_STARTRX = 1;

    return finish(dev);
}

int i2c_read_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                  void *data, size_t len, uint8_t flags)
{
    assert((dev < I2C_NUMOF) && data && (len > 0) && (len < 256));

    if (flags & (I2C_NOSTART | I2C_REG16 | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }
    DEBUG("[i2c] read_regs: %i byte(s) from reg 0x%02x at addr 0x%02x\n",
           (int)len, (int)reg, (int)addr);

    bus(dev)->ADDRESS = addr;
    bus(dev)->TXD.PTR = (uint32_t)&reg;
    bus(dev)->TXD.MAXCNT = 1;
    bus(dev)->RXD.PTR = (uint32_t)data;
    bus(dev)->RXD.MAXCNT = (uint8_t)len;
    bus(dev)->SHORTS = (TWIM_SHORTS_LASTTX_STARTRX_Msk);
    if (!(flags & I2C_NOSTOP)) {
        bus(dev)->SHORTS |=  TWIM_SHORTS_LASTRX_STOP_Msk;
    }
    bus(dev)->TASKS_STARTTX = 1;

    return finish(dev);
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len,
                    uint8_t flags)
{
    assert((dev < I2C_NUMOF) && data && (len > 0) && (len < 256));

    if (flags & (I2C_NOSTART | I2C_REG16 | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }
    DEBUG("[i2c] write_bytes: %i byte(s) to addr 0x%02x\n", (int)len, (int)addr);

    bus(dev)->ADDRESS = addr;
    bus(dev)->TXD.PTR = (uint32_t)data;
    bus(dev)->TXD.MAXCNT = (uint8_t)len;
    if (!(flags & I2C_NOSTOP)) {
        bus(dev)->SHORTS = TWIM_SHORTS_LASTTX_STOP_Msk;
    }
    bus(dev)->TASKS_STARTTX = 1;

    return finish(dev);
}
