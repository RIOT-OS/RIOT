/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences, Dimitri Nahm
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_atmega_common
 * @ingroup drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation for atmega common
 *
 * @note        This implementation only implements the 7-bit addressing mode.
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 *
 * @}
 */

#include <stdint.h>
#include <errno.h>
#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/i2c.h"
#include "periph_conf.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define MT_START            0x08
#define MT_START_REPEATED   0x10
#define MT_ADDRESS_ACK      0x18
#define MT_DATA_ACK         0x28
#define MR_ADDRESS_ACK      0x40

#define ATMEGA_I2C_FLAG_WRITE      0
#define ATMEGA_I2C_FLAG_READ       1

/* static function definitions */
static int _start(uint8_t address, uint8_t rw_flag);
static int _write(const uint8_t *data, int length);
static void _stop(void);
static void i2c_poweron(i2c_t dev);

static mutex_t locks[I2C_NUMOF];

/* TODO : 10 bits addresses */

void i2c_init(i2c_t dev)
{
    /* check if the line is valid */
    assert(dev < I2C_NUMOF);

    mutex_init(&locks[dev]);

    /* TWI Bit Rate Register - division factor for the bit rate generator */
    unsigned long twibrr;
    /* TWI Prescaler Bits - default 0 */
    uint8_t twipb = 0;

    /* calculate speed configuration */
    switch (I2C_BUS_SPEED) {

        case I2C_SPEED_LOW:
            if ((CLOCK_CORECLOCK > 20000000UL)
                || (CLOCK_CORECLOCK < 1000000UL)) {
                DEBUG("[i2c] init: bus speed incompatible with core clock\n");
                return;
            }
            twibrr = ((CLOCK_CORECLOCK / 10000UL) - 16) / (2 * 4); /* CLK Prescaler 4 */
            twipb = 1;
            break;

        case I2C_SPEED_NORMAL:
            if ((CLOCK_CORECLOCK > 50000000UL)
                || (CLOCK_CORECLOCK < 2000000UL)) {
                DEBUG("[i2c] init: bus speed incompatible with core clock\n");
                return;
            }
            twibrr = ((CLOCK_CORECLOCK / 100000UL) - 16) / 2;
            break;

        case I2C_SPEED_FAST:
            if (CLOCK_CORECLOCK < 7500000UL) {
                DEBUG("[i2c] init: bus speed incompatible with core clock\n");
                return;
            }
            twibrr = ((CLOCK_CORECLOCK / 400000UL) - 16) / 2;
            break;

        case I2C_SPEED_FAST_PLUS:
            if (CLOCK_CORECLOCK < 18000000UL) {
                DEBUG("[i2c] init: bus speed incompatible with core clock\n");
                return;
            }
            twibrr = ((CLOCK_CORECLOCK / 1000000UL) - 16) / 2;
            break;

        case I2C_SPEED_HIGH:
            if (CLOCK_CORECLOCK < 62000000UL) {
                DEBUG("[i2c] init: bus speed incompatible with core clock\n");
                return;
            }
            twibrr = ((CLOCK_CORECLOCK / 3400000UL) - 16) / 2;
            break;

        default:
            DEBUG("[i2c] init: invalid bus speed\n");
            return;
    }

    /* set pull-up on SCL and SDA */
    I2C_PORT_REG |= (I2C_PIN_MASK);

    /* enable I2C clock */
    i2c_poweron(dev);

    /* disable device */
    TWCR &= ~(1 << TWEN);
    /* configure I2C clock */
    TWBR = (uint8_t)twibrr;     /* Set TWI Bit Rate Register */
    TWSR &= ~(0x03);            /* Reset TWI Prescaler Bits */
    TWSR |= twipb;              /* Set TWI Prescaler Bits */
    /* enable device */
    TWCR |= (1 << TWEN);
}

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data, size_t len,
                   uint8_t flags)
{
    (void)dev;
    assert(dev < I2C_NUMOF);

    /* Check for unsupported operations */
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }
    /* Check for wrong arguments given */
    if (data == NULL || len == 0) {
        return -EINVAL;
    }

    uint8_t *my_data = data;

    /* send start condition and slave address */
    if (!(flags & I2C_NOSTART)) {
        if (_start(addr, ATMEGA_I2C_FLAG_READ) < 0) {
            return -ENXIO;
        }
    }

    for (size_t i = 0; i < len; i++) {
        /* Send NACK for last received byte */
        if ((len - i) == 1) {
            TWCR = (1 << TWEN) | (1 << TWINT);
        }
        else {
            TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWINT);
        }
        DEBUG("[i2c] i2c_read_bytes: Wait for byte\n");

        /* Wait for TWINT Flag set. This indicates that DATA has been received.*/
        while (!(TWCR & (1 << TWINT))) {}

        /* receive data byte */
        my_data[i] = TWDR;
        DEBUG("[i2c] i2c_read_bytes: Byte received\n");
    }

    /* end transmission */
    if (!(flags & I2C_NOSTOP)) {
        _stop();
    }

    return 0;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len,
                    uint8_t flags)
{
    (void)dev;
    assert(dev < I2C_NUMOF);

    /* Check for unsupported operations */
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }
    /* Check for wrong arguments given */
    if (data == NULL || len == 0) {
        return -EINVAL;
    }

    /* start transmission and send slave address */
    if (!(flags & I2C_NOSTART)) {
        if (_start(addr, ATMEGA_I2C_FLAG_WRITE) < 0) {
            DEBUG("[i2c] i2c_write_bytes: start failed\n");
            return -ENXIO;
        }
    }

    /* send out data bytes */
    if (_write(data, len) < 0) {
        DEBUG("[i2c] i2c_write_bytes: write failed\n");
        return -EIO;
    }

    /* end transmission */
    if (!(flags & I2C_NOSTOP)) {
        DEBUG("[i2c] i2c_write_bytes: sending stop\n");
        _stop();
    }

    return 0;
}

