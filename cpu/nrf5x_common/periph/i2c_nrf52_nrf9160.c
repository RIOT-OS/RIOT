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
 * @ingroup     cpu_nrf5x_common
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
#include "byteorder.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/**
 * @brief   If any of the 8 lower bits are set, the speed value is invalid
 */
#define INVALID_SPEED_MASK  (0xff)

/**
 * @brief   Allocate a tx buffer
 */
static uint8_t tx_buf[256];

/**
 * @brief   Mutex for locking the TX buffer
 */
static mutex_t buffer_lock;

/**
 * @brief   Initialized dev locks (we have a maximum of two devices...)
 */
static mutex_t locks[I2C_NUMOF];

/**
 * @brief   array with a busy mutex for each I2C device, used to block the
 *          thread until the transfer is done
 */
static mutex_t busy[I2C_NUMOF];

void i2c_isr_handler(void *arg);

static inline NRF_TWIM_Type *bus(i2c_t dev)
{
    return i2c_config[dev].dev;
}

static int finish(i2c_t dev)
{
    DEBUG("[i2c] waiting for STOPPED or ERROR event\n");
    /* Unmask interrupts */
    bus(dev)->INTENSET = TWIM_INTEN_STOPPED_Msk | TWIM_INTEN_ERROR_Msk;
    mutex_lock(&busy[dev]);

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

static void _init_pins(i2c_t dev)
{
    gpio_init(i2c_config[dev].scl, GPIO_IN_OD_PU);
    gpio_init(i2c_config[dev].sda, GPIO_IN_OD_PU);
    bus(dev)->PSEL.SCL = i2c_config[dev].scl;
    bus(dev)->PSEL.SDA = i2c_config[dev].sda;
}

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    /* Initialize mutex */
    mutex_init(&locks[dev]);
    mutex_init(&busy[dev]);
    mutex_lock(&busy[dev]);

    /* disable device during initialization, will be enabled when acquire is
     * called */
    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Disabled;

    /* configure pins */
    _init_pins(dev);

    /* configure dev clock speed */
    bus(dev)->FREQUENCY = i2c_config[dev].speed;

    spi_twi_irq_register_i2c(bus(dev), i2c_isr_handler, (void *)(uintptr_t)dev);

    /* We expect that the device was being acquired before
     * the i2c_init_master() function is called, so it should be enabled when
     * exiting this function. */
    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Enabled;
}

#ifdef MODULE_PERIPH_I2C_RECONFIGURE
void i2c_init_pins(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    _init_pins(dev);

    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Enabled;

    mutex_unlock(&locks[dev]);
}

void i2c_deinit_pins(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&locks[dev]);
    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Disabled;
}
#endif /* MODULE_PERIPH_I2C_RECONFIGURE */

void i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&locks[dev]);
    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Enabled;

    DEBUG("[i2c] acquired dev %i\n", (int)dev);
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
    assert((dev < I2C_NUMOF) && data && (len > 0) && (len < 253));

    if (flags & (I2C_NOSTART | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }

    /* the nrf52's TWI device does not support to do two consecutive transfers
     * without a repeated start condition in between. So we have to put all data
     * to be transferred into a buffer (tx_buf).
     *  */

    uint8_t reg_addr_len; /* Length in bytes of the register address */

    /* Lock tx_buf */
    mutex_lock(&buffer_lock);

    if (flags & (I2C_REG16)) {
        reg_addr_len = 2;
        /* Prepare the 16-bit register transfer  */
        tx_buf[0] = reg >> 8; /* AddrH in the first byte  */
        tx_buf[1] = reg & 0xFF; /* AddrL in the second byte  */
    }
    else{
        reg_addr_len = 1;
        tx_buf[0] = reg;
    }

    memcpy(&tx_buf[reg_addr_len], data, len);
    int ret = i2c_write_bytes(dev, addr, tx_buf, reg_addr_len + len, flags);

    /* Release tx_buf */
    mutex_unlock(&buffer_lock);

    return ret;

}

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data, size_t len,
                   uint8_t flags)
{
    assert((dev < I2C_NUMOF) && data && (len > 0) && (len < 256));

    if (flags & (I2C_NOSTART | I2C_ADDR10)) {
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

    if (flags & (I2C_NOSTART | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }

    DEBUG("[i2c] read_regs: %i byte(s) from reg 0x%02x at addr 0x%02x\n",
           (int)len, (int)reg, (int)addr);

    /* Prepare transfer */
    bus(dev)->ADDRESS = addr;
    if (flags & (I2C_REG16)) {
        /* Register endianness for 16 bit */
        reg = htons(reg);
        bus(dev)->TXD.MAXCNT = 2;
    }
    else {
        bus(dev)->TXD.MAXCNT = 1;
    }
    bus(dev)->TXD.PTR = (uint32_t)&reg;
    bus(dev)->RXD.PTR = (uint32_t)data;
    bus(dev)->RXD.MAXCNT = (uint8_t)len;
    bus(dev)->SHORTS = (TWIM_SHORTS_LASTTX_STARTRX_Msk);
    if (!(flags & I2C_NOSTOP)) {
        bus(dev)->SHORTS |=  TWIM_SHORTS_LASTRX_STOP_Msk;
    }

    /* Start transfer */
    bus(dev)->TASKS_STARTTX = 1;

    return finish(dev);
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len,
                    uint8_t flags)
{
    assert((dev < I2C_NUMOF) && data && (len > 0) && (len < 256));

    if (flags & (I2C_NOSTART | I2C_ADDR10)) {
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

void i2c_isr_handler(void *arg)
{
    i2c_t dev = (i2c_t)(uintptr_t)arg;

    /* Mask interrupts to ensure that they only trigger once */
    bus(dev)->INTENCLR = TWIM_INTEN_STOPPED_Msk | TWIM_INTEN_ERROR_Msk;

    mutex_unlock(&busy[dev]);
}
