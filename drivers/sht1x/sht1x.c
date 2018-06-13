/*
 * Copyright 2009 Freie Universitaet Berlin (FUB)
 *           2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sht1x
 * @brief       Driver for the Sensirion SHT10/SHT11/SHT15 humidity and
 *              temperature sensor
 * @{
 *
 * @file
 * @brief       SHT10/SHT11/SHT15 Device Driver
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */
#include <errno.h>
#include <stdint.h>

#include "xtimer.h"
#include "sht1x.h"
#include "sht1x_defines.h"
#include "bitarithm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief             Perform measurement
 *
 * @param dev         SHT1X device to use
 * @param value       Measured value
 * @param mode        The requested measurement mode: temperature or humidity
 *
 * @retval  0         Success
 * @retval -EIO       I/O failure (`gpio_init()` failed)
 * @retval -EBADMSG   CRC-8 checksum didn't match
 * @retval -EPROTO    SHT1x did not acknowledge command
 * @retval -ECANCELED Measurement timed out
 */
static int measure(const sht1x_dev_t *dev, uint16_t *value, uint8_t mode);

/**
 * @brief             Write one byte
 *
 * @param  dev        SHT1X device to send the byte to
 * @param  value      The value to write
 *
 * @retval  1         Write was acknowledged
 * @retval  0         Write was *NOT* acknowledged (communication failure)
 * @retval -EIO       I/O failure (`gpio_init()` failed)
 */
static int write_byte(const sht1x_dev_t *dev, uint8_t value);

/**
 * @brief             Read one byte
 *
 * @param  dev        SHT1X device to receive the byte from
 * @param  dest       Store the received byte here
 * @param  ack        `SHT1X_ACK` to acknowledge byte, `SHT1X_NO_ACK` otherwise
 *
 * @retval  0         Success
 * @retval -EIO       I/O failure (`gpio_init()` failed)
 */
static int read_byte(const sht1x_dev_t *dev, uint8_t *dest, int ack);

/**
 * @brief             Communication reset
 *
 * @param  dev        SHT1X device to reset the connection to
 *
 * @retval  0         Success
 * @retval -EIO       I/O failure (`gpio_init()` failed)
 */
static int connection_reset(const sht1x_dev_t *dev);

/**
 * @brief             Send start of transmission sequence
 *
 * @param  dev        SHT1X device to send the transmission start sequence to
 *
 * @retval  0         Success
 * @retval -EIO       I/O failure (`gpio_init()` failed)
 */
static int transmission_start(const sht1x_dev_t *dev);

/**
 * @brief             Toggle the clock line
 *
 * @param  dev        SHT1X device to send one clock signal to
 */
static inline void clk_signal(const sht1x_dev_t *dev);

/**
 * @brief             Calculate the initial value of the CRC-8 checksum
 *
 * @param status      The current sensor status
 *
 * @return            The initial value of the CRC-8 checksum
 */
static inline uint8_t crc_initial_value(uint8_t status);

/**
 * @brief             Reverse the order of bits in a byte (needed for CRC)
 *
 * @param value       The byte to reverse the bits of
 *
 * @return            The reversed input
 */
static inline uint8_t reverse_byte(uint8_t value);

/**
 * @brief             Look up table required for CRC-8 calculation
 *
 * Values taken from the Application Note PDF of Sensirion (December 2011)
 */