int i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&locks[dev]);
    return 0;
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_unlock(&locks[dev]);
}

static void i2c_poweron(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    (void) dev;
    power_twi_enable();
}

static int _start(uint8_t address, uint8_t rw_flag)
{
    /* Reset I2C Interrupt Flag and transmit START condition */
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    DEBUG("[i2c] start: START condition transmitted\n");

    /* Wait for TWINT Flag set. This indicates that the START has been
     * transmitted, and ACK/NACK has been received.*/
    while (!(TWCR & (1 << TWINT))) {}

    /* Check value of TWI Status Register. Mask prescaler bits.
     * If status different from START go to ERROR */
    if ((TWSR & 0xF8) == MT_START) {
        DEBUG("[i2c] start: I2C Status is: START\n");
    }
    else if ((TWSR & 0xF8) == MT_START_REPEATED) {
        DEBUG("[i2c] start: I2C Status is: START REPEATED\n");
    }
    else {
        DEBUG("[i2c] start: I2C Status Register is different from "
              "START/START_REPEATED\n");
        _stop();
        return -1;
    }

    /* Load ADDRESS and R/W Flag into TWDR Register.
     * Clear TWINT bit in TWCR to start transmission of ADDRESS */
    TWDR = (address << 1) | rw_flag;
    TWCR = (1 << TWINT) | (1 << TWEN);
    DEBUG("[i2c] start: ADDRESS and FLAG transmitted\n");

    /* Wait for TWINT Flag set. This indicates that ADDRESS has been transmitted.*/
    while (!(TWCR & (1 << TWINT))) {}

    /* Check value of TWI Status Register. Mask prescaler bits.
     * If status different from ADDRESS ACK go to ERROR */
    if ((TWSR & 0xF8) == MT_ADDRESS_ACK) {
        DEBUG("[i2c] start: ACK has been received for ADDRESS %02X (write)\n",
              address);
    }
    else if ((TWSR & 0xF8) == MR_ADDRESS_ACK) {
        DEBUG("[i2c] start: ACK has been received for ADDRESS %02X (read)\n",
              address);
    }
    else {
        DEBUG("[i2c] start: NACK has been received for ADDRESS %02X \n",
              address);
        _stop();
        return -2;
    }

    return 0;
}

/* TODO : const uint8_t data instead of *data */
static int _write(const uint8_t *data, int length)
{
    for (int i = 0; i < length; i++) {
        /* Load DATA into TWDR Register.
         * Clear TWINT bit in TWCR to start transmission of data */
        TWDR = data[i];
        TWCR = (1 << TWINT) | (1 << TWEN);
        DEBUG("[i2c] write: Byte transmitted\n");

        /* Wait for TWINT Flag set. This indicates that DATA has been transmitted.*/
        while (!(TWCR & (1 << TWINT))) {}

        /* Check value of TWI Status Register. Mask prescaler bits. If status
         * different from MT_DATA_ACK, return number of transmitted bytes */
        if ((TWSR & 0xF8) != MT_DATA_ACK) {
            DEBUG("[i2c] write: NACK has been received\n");
            return -1;
        }
        else {
            DEBUG("[i2c] write: ACK has been received\n");
        }
    }

    return 0;
}

static void _stop(void)
{
    /* Reset I2C Interrupt Flag and transmit STOP condition */
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    /* Wait for STOP Flag reset. This indicates that STOP has been transmitted.*/
    while (TWCR & (1 << TWSTO)) {}
    DEBUG("[i2c] stop: STOP condition transmitted\n");
    TWCR = 0;
}
