/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis_common_i2c
 *
 * @note        This driver only implements the 7-bit addressing master mode.
 *
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "irq.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* guard file in case no I2C device is defined */
#if I2C_NUMOF

/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t locks[] =  {
#if I2C_0_EN
    [I2C_0] = MUTEX_INIT,
#endif
#if I2C_1_EN
    [I2C_1] = MUTEX_INIT,
#endif
#if I2C_2_EN
    [I2C_2] = MUTEX_INIT
#endif
#if I2C_3_EN
    [I2C_3] = MUTEX_INIT
#endif
};

int i2c_acquire(i2c_t dev)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}

int i2c_release(i2c_t dev)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
    return 0;
}

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    I2C_Type *i2c;
    PORT_Type *i2c_port;
    int pin_scl = 0;
    int pin_sda = 0;
    uint32_t baudrate_flags = 0;

    /** @todo Kinetis I2C: Add automatic baud rate flags selection function */
    /*
     * See the Chapter "I2C divider and hold values":
     *     Kinetis K60 Reference Manual, section 51.4.1.10, Table 51-41.
     *     Kinetis MKW2x  Reference Manual, section 52.4.1.10, Table 52-41.
     *
     * baud rate = I2C_module_clock / (mul × ICR)
     */
    switch (speed) {
        case I2C_SPEED_LOW:
            baudrate_flags |= I2C_F_MULT(KINETIS_I2C_F_MULT_LOW) |
                I2C_F_ICR(KINETIS_I2C_F_ICR_LOW);
            break;

        case I2C_SPEED_NORMAL:
            baudrate_flags |= I2C_F_MULT(KINETIS_I2C_F_MULT_NORMAL) |
                I2C_F_ICR(KINETIS_I2C_F_ICR_NORMAL);
            break;

        case I2C_SPEED_FAST:
            baudrate_flags |= I2C_F_MULT(KINETIS_I2C_F_MULT_FAST) |
                I2C_F_ICR(KINETIS_I2C_F_ICR_FAST);
            break;

        case I2C_SPEED_FAST_PLUS:
            baudrate_flags |= I2C_F_MULT(KINETIS_I2C_F_MULT_FAST_PLUS) |
                I2C_F_ICR(KINETIS_I2C_F_ICR_FAST_PLUS);
            break;

        default:
            /*
             * High speed mode is not supported on Kinetis devices,
             * see: https://community.freescale.com/thread/316371
             *
             * Hardware allows setting the baud rate high enough but the
             * capacitance of the bus and lacking a proper high speed mode SCL
             * driver will make the signals go out of spec.
             */
            return -2;
    }

    /* read static device configuration */
    switch (dev) {
#if I2C_0_EN

        case I2C_0:
            i2c = I2C_0_DEV;
            i2c_port = I2C_0_PORT;
            pin_scl = I2C_0_SCL_PIN;
            pin_sda = I2C_0_SDA_PIN;
            I2C_0_CLKEN();
            I2C_0_PORT_CLKEN();
            break;
#endif

        default:
            return -1;
    }

    /* configure pins, alternate output */
    i2c_port->PCR[pin_scl] = I2C_0_PORT_CFG;
    i2c_port->PCR[pin_sda] = I2C_0_PORT_CFG;

    i2c->F = baudrate_flags;

    /* enable i2c-module and interrupt */
    i2c->C1 = I2C_C1_IICEN_MASK | I2C_C1_IICIE_MASK | I2C_C1_TXAK_MASK;
    i2c->C2 = 0;

    return 0;
}

int i2c_init_slave(i2c_t dev, uint8_t address)
{
    /* TODO: implement slave mode */
    (void) dev;
    (void) address;
    return -1;
}

/*
 * Check for bus master arbitration lost.
 * Arbitration is lost in the following circumstances:
 * 1. SDA is sampled as low when the master drives high during an
 *    address or data transmit cycle.
 * 2. SDA is sampled as low when the master drives high during the
 *    acknowledge bit of a data receive cycle.
 * 3. A START cycle is attempted when the bus is busy.
 * 4. A repeated START cycle is requested in slave mode.
 * 5. A STOP condition is detected when the master did not request
 *    it.
 */
static inline int _i2c_arbitration_lost(I2C_Type *dev)
{
    return (int)(dev->S & I2C_S_ARBL_MASK);
}

static inline int _i2c_start(I2C_Type *dev, uint8_t address, uint8_t rw_flag)
{
    /* bus free ? */
    if (dev->S & I2C_S_BUSY_MASK) {
        return -1;
    }

    dev->S = I2C_S_IICIF_MASK;

    dev->C1 = I2C_C1_IICEN_MASK | I2C_C1_MST_MASK | I2C_C1_TX_MASK;
    dev->D = address << 1 | (rw_flag & 1);

    /* wait for bus-busy to be set */
    while (!(dev->S & I2C_S_BUSY_MASK)) {
        if (_i2c_arbitration_lost(dev)) {
            return -1;
        }
    }

    /* wait for address transfer to complete */
    while (!(dev->S & I2C_S_IICIF_MASK));

    dev->S = I2C_S_IICIF_MASK;

    if (_i2c_arbitration_lost(dev)) {
        return -1;
    }

    /* check for receive acknowledge */
    if (dev->S & I2C_S_RXAK_MASK) {
        return -1;
    }

    return 0;
}