static const uint8_t crc_lookup_table[] = {
    0x00, 0x31, 0x62, 0x53,  0xc4, 0xf5, 0xa6, 0x97,
    0xb9, 0x88, 0xdb, 0xea,  0x7d, 0x4c, 0x1f, 0x2e,
    0x43, 0x72, 0x21, 0x10,  0x87, 0xb6, 0xe5, 0xd4,
    0xfa, 0xcb, 0x98, 0xa9,  0x3e, 0x0f, 0x5c, 0x6d,
    0x86, 0xb7, 0xe4, 0xd5,  0x42, 0x73, 0x20, 0x11,
    0x3f, 0x0e, 0x5d, 0x6c,  0xfb, 0xca, 0x99, 0xa8,
    0xc5, 0xf4, 0xa7, 0x96,  0x01, 0x30, 0x63, 0x52,
    0x7c, 0x4d, 0x1e, 0x2f,  0xb8, 0x89, 0xda, 0xeb,
    0x3d, 0x0c, 0x5f, 0x6e,  0xf9, 0xc8, 0x9b, 0xaa,
    0x84, 0xb5, 0xe6, 0xd7,  0x40, 0x71, 0x22, 0x13,
    0x7e, 0x4f, 0x1c, 0x2d,  0xba, 0x8b, 0xd8, 0xe9,
    0xc7, 0xf6, 0xa5, 0x94,  0x03, 0x32, 0x61, 0x50,
    0xbb, 0x8a, 0xd9, 0xe8,  0x7f, 0x4e, 0x1d, 0x2c,
    0x02, 0x33, 0x60, 0x51,  0xc6, 0xf7, 0xa4, 0x95,
    0xf8, 0xc9, 0x9a, 0xab,  0x3c, 0x0d, 0x5e, 0x6f,
    0x41, 0x70, 0x23, 0x12,  0x85, 0xb4, 0xe7, 0xd6,
    0x7a, 0x4b, 0x18, 0x29,  0xbe, 0x8f, 0xdc, 0xed,
    0xc3, 0xf2, 0xa1, 0x90,  0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5b, 0x6a,  0xfd, 0xcc, 0x9f, 0xae,
    0x80, 0xb1, 0xe2, 0xd3,  0x44, 0x75, 0x26, 0x17,
    0xfc, 0xcd, 0x9e, 0xaf,  0x38, 0x09, 0x5a, 0x6b,
    0x45, 0x74, 0x27, 0x16,  0x81, 0xb0, 0xe3, 0xd2,
    0xbf, 0x8e, 0xdd, 0xec,  0x7b, 0x4a, 0x19, 0x28,
    0x06, 0x37, 0x64, 0x55,  0xc2, 0xf3, 0xa0, 0x91,
    0x47, 0x76, 0x25, 0x14,  0x83, 0xb2, 0xe1, 0xd0,
    0xfe, 0xcf, 0x9c, 0xad,  0x3a, 0x0b, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57,  0xc0, 0xf1, 0xa2, 0x93,
    0xbd, 0x8c, 0xdf, 0xee,  0x79, 0x48, 0x1b, 0x2a,
    0xc1, 0xf0, 0xa3, 0x92,  0x05, 0x34, 0x67, 0x56,
    0x78, 0x49, 0x1a, 0x2b,  0xbc, 0x8d, 0xde, 0xef,
    0x82, 0xb3, 0xe0, 0xd1,  0x46, 0x77, 0x24, 0x15,
    0x3b, 0x0a, 0x59, 0x68,  0xff, 0xce, 0x9d, 0xac,
};

/** @brief Lookuptable for d1 parameter depending on supply voltage */
static const int16_t sht1x_d1[] = { -4010, -3980, -3970, -3960, -3940 };

/*---------------------------------------------------------------------------*/
static inline void clk_signal(const sht1x_dev_t *dev)
{
    gpio_set(dev->clk);
    xtimer_usleep(SHT1X_HALF_CLOCK);
    gpio_clear(dev->clk);
    xtimer_usleep(SHT1X_HALF_CLOCK);
}

/*---------------------------------------------------------------------------*/
static int write_byte(const sht1x_dev_t *dev, uint8_t value)
{
    int ack;

    if (gpio_init(dev->data, GPIO_OUT) == -1) {
        return -EIO;
    }

    /* send value bit by bit to sht1x */
    for (int i = 0; i < 8; i++) {
        if (value & BIT7) {
            gpio_set(dev->data);
        }
        else {
            gpio_clear(dev->data);
        }
        xtimer_usleep(SHT1X_HALF_CLOCK);

        /* trigger clock signal */
        clk_signal(dev);

        /* shift value to write next bit */
        value <<= 1;
    }

    /* wait for ack */
    if (gpio_init(dev->data, GPIO_IN) == -1) {
        return -EIO;
    }
    xtimer_usleep(SHT1X_HALF_CLOCK);
    ack = gpio_read(dev->data);

    clk_signal(dev);

    return ack;
}

