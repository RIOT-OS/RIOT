/*
 * Copyright (C) 2016 Pavol Malosek <malo@25cmsquare.io>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_periph
 * @{
 *
 * @file
 * @brief       Software single master I2C implementation with clock stretch.
 * @brief       spec: http://www.nxp.com/documents/user_manual/UM10204.pdf
 *
 * @author      Pavol Malosek <malo@25cmsquare.io>
 *
 * @}
 */

#include "periph/i2c_sw.h"
#include "periph_conf.h"
#include "periph/timer.h"
#include "log.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifdef I2C_SOFTWARE

/* Do not use higher timeouts than 0xFFFF since on some platforms
 * timer_read returns uint16_t */
#define CLOCK_STRETCH_TIMEOUT_US (50000U)

static inline int wait_for_SCL(i2c_t dev)
{
    unsigned int start_stamp = timer_read(XTIMER);
    while ((timer_read(XTIMER) - start_stamp) < CLOCK_STRETCH_TIMEOUT_US) {
        //  Check if SCL is up.
        if (i2c_sw_read_SCL(dev)) {
            return 0;
        }
    }

    return -1;
}

static inline void _bit_out(i2c_t dev, unsigned int bit)
{
    /* put data on SDA */
    if (bit) {
        i2c_sw_set_SDA(dev);
    }
    else {
        i2c_sw_clear_SDA(dev);
    }

    /* wait half of tLOW */
    i2c_sw_delay_2_35us();
    /* set SCL */
    i2c_sw_set_SCL(dev);
    /* wait tHIGH */
    i2c_sw_delay_4_00us();
    /* check for clock stretch */
    if (wait_for_SCL(dev) != 0) {
        LOG_ERROR("out: SCL line down too long - clock stretch timeout\n");
    }
    i2c_sw_clear_SCL(dev);
    /* wait half of tLOW */
    i2c_sw_delay_2_35us();
}

static inline int _bit_in(i2c_t dev, unsigned int last)
{
    unsigned int bit;
    /* slave is driving the data line */
    i2c_sw_set_SDA(dev);
    /* set SCL */
    i2c_sw_set_SCL(dev);
    /* wait tHIGH */
    i2c_sw_delay_4_00us();
    /* check clock stretch */
    if (wait_for_SCL(dev) != 0) {
        LOG_ERROR("in: SCL line down too long - clock stretch timeout\n");
    }
    /* read SDA */
    bit = i2c_sw_read_SDA(dev);
    /* clear SCL */
    i2c_sw_clear_SCL(dev);
    /* wait tLOW */
    if (!last) {
        i2c_sw_delay_4_70us();
    }
    else {
        i2c_sw_delay_2_35us();
    }

    return bit;
}

static int _byte_out(i2c_t dev, char byte)
{
    DEBUG("{0x%x}", byte & 0xFF);
    unsigned int ack;
    unsigned int count = 8;
    do {
        _bit_out(dev, byte & 0x80);
        byte <<= 1;
    } while (--count);

    /* read ACk */
    ack = _bit_in(dev, 0);

    return ack ? -1 : 0;
}

static char _byte_in(i2c_t dev, unsigned int ack)
{
    unsigned char byte = 0;
    unsigned int count = 8;
    unsigned int last  = 0;
    do {
        if (count == 1) {
            last = 1;
        }
        /* read the data */
        byte <<= 1;
        byte |= _bit_in(dev, last);
    } while (--count);

    /* send ACK */
    _bit_out(dev, ack);

    DEBUG("[0x%x]", byte & 0xFF);

    return byte;
}

static int _start(i2c_t dev)
{
    i2c_sw_set_SDA(dev);
    i2c_sw_set_SCL(dev);

    /* bus is busy */
    if (i2c_sw_read_SDA(dev) == 0) {
        LOG_ERROR("SDA line down - bus is busy/no pullup\n");
        return -1;
    }

    /* check SCL */
    if (wait_for_SCL(dev) != 0) {
        LOG_ERROR("SCL line down - bus is busy/no pullup\n");
        return -1;
    }

    /* tBUF */
    i2c_sw_delay_4_70us();
    i2c_sw_clear_SDA(dev);
    /* tHD; STA */
    i2c_sw_delay_4_00us();
    i2c_sw_clear_SCL(dev);
    /* wait half of tLOW */
    i2c_sw_delay_2_35us();

    return 0;
}

static void _stop(i2c_t dev)
{
    i2c_sw_clear_SDA(dev);
    /* wait half of tLOW */
    i2c_sw_delay_2_35us();
    i2c_sw_set_SCL(dev);
    /* tSU; STO */
    i2c_sw_delay_4_00us();
    i2c_sw_set_SDA(dev);
}

static int _send_address(i2c_t dev, uint8_t address, uint8_t rw_flag)
{
    char byte = address << 1 | rw_flag;
    if (_start(dev) != 0) {
        return -1;
    }

    return _byte_out(dev, byte);
}

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    if (speed != I2C_SPEED_NORMAL) {
        return -2;
    }

    if (i2c_sw_init_gpio(dev) != 0) {
        return -1;
    }

    i2c_sw_set_SDA(dev);
    i2c_sw_set_SCL(dev);

    return 0;
}

int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_DEV(0):
            break;
#endif
#if I2C_1_EN
        case I2C_DEV(1):
            break;
#endif
    default:
        return -1;
    }

    int byte_count = length;
    if (_send_address(dev, address, I2C_FLAG_WRITE) != 0) {
        return -2;
    }

    while(byte_count--) {
        if (_byte_out(dev, *data++) != 0) {
            _stop(dev);
            return -1;
        }
    }
    _stop(dev);

    return length;
}

int i2c_write_byte(i2c_t dev, uint8_t address, char data)
{
    return i2c_write_bytes(dev, address, &data, sizeof(data));
}

int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_DEV(0):
            break;
#endif
#if I2C_1_EN
        case I2C_DEV(1):
            break;
#endif
    default:
        return -1;
    }

    int byte_count = length;
    if (_send_address(dev, address, I2C_FLAG_READ) != 0) {
        return -2;
    }

    while(byte_count--) {
        *data++ = _byte_in(dev, !byte_count);
    }
    _stop(dev);

    return length;
}

int i2c_read_byte(i2c_t dev, uint8_t address, char *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}
#endif
