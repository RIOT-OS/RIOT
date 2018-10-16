/*
 * Copyright (C) 2017 George Psimenos (gp7g14@soton.ac.uk),
 * Copyright (C) 2018 Steffen Robertz (steffen.robertz@rwth-aachen.de)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sht2x
 * @{
 *
 * @file
 * @brief       Driver for the Sensirion SHT2x sensor series
 *
 * @author      George Psimenos <gp7g14@soton.ac.uk>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "assert.h"
#include "periph/i2c.h"
#include "xtimer.h"
#include "sht2x.h"
#include "sht2x_regs.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#define I2C_SPEED           I2C_SPEED_FAST
#define MAX_RETRIES         20

#define BUS                 (dev->params.i2c)
#define ADDR                (dev->params.addr)
#define MODE                (dev->params.measurement_mode)
#define CRC                 (dev->params.crc_mode)

enum {
    SHT2X_MEASURE_TEMP,
    SHT2X_MEASURE_RH
};


/*
 * does a crc check and returns 0 for passed. Anthing else for failed
 */
uint8_t __check_crc(uint8_t *rec_values, uint8_t right_crc)
{
    uint8_t crc = 0x00;
    uint8_t current_byte;
    uint8_t bit;

    for (current_byte = 0; current_byte < 2; current_byte++) {
        crc ^= (rec_values[current_byte]);
        for (bit = 8; bit > 0; bit--) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ SHT2X_CRC_POLYNOMIAL;
            }
            else {
                crc = (crc << 1);
            }
        }
    }
    return !(crc == right_crc);
}

void __sleep_during_measurement(uint8_t resolution)
{
    /*  sleep for the typical time it takes to complete the measurement
        this depends on the resolution and is taken from the datasheet */
    switch (resolution) {
        case SHT2X_RES_12_14BIT:
            xtimer_usleep(66 * US_PER_MS);
            break;
        case SHT2X_RES_8_12BIT:
            xtimer_usleep(17 * US_PER_MS);
            break;
        case SHT2X_RES_10_13BIT:
            xtimer_usleep(33 * US_PER_MS);
            break;
        case SHT2X_RES_11_11BIT:
            xtimer_usleep(12 * US_PER_MS);
            break;
    }
}

int16_t __measure_polling_mode(const sht2x_t *const dev, const uint8_t type, uint8_t *buffer)
{
    int16_t result = 0;

    switch (type) {
        case SHT2X_MEASURE_TEMP:
            result = i2c_write_byte(BUS, ADDR, SHT2X_TRIG_T_MEASUREMENT_POLL, 0);
            break;
        case SHT2X_MEASURE_RH:
            result = i2c_write_byte(BUS, ADDR, SHT2X_TRIG_RH_MEASUREMENT_POLL, 0);
            break;
    }
    /*  sleep for the typical time it takes to complete the measurement
        this depends on the resolution and is taken from the datasheet */
    __sleep_during_measurement(dev->params.resolution);
    uint8_t retries = 0;
    while ((result = i2c_read_bytes(BUS, ADDR, buffer, 3, 0))) {
        //DEBUG("result: %d\n", result);
        xtimer_usleep(3 * US_PER_MS);
        retries++;
        if (retries > MAX_RETRIES) {
            i2c_release(BUS);
            DEBUG("[sht2x] error: sensor read timed out\n");
            return SHT2X_READERROR;
        }
    }
    return result;
}

int sht2x_init(sht2x_t *dev, const sht2x_params_t *params)
{
    uint8_t user_reg;

    assert(dev && params);
    assert(params->resolution == SHT2X_RES_12_14BIT ||
           params->resolution == SHT2X_RES_8_12BIT  ||
           params->resolution == SHT2X_RES_10_13BIT ||
           params->resolution == SHT2X_RES_11_11BIT );

    /* write device descriptor */
    DEBUG("[sht2x] init: copying dev descriptor\n");
    memcpy(&(dev->params), params, sizeof(sht2x_params_t));

    /* reset the device - send a soft reset command */
    DEBUG("[sht2x] init: issuing soft reset\n");
    if (i2c_write_byte(BUS, ADDR, SHT2X_SOFT_RESET, 0)) {
        i2c_release(BUS);
        DEBUG("[sht2x] init - error: soft reset failed\n");
        return SHT2X_NODEV;
    }

    /* wait 15 ms for device to reset */
    DEBUG("[sht2x] init: waiting\n");
    i2c_release(BUS);
    xtimer_usleep(15 * US_PER_MS);
    i2c_acquire(BUS);

    /*  check device's responsiveness and identity by
        reading the user register after the reset we did
        and comparing with the default value which is 0x02 */
    DEBUG("[sht2x] init: reading user register\n");
    if (i2c_read_reg(BUS, ADDR, SHT2X_USER_REG_R, &user_reg, 0) || (user_reg & ~SHT2X_USER_RESERVED_MASK) != 0x02) {
        i2c_release(BUS);
        DEBUG("[sht2x] init - error: can't read user_reg or wrong value [0x%02x]\n", (int)user_reg);
        return SHT2X_NODEV;
    }

    /*  write desired resolution to user register
        while keeping the rest of the register the same
        (only if current resolution != desired resolution) */
    DEBUG("[sht2x] init: setting resolution\n");
    if ((user_reg & SHT2X_USER_RESOLUTION_MASK) != dev->params.resolution) {
        if (i2c_write_reg(BUS, ADDR, SHT2X_USER_REG_W, (dev->params.resolution | (user_reg & ~SHT2X_USER_RESOLUTION_MASK)), 0)) {
            i2c_release(BUS);
            DEBUG("[sht2x] init - error: user register write failed\n");
            return SHT2X_NODEV;
        }
    }

    DEBUG("[sht2x] init: releasing I2C\n");
    i2c_release(BUS);

    DEBUG("[sht2x] init: successful\n");

    return SHT2X_OK;
}

