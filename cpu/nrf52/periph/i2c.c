/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/i2c.h"
#include "periph_conf.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/**
 * @brief   If any of the 4 lower bits are set, the speed value is invalid
 */
#define INVALID_SPEED_MASK  (0x0f)

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

static int error(i2c_t bus)
{
    DEBUG("[i2c] error 0x%02x\n", (int)dev(bus)->ERRORSRC);
    dev(bus)->ERRORSRC = 0;
    dev(bus)->EVENTS_ERROR = 0;
    return -1;
}

static int write(i2c_t bus, uint8_t address, const void *data, int length, int stop)
{
    uint8_t *out_buf = (uint8_t *)data;
    assert((bus <= I2C_NUMOF) && (length > 0));

    DEBUG("[i2c]: writing %i bytes to the bus\n", length);

    /* disable shortcuts */
    if (stop == 0) {
        dev(bus)->SHORTS = 0;
    }

    /* set the client address and the data pointer */
    dev(bus)->ADDRESS = (address & 0x7f);
    dev(bus)->TXD.PTR = (uint32_t)out_buf;
    dev(bus)->TXD.MAXCNT = (length << TWIM_TXD_MAXCNT_MAXCNT_Pos);

    /* start write sequence */
    dev(bus)->EVENTS_LASTTX = 0;
    dev(bus)->TASKS_STARTTX = 1;

    /* wait for the device to finish up */
    while ((dev(bus)->EVENTS_LASTTX == 0) && (dev(bus)->EVENTS_ERROR == 0)) {}
    if (dev(bus)->EVENTS_ERROR) {
        return error(bus);
    }

    /* wait for the device to finish up */
    while ((dev(bus)->TXD.AMOUNT != (unsigned)length) && (dev(bus)->EVENTS_ERROR == 0)) {}
    if (dev(bus)->EVENTS_ERROR) {
        return error(bus);
    }

    /* enable shortcuts */
    if (stop == 0) {
        dev(bus)->SHORTS = (TWIM_SHORTS_LASTTX_STOP_Enabled << TWIM_SHORTS_LASTTX_STOP_Pos) |
                           (TWIM_SHORTS_LASTRX_STOP_Enabled << TWIM_SHORTS_LASTRX_STOP_Pos);
    }

    return dev(bus)->TXD.AMOUNT;
}

int i2c_init_master(i2c_t bus, i2c_speed_t speed)
{
    if (bus >= I2C_NUMOF) {
        return -1;
    }
    if (speed & INVALID_SPEED_MASK) {
        return -2;
    }

    /* pin configuration */
    NRF_P0->PIN_CNF[i2c_config[bus].pin_scl] = (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos);
    NRF_P0->PIN_CNF[i2c_config[bus].pin_scl] = (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos);
    dev(bus)->PSEL.SCL = i2c_config[bus].pin_scl;
    dev(bus)->PSEL.SDA = i2c_config[bus].pin_sda;

    /* shortcuts configuration */
    dev(bus)->SHORTS = (TWIM_SHORTS_LASTTX_STOP_Enabled << TWIM_SHORTS_LASTTX_STOP_Pos) |
                       (TWIM_SHORTS_LASTRX_STOP_Enabled << TWIM_SHORTS_LASTRX_STOP_Pos);

    /* bus clock speed configuration */
    dev(bus)->FREQUENCY = (speed << TWIM_FREQUENCY_FREQUENCY_Pos);

    /* enable the device */
    dev(bus)->ENABLE = (TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos);

    return 0;
}

int i2c_acquire(i2c_t bus)
{
    assert(bus <= I2C_NUMOF);
    mutex_lock(&locks[bus]);
    return 0;
}

int i2c_release(i2c_t bus)
{
    assert(bus <= I2C_NUMOF);
    mutex_unlock(&locks[bus]);
    return 0;
}

int i2c_read_byte(i2c_t bus, uint8_t address, void *data)
{
    return i2c_read_bytes(bus, address, data, 1);
}

int i2c_read_bytes(i2c_t bus, uint8_t address, void *data, int length)
{
    uint8_t *in_buf = (uint8_t *)data;
    assert((bus <= I2C_NUMOF) && (length > 0));

    DEBUG("[i2c] reading %i bytes from the bus\n", length);

    /* set the client address and the data pointer */
    dev(bus)->ADDRESS = (address & 0x7f);
    dev(bus)->RXD.PTR = (uint32_t)in_buf;
    dev(bus)->RXD.MAXCNT = (length << TWIM_RXD_MAXCNT_MAXCNT_Pos);

    /* start read sequence */
    dev(bus)->EVENTS_STOPPED = 0;
    dev(bus)->TASKS_STARTRX = 1;

    /* wait for the device to finish up */
    while ((dev(bus)->EVENTS_STOPPED == 0) && (dev(bus)->EVENTS_ERROR == 0)) {}
    if (dev(bus)->EVENTS_ERROR) {
        return error(bus);
    }

    return dev(bus)->RXD.AMOUNT;
}

int i2c_read_reg(i2c_t bus, uint8_t address, uint8_t reg, void *data)
{
    write(bus, address, &reg, 1, 0);
    return i2c_read_bytes(bus, address, data, 1);
}

int i2c_read_regs(i2c_t bus, uint8_t address, uint8_t reg,
                  void *data, int length)
{
    write(bus, address, &reg, 1, 0);
    return i2c_read_bytes(bus, address, data, length);
}

int i2c_write_byte(i2c_t bus, uint8_t address, uint8_t data)
{
    return write(bus, address, &data, 1, 1);
}

int i2c_write_bytes(i2c_t bus, uint8_t address, const void *data, int length)
{
    return write(bus, address, data, length, 1);
}

int i2c_write_reg(i2c_t bus, uint8_t address, uint8_t reg, uint8_t data)
{
    /* send reg and data in one function call */
    uint8_t out_buf[2];
    out_buf[0] = reg;
    out_buf[1] = data;
    return write(bus, address, &out_buf, 2, 1) - 1;
}

int i2c_write_regs(i2c_t bus, uint8_t address, uint8_t reg, const void *data, int length)
{
    /* send reg and data in one function call */
    uint8_t *buf = (uint8_t *)data;
    uint8_t out_buf[length + 1];
    out_buf[0] = reg;
    for (int i = 0; i < length ; i++) {
        out_buf[i + 1] = buf[i];
    }
    return write(bus, address, &out_buf, (length + 1), 1) - 1;
}