static inline int _i2c_restart(I2C_Type *dev, uint8_t address, uint8_t rw_flag)
{
    /* put master in rx mode and repeat start */
    dev->C1 |= I2C_C1_RSTA_MASK;
    dev->D = address << 1 | (rw_flag & 1);

    /* wait for address transfer to complete */
    while (!(dev->S & I2C_S_IICIF_MASK));

    dev->S = I2C_S_IICIF_MASK;

    if (_i2c_arbitration_lost(dev)) {
        return -1;
    }

    /* check for receive acknowledge */
    if (dev->S & I2C_S_RXAK_MASK) {
        return -1;
    }

    return 0;
}

static inline int _i2c_receive(I2C_Type *dev, uint8_t *data, int length)
{
    int n = 0;

    /* set receive mode */
    dev->C1 = I2C_C1_IICEN_MASK | I2C_C1_MST_MASK;

    if (length == 1) {
        /* Send NACK after receiving the next byte */
        dev->C1 |= I2C_C1_TXAK_MASK;
    }

    /* Initiate master receive mode by reading the data register */
    dev->D;

    while (length > 0) {
        while (!(dev->S & I2C_S_IICIF_MASK));

        dev->S = I2C_S_IICIF_MASK;

        if (_i2c_arbitration_lost(dev)) {
            return -1;
        }

        length--;

        if (length == 1) {
            /* Send NACK after receiving the next byte */
            dev->C1 |= I2C_C1_TXAK_MASK;
        }

        data[n] = (char)dev->D;
        n++;
    }

    return n;
}

static inline int _i2c_transmit(I2C_Type *dev, uint8_t *data, int length)
{
    int n = 0;

    while (length > 0) {
        dev->D = data[n];

        while (!(dev->S & I2C_S_IICIF_MASK));

        dev->S = I2C_S_IICIF_MASK;

        if (dev->S & I2C_S_RXAK_MASK) {
            return n;
        }

        n++;
        length--;
    }

    return n;
}

static inline void _i2c_stop(I2C_Type *dev)
{
    /* put bus in idle state */
    dev->C1 = I2C_C1_IICEN_MASK;

    /* wait for bus idle */
    while (dev->S & I2C_S_BUSY_MASK);
}

static inline void _i2c_reset(I2C_Type *dev)
{
    /* put bus in idle state */
    dev->C1 = I2C_C1_IICEN_MASK;
    /* reset status flags */
    dev->S = I2C_S_ARBL_MASK | I2C_S_IICIF_MASK;
}


int i2c_read_byte(i2c_t dev, uint8_t address, char *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    I2C_Type *i2c;
    int n = 0;

    switch (dev) {
#if I2C_0_EN

        case I2C_0:
            i2c = I2C_0_DEV;
            break;
#endif

        default:
            return -1;
    }

    if (_i2c_start(i2c, address, I2C_FLAG_READ)) {
        _i2c_reset(i2c);
        return -1;
    }

    n = _i2c_receive(i2c, (uint8_t *)data, length);
    if (n < 0) {
        _i2c_reset(i2c);
        return -1;
    }

    _i2c_stop(i2c);

    return n;
}

int i2c_write_byte(i2c_t dev, uint8_t address, char data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    I2C_Type *i2c;
    int n = 0;

    switch (dev) {
#if I2C_0_EN

        case I2C_0:
            i2c = I2C_0_DEV;
            break;
#endif

        default:
            return -1;
    }

    if (_i2c_start(i2c, address, I2C_FLAG_WRITE)) {
        _i2c_reset(i2c);
        return -1;
    }

    n = _i2c_transmit(i2c, (uint8_t *)data, length);
    _i2c_stop(i2c);

    return n;
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, char *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);

}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
{
    I2C_Type *i2c;
    int n = 0;

    switch (dev) {
#if I2C_0_EN

        case I2C_0:
            i2c = I2C_0_DEV;
            break;
#endif

        default:
            return -1;
    }

    if (_i2c_start(i2c, address, I2C_FLAG_WRITE)) {
        _i2c_reset(i2c);
        return -1;
    }

    /* send reg */
    n = _i2c_transmit(i2c, &reg, 1);

    if (!n) {
        _i2c_stop(i2c);
        return n;
    }

    if (_i2c_restart(i2c, address, I2C_FLAG_READ)) {
        _i2c_reset(i2c);
        return -1;
    }

    n = _i2c_receive(i2c, (uint8_t *)data, length);
    if (n < 0) {
        _i2c_reset(i2c);
        return -1;
    }

    _i2c_stop(i2c);

    return n;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, char data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
{
    I2C_Type *i2c;
    int n = 0;

    switch (dev) {
#if I2C_0_EN

        case I2C_0:
            i2c = I2C_0_DEV;
            break;
#endif

        default:
            return -1;
    }

    if (_i2c_start(i2c, address, I2C_FLAG_WRITE)) {
        _i2c_reset(i2c);
        return -1;
    }

    n = _i2c_transmit(i2c, &reg, 1);

    if (!n) {
        _i2c_stop(i2c);
        return n;
    }

    n = _i2c_transmit(i2c, (uint8_t *)data, length);
    _i2c_stop(i2c);

    return n;
}

void i2c_poweron(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN

        case I2C_0:
            I2C_0_CLKEN();
            break;
#endif
    }
}

void i2c_poweroff(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN

        case I2C_0:
            I2C_0_CLKDIS();
            break;
#endif
    }
}

#endif /* I2C_NUMOF */