/* Returns °C (x100) */
int16_t sht2x_read_temp(sht2x_t *dev)
{
    int16_t result;
    uint8_t buffer[3];

    uint16_t st;
    float coeff;

    assert(dev);

    /* acquire I2C bus */
    DEBUG("[sht2x] read_temp: acquiring I2C\n");
    i2c_acquire(BUS);

    /* trigger measurement */
    DEBUG("[sht2x] read_temp: triggering temp read\n");
    if (MODE == SHT2X_CLOCKSTRETCHING_OFF) {
        result = __measure_polling_mode(dev, SHT2X_MEASURE_TEMP, buffer);
        DEBUG("Mode = Polling\n");
    }
    else {
        /* Use clock stretching as default mode as it is faster and easier */
        result = i2c_read_regs(BUS, ADDR, SHT2X_TRIG_T_MEASUREMENT_HM, buffer, 3, 0);
    }
    i2c_release(BUS);
    if (result) {
        DEBUG("[sht2x] read_temp - error: i2c read/write failed\n");
        return SHT2X_NODEV;
    }

    /* verify checksum */
    if (CRC == SHT2X_CRC_ON && __check_crc(buffer, buffer[2])) {
        DEBUG("CRC Error\n");
        return SHT2X_CRCERROR;
    }

    /* last two bits are status bits and need to be set to 0 */
    st = (buffer[0] << 8 | buffer[1]) & 0xfffc;
    coeff = st / 65536.0;
    dev->values.temp = round(-4685 + (17572 * coeff));
    DEBUG("Read Temp Value: %d, Coeff %f \n", st, coeff);
    return SHT2X_OK;
}

/* Returns % RH (x100) */
int16_t sht2x_read_humidity(sht2x_t *dev)
{
    int16_t result;
    uint8_t buffer[3];

    uint16_t srh;
    float coeff;

    assert(dev);

    /* acquire I2C bus */
    DEBUG("[sht2x] read_humidity: acquiring I2C\n");
    i2c_acquire(BUS);

    /* trigger measurement */
    DEBUG("[sht2x] read_humidity: triggering humidity read\n");
    if (MODE == SHT2X_CLOCKSTRETCHING_OFF) {
        result = __measure_polling_mode(dev, SHT2X_MEASURE_RH, buffer);
    }
    else {
        /* Use clock stretching as default mode as it is faster and easier */
        result = i2c_read_regs(BUS, ADDR, SHT2X_TRIG_RH_MEASUREMENT_HM, buffer, 3, 0);
    }
    i2c_release(BUS);
    if (result) {
        DEBUG("[sht2x] read_humidity - error: i2c read/write failed\n");
        return SHT2X_NODEV;
    }

    /* verify checksum */
    if (CRC == SHT2X_CRC_ON && __check_crc(buffer, buffer[2])) {
        DEBUG("CRC Error\n");
        return SHT2X_CRCERROR;
    }
    /* last two bits are status bits and need to be set to 0 */
    srh = (buffer[0] << 8 | buffer[1]) & 0xfffc;
    coeff = srh / 65536.0;
    dev->values.rel_humidity = round(-600 + 12500 * coeff);
    DEBUG("Read Humidity Value: %d\n", srh);
    return SHT2X_OK;
}

int8_t sht2x_reset(const sht2x_t *const dev)
{
    /* send reset command */
    i2c_acquire(BUS);
    int check = i2c_write_byte(BUS, ADDR, SHT2X_SOFT_RESET, 0);
    i2c_release(BUS);
    /* check how many bytes were written to the bus for verification */
    return (check == 0) ? SHT2X_OK : SHT2X_NODEV;
}

uint8_t sht2x_read_settings(sht2x_t *const dev)
{
    /* send the read settings command */
    i2c_acquire(BUS);
    int check = i2c_read_reg(BUS, ADDR, SHT2X_USER_REG_R, &(dev->values.current_settings), 0);
    i2c_release(BUS);
    /* check how many bytes were written to the bus for verification */
    return (check == 0) ? SHT2X_OK : SHT2X_NODEV;
}

uint8_t sht2x_write_settings(sht2x_t *const dev, const uint8_t setting)
{
    /* write settings to sensor */
    i2c_acquire(BUS);
    int check = i2c_write_reg(BUS, ADDR, SHT2X_USER_REG_W, setting, 0);
    i2c_release(BUS);
    sht2x_read_settings(dev);
    /* check how many bytes were written to the bus for verification */
    return (check == 0) ? SHT2X_OK : SHT2X_NODEV;
}

uint8_t sht2x_is_end_of_battery(sht2x_t *const dev)
{
    return ((sht2x_read_settings(dev) & SHT2X_USER_EOB_MASK) >> 6);
}