/*---------------------------------------------------------------------------*/
static int read_byte(const sht1x_dev_t *dev, uint8_t *dest, int ack)
{
    uint8_t value = 0;

    xtimer_usleep(SHT1X_HALF_CLOCK);

    /* read value bit by bit */
    for (int i = 0; i < 8; i++) {
        value <<= 1;
        gpio_set(dev->clk);
        xtimer_usleep(SHT1X_HALF_CLOCK);

        if (gpio_read(dev->data)) {
            /* set bit when DATA is high */
            value |= 0x01;
        }

        gpio_clear(dev->clk);
        xtimer_usleep(SHT1X_HALF_CLOCK);
    }

    /* send ack if necessary */
    if (gpio_init(dev->data, GPIO_OUT) == -1) {
        return -EIO;
    }

    gpio_write(dev->data, ack);
    xtimer_usleep(SHT1X_HALF_CLOCK);

    clk_signal(dev);

    /* release data line */
    if (gpio_init(dev->data, GPIO_IN) == -1) {
        return -EIO;
    }

    *dest = value;

    return 0;
}

/*---------------------------------------------------------------------------*/
static int transmission_start(const sht1x_dev_t *dev)
{
    /*       _____         ________
       DATA:      |_______|
                 ___     ___
       SCK : ___|   |___|   |______
     */
    if (gpio_init(dev->data, GPIO_OUT) == -1) {
        return -EIO;
    }

    /* set initial state */
    gpio_set(dev->data);
    xtimer_usleep(SHT1X_HALF_CLOCK);
    gpio_clear(dev->clk);
    xtimer_usleep(SHT1X_HALF_CLOCK);

    gpio_set(dev->clk);
    xtimer_usleep(SHT1X_HALF_CLOCK);

    gpio_clear(dev->data);
    xtimer_usleep(SHT1X_HALF_CLOCK);

    gpio_clear(dev->clk);
    xtimer_usleep(SHT1X_HALF_CLOCK);

    gpio_set(dev->clk);
    xtimer_usleep(SHT1X_HALF_CLOCK);

    gpio_set(dev->data);
    xtimer_usleep(SHT1X_HALF_CLOCK);

    gpio_clear(dev->clk);
    xtimer_usleep(SHT1X_HALF_CLOCK);

    if (gpio_init(dev->data, GPIO_IN) == -1) {
        return -EIO;
    }

    return 0;
}

/*---------------------------------------------------------------------------*/
static int connection_reset(const sht1x_dev_t *dev)
{
    /*       _____________________________________________________         ____
       DATA:                                                      |_______|
                _    _    _    _    _    _    _    _    _        ___     ___
       SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |__
     */
    if (gpio_init(dev->data, GPIO_OUT) == -1) {
        return -EIO;
    }

    gpio_set(dev->data);
    xtimer_usleep(SHT1X_HALF_CLOCK);
    gpio_clear(dev->clk);
    xtimer_usleep(SHT1X_HALF_CLOCK);

    for (int i = 0; i < 9; i++) {
        clk_signal(dev);
    }

    return transmission_start(dev);
}

/*---------------------------------------------------------------------------*/
static inline uint8_t crc_initial_value(uint8_t status)
{
    status &= 0x07;

    return (
        ((0x01 & status) << 7) |
        ((0x02 & status) << 5) |
        ((0x04 & status) << 3)
        );
}

/*---------------------------------------------------------------------------*/
static inline uint8_t reverse_byte(uint8_t value)
{
    uint8_t result = (value & 0x01) << 7;

    result |= (value & 0x02) << 5;
    result |= (value & 0x04) << 3;
    result |= (value & 0x08) << 1;
    result |= (value & 0x10) >> 1;
    result |= (value & 0x20) >> 3;
    result |= (value & 0x40) >> 5;
    result |= (value & 0x80) >> 7;

    return result;
}

