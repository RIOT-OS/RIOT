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
 * @brief       Low-level I2C driver implementation fot atmega common
 *
 * @note        This implementation only implements the 7-bit addressing mode.
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/i2c.h"
#include "periph_conf.h"

#include "debug.h"
#define ENABLE_DEBUG      (0)

/* guard file in case no I2C device is defined */
#if I2C_NUMOF

#define MT_START            0x08
#define MT_START_REPEATED   0x10
#define MT_ADDRESS_ACK      0x18
#define MT_DATA_ACK         0x28
#define MR_ADDRESS_ACK      0x40

/* static function definitions */
static int _start(uint8_t address, uint8_t rw_flag);
static int _write(const uint8_t *data, int length);
static void _stop(void);

static mutex_t lock = MUTEX_INIT;

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    /* TWI Bit Rate Register - division factor for the bit rate generator */
    unsigned long twibrr;
    /* TWI Prescaler Bits - default 0 */
    uint8_t twipb = 0;

    /* check if the line is valid */
    if (dev >= I2C_NUMOF) {
        return -1;
    }

    /* calculate speed configuration */
    switch (speed) {

        case I2C_SPEED_LOW:
            if ((CLOCK_CORECLOCK > 20000000UL)
                || (CLOCK_CORECLOCK < 1000000UL)) {
                return -2;
            }
            twibrr = ((CLOCK_CORECLOCK / 10000UL) - 16) / (2 * 4);  /* CLK Prescaler 4 */
            twipb = 1;
            break;

        case I2C_SPEED_NORMAL:
            if ((CLOCK_CORECLOCK > 50000000UL)
                || (CLOCK_CORECLOCK < 2000000UL)) {
                return -2;
            }
            twibrr = ((CLOCK_CORECLOCK / 100000UL) - 16) / 2;
            break;

        case I2C_SPEED_FAST:
            if (CLOCK_CORECLOCK < 7500000UL) {
                return -2;
            }
            twibrr = ((CLOCK_CORECLOCK / 400000UL) - 16) / 2;
            break;

        case I2C_SPEED_FAST_PLUS:
            if (CLOCK_CORECLOCK < 18000000UL) {
                return -2;
            }
            twibrr = ((CLOCK_CORECLOCK / 1000000UL) - 16) / 2;
            break;

        case I2C_SPEED_HIGH:
            if (CLOCK_CORECLOCK < 62000000UL) {
                return -2;
            }
            twibrr = ((CLOCK_CORECLOCK / 3400000UL) - 16) / 2;
            break;

        default:
            return -2;
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

    return 0;
}

int i2c_acquire(i2c_t dev)
{
    if (!(dev < I2C_NUMOF)) {
        return -1;
    }
    mutex_lock(&lock);
    return 0;
}

int i2c_release(i2c_t dev)
{
    if (!(dev < I2C_NUMOF)) {
        return -1;
    }
    mutex_unlock(&lock);
    return 0;
}

int i2c_read_byte(i2c_t dev, uint8_t address, void *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

int i2c_read_bytes(i2c_t dev, uint8_t address, void *data, int length)
{
    assert(length > 0);

    if (!(dev < I2C_NUMOF)) {
        return -1;
    }

    uint8_t *my_data = data;
    /* send start condition and slave address */
    if (_start(address, I2C_FLAG_READ) != 0) {
        return 0;
    }

    for (int i = 0; i < length; i++) {
        /* Send NACK for last received byte */
        if ((length - i) == 1) {
            TWCR = (1 << TWEN) | (1 << TWINT);
        }
        else {
            TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWINT);
        }
        DEBUG("Wait for byte %i\n", i+1);
        /* Wait for TWINT Flag set. This indicates that DATA has been received.*/
        while (!(TWCR & (1 << TWINT))) {}
        /* receive data byte */
        my_data[i] = TWDR;
        DEBUG("Byte %i received\n", i+1);
    }

    /* end transmission */
    _stop();

    return length;
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, void *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, void *data, int length)
{
    assert(length > 0);

    if (!(dev < I2C_NUMOF)) {
        return -1;
    }

    /* start transmission and send slave address */
    if (_start(address, I2C_FLAG_WRITE) != 0) {
        return 0;
    }

    /* send register address and wait for complete transfer to be finished*/
    if (_write(&reg, 1) != 1) {
        _stop();
        return 0;
    }

    /* now start a new start condition and receive data */
    return i2c_read_bytes(dev, address, data, length);
}

