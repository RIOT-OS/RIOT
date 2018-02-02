/*
 * Copyright (C) 2017 HAW Hamburg
 *               2018 Freie Universität Berlin
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
 *
 * @}
 */

#include <string.h>

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
 * @brief   Initialized bus locks (we have a maximum of two devices...)
 */
static mutex_t locks[] = {
    MUTEX_INIT,
    MUTEX_INIT
};

static inline NRF_TWIM_Type *dev(i2c_t bus)
{
    return i2c_config[bus].dev;
}

static int finish(i2c_t bus, int len)
{
    DEBUG("[i2c] waiting for STOPPED or ERROR event\n");

    while ((!(dev(bus)->EVENTS_STOPPED)) && (!(dev(bus)->EVENTS_ERROR))) {
        nrf52_sleep();
    }

    if ((dev(bus)->EVENTS_STOPPED)) {
        dev(bus)->EVENTS_STOPPED = 0;
        DEBUG("[i2c] finish: stop event occurred\n");
    }

    if (dev(bus)->EVENTS_ERROR) {
        dev(bus)->EVENTS_ERROR = 0;
        if (dev(bus)->ERRORSRC & TWIM_ERRORSRC_ANACK_Msk) {
            dev(bus)->ERRORSRC = TWIM_ERRORSRC_ANACK_Msk;
            DEBUG("[i2c] check_error: NACK on address byte\n");
            return -1;
        }
        if (dev(bus)->ERRORSRC & TWIM_ERRORSRC_DNACK_Msk) {
            dev(bus)->ERRORSRC = TWIM_ERRORSRC_DNACK_Msk;
            DEBUG("[i2c] check_error: NACK on data byte\n");
            return -1;
        }
    }

    return len;
}

int i2c_init_master(i2c_t bus, i2c_speed_t speed)
{
    if (bus >= I2C_NUMOF) {
        return -1;
    }
    if (speed & INVALID_SPEED_MASK) {
        return -2;
    }

    /* disable device during initialization, will be enabled when acquire is
     * called */
    dev(bus)->ENABLE = TWIM_ENABLE_ENABLE_Disabled;

    /* configure pins */
    gpio_init(i2c_config[bus].scl, GPIO_IN_PU);
    gpio_init(i2c_config[bus].sda, GPIO_IN_PU);
    dev(bus)->PSEL.SCL = i2c_config[bus].scl;
    dev(bus)->PSEL.SDA = i2c_config[bus].sda;

    /* configure bus clock speed */
    dev(bus)->FREQUENCY = speed;

    /* re-enable the device. We expect that the device was being acquired before
     * the i2c_init_master() function is called, so it should be enabled when
     * exiting this function. */
    dev(bus)->ENABLE = TWIM_ENABLE_ENABLE_Enabled;

    return 0;
}

int i2c_acquire(i2c_t bus)
{
    assert(bus < I2C_NUMOF);

    mutex_lock(&locks[bus]);
    dev(bus)->ENABLE = TWIM_ENABLE_ENABLE_Enabled;

    DEBUG("[i2c] acquired bus %i\n", (int)bus);
    return 0;
}

int i2c_release(i2c_t bus)
{
    assert(bus < I2C_NUMOF);

    dev(bus)->ENABLE = TWIM_ENABLE_ENABLE_Disabled;
    mutex_unlock(&locks[bus]);

    DEBUG("[i2c] released bus %i\n", (int)bus);
    return 0;
}

int i2c_read_byte(i2c_t bus, uint8_t addr, void *data)
{
    return i2c_read_bytes(bus, addr, data, 1);
}

int i2c_read_bytes(i2c_t bus, uint8_t addr, void *data, int len)
{
    assert((bus < I2C_NUMOF) && data && (len > 0) && (len < 256));

    DEBUG("[i2c] read_bytes: %i bytes from addr 0x%02x\n", (int)len, (int)addr);

    dev(bus)->ADDRESS = addr;
    dev(bus)->RXD.PTR = (uint32_t)data;
    dev(bus)->RXD.MAXCNT = (uint8_t)len;
    dev(bus)->SHORTS =  TWIM_SHORTS_LASTRX_STOP_Msk;
    dev(bus)->TASKS_STARTRX = 1;

    return finish(bus, len);
}

int i2c_read_reg(i2c_t bus, uint8_t addr, uint8_t reg, void *data)
{
    return i2c_read_regs(bus, addr, reg, data, 1);
}

int i2c_read_regs(i2c_t bus, uint8_t addr, uint8_t reg,
                  void *data, int len)
{
    assert((bus < I2C_NUMOF) && data && (len > 0) && (len < 256));

    DEBUG("[i2c] read_regs: %i byte(s) from reg 0x%02x at addr 0x%02x\n",
           (int)len, (int)reg, (int)addr);

    dev(bus)->ADDRESS = addr;
    dev(bus)->TXD.PTR = (uint32_t)&reg;
    dev(bus)->TXD.MAXCNT = 1;
    dev(bus)->RXD.PTR = (uint32_t)data;
    dev(bus)->RXD.MAXCNT = (uint8_t)len;
    dev(bus)->SHORTS = (TWIM_SHORTS_LASTTX_STARTRX_Msk |
                        TWIM_SHORTS_LASTRX_STOP_Msk);
    dev(bus)->TASKS_STARTTX = 1;

    return finish(bus, len);
}

int i2c_write_byte(i2c_t bus, uint8_t addr, uint8_t data)
{
    return i2c_write_bytes(bus, addr, &data, 1);
}

int i2c_write_bytes(i2c_t bus, uint8_t addr, const void *data, int len)
{
    assert((bus < I2C_NUMOF) && data && (len > 0) && (len < 256));

    DEBUG("[i2c] write_bytes: %i byte(s) to addr 0x%02x\n", (int)len, (int)addr);

    dev(bus)->ADDRESS = addr;
    dev(bus)->TXD.PTR = (uint32_t)data;
    dev(bus)->TXD.MAXCNT = (uint8_t)len;
    dev(bus)->SHORTS = TWIM_SHORTS_LASTTX_STOP_Msk;
    dev(bus)->TASKS_STARTTX = 1;

    return finish(bus, len);
}

int i2c_write_reg(i2c_t bus, uint8_t addr, uint8_t reg, uint8_t data)
{
    return i2c_write_regs(bus, addr, reg, &data, 1);
}

int i2c_write_regs(i2c_t bus, uint8_t addr, uint8_t reg,
                   const void *data, int len)
{
    assert((bus < I2C_NUMOF) && data && (len > 0) && (len < 255));

    /* the nrf52's TWI device does not support to do two consecutive transfers
     * without a repeated start condition in between. So we have to put all data
     * to be transfered into a temporary buffer
     *
     * CAUTION: this might become critical when transferring large blocks of
     *          data as the temporary buffer is allocated on the stack... */
    uint8_t buf_tmp[len + 1];
    buf_tmp[0] = reg;
    memcpy(&buf_tmp[1], data, len);
    return i2c_write_bytes(bus, addr, buf_tmp, (len + 1)) - 1;
}