/*---------------------------------------------------------------------------*/
static int measure(const sht1x_dev_t *dev, uint16_t *value, uint8_t mode)
{
    uint8_t data[2] = { 0, 0 };
    int retval;

    retval = transmission_start(dev);
    if (retval != 0) {
        return retval;
    }

    switch (write_byte(dev, mode)) {
        case -EIO:
            return -EIO;
        case 0:
            break;
        default:
        case 1:
            return -EPROTO;
    }

    /* wait until sensor has finished measurement or timeout */
    {
        int ack = 1;
        for (int i = 0; ack != 0; i++) {
            if (i > SHT1X_MEASURE_TIMEOUT) {
                return -ECANCELED;
            }

            xtimer_usleep(1000);
            ack = gpio_read(dev->data);
        }
    }

    /* read MSB */
    retval = read_byte(dev, &data[0], SHT1X_ACK);
    if (retval != 0) {
        return retval;
    }

    /* read LSB, send ACK only if CRC checking is enabled */
    retval = (dev->conf & SHT1X_CONF_SKIP_CRC) ? SHT1X_NO_ACK : SHT1X_ACK;
    retval = read_byte(dev, &data[1], retval);
    if (retval != 0) {
        return retval;
    }

    if (!(dev->conf & SHT1X_CONF_SKIP_CRC)) {
        uint8_t crc;
        uint8_t expected;

        retval = read_byte(dev, &crc, SHT1X_NO_ACK);
        if (retval != 0) {
            return retval;
        }

        expected = crc_initial_value(dev->conf);
        expected = crc_lookup_table[expected ^ mode];
        expected = crc_lookup_table[expected ^ data[0]];
        expected = crc_lookup_table[expected ^ data[1]];
        expected = reverse_byte(expected);
        if (expected != crc) {
            DEBUG("[sht1x] CRC expected: 0x%02x, got: 0x%02x\n"
                  "        CRC0: 0x%02x, CMD: 0x%02x, data: {0x%02x, 0x%02x}\n",
                  (int)expected, (int)crc,
                  (int)crc_initial_value(dev->conf), mode,
                  (int)data[0], (int)data[1]);
            return -EBADMSG;
        }
    }

    *value = (((uint16_t)data[0]) << 8) | (uint16_t)data[1];
    return 0;
}

/*---------------------------------------------------------------------------*/
int sht1x_init(sht1x_dev_t *dev, const sht1x_params_t *params)
{
    if (
        !dev ||
        !params ||
        (((uint8_t)params->vdd) >= sizeof(sht1x_d1) / sizeof(sht1x_d1[0]))
        ) {
        return -EINVAL;
    }

    dev->clk = params->clk;
    dev->data = params->data;
    if (gpio_init(dev->clk, GPIO_OUT) || gpio_init(dev->data, GPIO_IN)) {
        return -EIO;
    }

    dev->temp_off = 0;
    dev->hum_off = 0;
    dev->conf = 0;
    dev->vdd = (uint8_t)params->vdd;
    return sht1x_reset(dev);
}

/*---------------------------------------------------------------------------*/
int16_t sht1x_temperature(const sht1x_dev_t *dev, uint16_t raw)
{
    if (!dev || (dev->vdd >= sizeof(sht1x_d1) / sizeof(sht1x_d1[0]))) {
        return INT16_MIN;
    }

    int16_t d1 = sht1x_d1[dev->vdd];
    int16_t d2 = (dev->conf & SHT1X_CONF_LOW_RESOLUTION) ? 4 : 1;
    return d1 + d2 * ((int16_t)raw);
}

/*---------------------------------------------------------------------------*/
int16_t sht1x_humidity(const sht1x_dev_t *dev, uint16_t raw, int16_t temp)
{
    if (!dev) {
        return -1;
    }

    static const int32_t c1 = -20468;
    static const int32_t t1 = 1;
    int32_t c2, c3, c4, t2;
    if (dev->conf & SHT1X_CONF_LOW_RESOLUTION) {
        c2 = 5872;
        c3 = 494801;
        c4 = 1000000;
        t2 = 781;
    }
    else {
        c2 = 367;
        c3 = 791684;
        c4 = 100000;
        t2 = 12500;
    }

    /*
     * Calculate linear humidity, but slightly different. Original formula:
     *
     * hum_lin = c1 + c2 * raw + c3 * (raw * raw)
     *
     * But we use:
     *
     * hum_lin = c1 + c2 * raw - (c4 * raw / c3') * (c4 * raw / c3')
     *
     * where: c3' = 1 / (sqrt(-c3) / c4)
     *
     * (This better fits for integer calculation)
     */

    int32_t res = ((int32_t)raw * c4) / c3;
    res = c1 + c2 * (int32_t)raw - (res * res);

    /*
     * Perform temperature compensation, again slightly different.
     * Original formula:
     *
     * hum_true = (temp - 25) * (t1 + t2 * raw) + hum_lin
     *
     * But we use:
     *
     * hum_true = (temp - 25) * t1 + (temp - 25) * raw / t2') + hum_lin
     *
     * where t2' = 1/t2
     */
    int32_t temp_diff = temp - 2500;
    res = temp_diff * t1 + (temp_diff * (int32_t)raw * 100) / t2 + res;
    return (int16_t)(res / 100);
}