int i2c_write_byte(i2c_t dev, uint8_t address, uint8_t data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, const void *data, int length)
{
    int bytes = 0;

    assert(length > 0);

    if (!(dev < I2C_NUMOF)) {
        return -1;
    }

    /* start transmission and send slave address */
    if (_start(address, I2C_FLAG_WRITE) != 0) {
        return 0;
    }

    /* send out data bytes */
    bytes = _write(data, length);

    /* end transmission */
    _stop();

    return bytes;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, uint8_t data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, const void *data, int length)
{
    int bytes = 0;

    assert(length > 0);

    if (!(dev < I2C_NUMOF)) {
        return -1;
    }

    /* start transmission and send slave address */
    if (_start(address, I2C_FLAG_WRITE) != 0) {
        return 0;
    }
    /* send register address and wait for complete transfer to be finished*/
    if (_write(&reg, 1)) {
        /* write data to register */
        bytes = _write(data, length);
    }
    /* finish transfer */
    _stop();
    /* return number of bytes send */
    return bytes;
}

void i2c_poweron(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    (void) dev;
    power_twi_enable();
}

void i2c_poweroff(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    (void) dev;
    power_twi_disable();
}

static int _start(uint8_t address, uint8_t rw_flag)
{
    /* Reset I2C Interrupt Flag and transmit START condition */
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    DEBUG("START condition transmitted\n");

    /* Wait for TWINT Flag set. This indicates that the START has been
     * transmitted, and ACK/NACK has been received.*/
    while (!(TWCR & (1 << TWINT))) {}

    /* Check value of TWI Status Register. Mask prescaler bits.
     * If status different from START go to ERROR */
    if ((TWSR & 0xF8) == MT_START) {
        DEBUG("I2C Status is: START\n");
    }
    else if ((TWSR & 0xF8) == MT_START_REPEATED) {
        DEBUG("I2C Status is: START REPEATED\n");
    }
    else {
        DEBUG("I2C Status Register is different from START/START_REPEATED\n");
        _stop();
        return -1;
    }


    /* Load ADDRESS and R/W Flag into TWDR Register.
     * Clear TWINT bit in TWCR to start transmission of ADDRESS */
    TWDR = (address << 1) | rw_flag;
    TWCR = (1 << TWINT) | (1 << TWEN);
    DEBUG("ADDRESS and FLAG transmitted\n");

    /* Wait for TWINT Flag set. This indicates that ADDRESS has been transmitted.*/
    while (!(TWCR & (1 << TWINT))) {}

    /* Check value of TWI Status Register. Mask prescaler bits.
     * If status different from ADDRESS ACK go to ERROR */
    if ((TWSR & 0xF8) == MT_ADDRESS_ACK) {
        DEBUG("ACK has been received for ADDRESS (write)\n");
    }
    else if ((TWSR & 0xF8) == MR_ADDRESS_ACK) {
        DEBUG("ACK has been received for ADDRESS (read)\n");
    }
    else {
        DEBUG("NOT ACK has been received for ADDRESS\n");
        _stop();
        return -2;
    }

    return 0;
}

static int _write(const uint8_t *data, int length)
{
    for (int i = 0; i < length; i++) {
        /* Load DATA into TWDR Register.
         * Clear TWINT bit in TWCR to start transmission of data */
        TWDR = data[i];
        TWCR = (1 << TWINT) | (1 << TWEN);
        DEBUG("Byte %i transmitted\n", i+1);

        /* Wait for TWINT Flag set. This indicates that DATA has been transmitted.*/
        while (!(TWCR & (1 << TWINT))) {}

        /* Check value of TWI Status Register. Mask prescaler bits. If status
         * different from MT_DATA_ACK, return number of transmitted bytes */
        if ((TWSR & 0xF8) != MT_DATA_ACK) {
            DEBUG("NACK has been received for BYTE %i\n", i+1);
            return i;
        }
        else {
            DEBUG("ACK has been received for BYTE %i\n", i+1);
        }
    }

    return length;
}

static void _stop(void)
{
    /* Reset I2C Interrupt Flag and transmit STOP condition */
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    /* Wait for STOP Flag reset. This indicates that STOP has been transmitted.*/
    while (TWCR & (1 << TWSTO)) {}
    DEBUG("STOP condition transmitted\n");
    TWCR = 0;
}

#endif /* I2C_NUMOF */