/*---------------------------------------------------------------------------*/
int sht1x_read(const sht1x_dev_t *dev, int16_t *temp, int16_t *rel_hum)
{
    uint16_t temp_raw;
    int16_t t;
    uint16_t hum_raw;
    int retval;

    if (
        !dev ||
        (dev->vdd >= sizeof(sht1x_d1) / sizeof(sht1x_d1[0])) ||
        (!temp && !rel_hum)
        ) {
        return -EINVAL;
    }

    retval = measure(dev, &temp_raw, SHT1X_MEASURE_TEMP);
    if (retval != 0) {
        connection_reset(dev);
        return retval;
    }

    t = sht1x_temperature(dev, temp_raw) + dev->temp_off;

    if (temp != NULL) {
        *temp = t;
    }

    if (rel_hum != NULL) {
        retval = measure(dev, &hum_raw, SHT1X_MEASURE_HUM);
        if (retval != 0) {
            connection_reset(dev);
            return retval;
        }

        *rel_hum = sht1x_humidity(dev, hum_raw, t) + dev->hum_off;
    }

    return 0;
}

/*---------------------------------------------------------------------------*/
int sht1x_configure(sht1x_dev_t *dev, sht1x_conf_t conf)
{
    if (!dev) {
        return -EINVAL;
    }

    /* Apply config that is not stored on the sensor */
    dev->conf &= SHT1X_CONF_MASK;
    dev->conf |= conf & (~(SHT1X_CONF_MASK));

    /* Send new status byte to sensor, if on-device config was changed */
    if ((conf & SHT1X_CONF_MASK) != (dev->conf & SHT1X_CONF_MASK)) {
        int retval = transmission_start(dev);
        if (retval != 0) {
            return retval;
        }

        switch (write_byte(dev, SHT1X_STATUS_REG_W)) {
            case -EIO:
                return -EIO;
            case 0:
                break;
            default:
            case 1:
                return -EPROTO;
        }

        switch (write_byte(dev, conf & SHT1X_CONF_MASK)) {
            case -EIO:
                return -EIO;
            case 0:
                break;
            default:
            case 1:
                return -EPROTO;
        }

        /* Read back uploaded configuration to verify that sensor applied it */
        uint8_t status;
        retval = sht1x_read_status(dev, &status);
        if (retval != 0) {
            return retval;
        }
        if (dev->conf != conf) {
            /* Configuration was not applied by sensor */
            return -ECANCELED;
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------*/
int sht1x_read_status(sht1x_dev_t *dev, uint8_t *status)
{
    int retval;

    if (!dev || !status) {
        return -EINVAL;
    }

    retval = transmission_start(dev);
    if (retval != 0) {
        return retval;
    }

    switch (write_byte(dev, SHT1X_STATUS_REG_R)) {
        case -EIO:
            return -EIO;
        case 0:
            break;
        default:
        case 1:
            return -EPROTO;
    }

    retval = read_byte(dev, status, SHT1X_ACK);
    if (retval != 0) {
        return retval;
    }

    {
        uint8_t crc;
        uint8_t expected;
        /* read checksum */
        retval = read_byte(dev, &crc, SHT1X_NO_ACK);
        if (retval != 0) {
            return retval;
        }

        expected = crc_initial_value(*status);
        expected = crc_lookup_table[expected ^ SHT1X_STATUS_REG_R];
        expected = crc_lookup_table[expected ^ *status];
        expected = reverse_byte(expected);
        if (expected != crc) {
            DEBUG("[sht1x] CRC expected: 0x%02x, got: 0x%02x\n"
                  "        CRC0: 0x%02x, CMD: 0x%02x, data: {0x%02x}\n",
                  (int)expected, (int)crc,
                  (int)crc_initial_value(*status), SHT1X_STATUS_REG_R,
                  (int)*status);
            return -EBADMSG;
        }
    }

    /* Extract config from status and store it after CRC check passed */
    dev->conf &= ~(SHT1X_CONF_MASK);
    dev->conf |= *status & SHT1X_CONF_MASK;

    return 0;
}

/*---------------------------------------------------------------------------*/
int sht1x_reset(sht1x_dev_t *dev)
{
    int retval;

    if (!dev) {
        return -EINVAL;
    }

    retval = transmission_start(dev);
    if (retval != 0) {
        return retval;
    }

    switch (write_byte(dev, SHT1X_RESET)) {
        case -EIO:
            return -EIO;
        case 0:
            break;
        default:
        case 1:
            return -EPROTO;
    }

    dev->conf = 0;
    xtimer_usleep(SHT1X_RESET_WAIT);

    return 0;
}
