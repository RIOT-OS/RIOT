/*
 * Copyright (C) 2016,2017,2018 Kees Bakker, SODAQ
 *               2017 George Psimenos
 *               2018 Steffen Robertz
 *               2022 Gunar Schorcht
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
 * @brief       Device driver implementation for the SHT2x humidity and
 *              temperature humidity sensor.
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      George Psimenos <gp7g14@soton.ac.uk>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 *
 * @}
 */

#include <math.h>

#include "checksum/crc8.h"
#include "log.h"
#include "periph/i2c.h"
#include "ztimer.h"

#include "sht2x.h"
#include "sht2x_params.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/**
 * @brief       The number of retries when doing a polled measurement
 */
#define MAX_RETRIES         20

/**
 * @brief       A few helper macros
 */
#define _BUS                (dev->params.i2c_dev)
#define _ADDR               (dev->params.i2c_addr)

typedef enum {
    temp_hold_cmd       = 0xE3,     /**< trigger temp measurement, hold master */
    hum_hold_cmd        = 0xE5,     /**< trigger humidity measurement, hold master */
    write_user_cmd      = 0xE6,     /**< write user register */
    read_user_cmd       = 0xE7,     /**< read user register */
    temp_no_hold_cmd    = 0xF3,     /**< trigger temp measurement, no hold master (poll) */
    hum_no_hold_cmd     = 0xF5,     /**< trigger humidity measurement, no hold master (poll) */
    soft_reset_cmd      = 0xFE,     /**< soft reset */
} cmd_t;

typedef enum {
    SHT2X_MEASURE_TEMP,
    SHT2X_MEASURE_RH,
} measure_type_t;

/**
 * @brief   Register addresses to read SHT2x Identification Code.
 */
static const uint16_t first_mem_addr = 0x0FFA;
static const uint16_t second_mem_addr = 0xC9FC;

static int read_sensor(const sht2x_t* dev, cmd_t command, uint16_t *val);
static int read_sensor_poll(const sht2x_t* dev, cmd_t command, uint16_t *val);
static uint8_t sht2x_checkcrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum);
static void sleep_during_temp_measurement(sht2x_res_t res);
static void sleep_during_hum_measurement(sht2x_res_t resolution);

/*---------------------------------------------------------------------------*
 *                          SHT2x Core API                                   *
 *---------------------------------------------------------------------------*/

int sht2x_init(sht2x_t* dev, const sht2x_params_t* params)
{
    int i2c_result;

    dev->params = *params;
    if (dev->params.resolution != SHT2X_RES_12_14BIT &&
        dev->params.resolution != SHT2X_RES_8_12BIT &&
        dev->params.resolution != SHT2X_RES_10_13BIT &&
        dev->params.resolution != SHT2X_RES_11_11BIT) {
        return SHT2X_ERR_RES;
    }

    i2c_result = sht2x_reset(dev);
    if (i2c_result != SHT2X_OK) {
        return SHT2X_ERR_I2C;
    }
    /* wait 15 ms for device to reset */
    ztimer_sleep(ZTIMER_MSEC, 15);

    uint8_t userreg;
    uint8_t userreg2;
    i2c_result = sht2x_read_userreg(dev, &userreg);
    if (i2c_result != SHT2X_OK) {
        return i2c_result;
    }
    DEBUG("[SHT2x] User Register=%02x\n", userreg);

    if ((userreg & SHT2X_USER_RESOLUTION_MASK) != (uint8_t)dev->params.resolution) {
        userreg &= ~SHT2X_USER_RESOLUTION_MASK;
        userreg |= (uint8_t)dev->params.resolution;
        i2c_result = sht2x_write_userreg(dev, userreg);
        if (i2c_result != SHT2X_OK) {
            return i2c_result;
        }
        i2c_result = sht2x_read_userreg(dev, &userreg2);
        if (i2c_result != SHT2X_OK) {
            return i2c_result;
        }
        DEBUG("[SHT2x] New User Register=%02x\n", userreg2);
        if (userreg != userreg2) {
            return SHT2X_ERR_USERREG;
        }
    }

    return SHT2X_OK;
}

int sht2x_reset(sht2x_t* dev)
{
    int i2c_result;
    cmd_t command = soft_reset_cmd;

    /* Acquire exclusive access */
    i2c_acquire(_BUS);

    DEBUG("[SHT2x] write command: addr=%02x cmd=%02x\n", _ADDR, (uint8_t)command);
    i2c_result = i2c_write_byte(_BUS, _ADDR, (uint8_t)command, 0);

    i2c_release(_BUS);

    if (i2c_result != 0) {
        return SHT2X_ERR_I2C;
    }

    return SHT2X_OK;
}

/*
 * Returns temperature in centi DegC.
 */
int16_t sht2x_read_temperature(const sht2x_t* dev)
{
    uint16_t raw_value;
    int i2c_result;
    if (dev->params.measure_mode == SHT2X_MEASURE_MODE_NO_HOLD) {
        i2c_result = read_sensor_poll(dev, temp_no_hold_cmd, &raw_value);
    } else {
        i2c_result = read_sensor(dev, temp_hold_cmd, &raw_value);
    }
    if (i2c_result != SHT2X_OK) {
        return INT16_MIN;
    }
    return ((17572 * raw_value) / 65536) - 4685;
}

/*
 * Returns humidity in centi %RH (i.e. the percentage times 100).
 */
uint16_t sht2x_read_humidity(const sht2x_t *dev)
{
    uint16_t raw_value;
    int i2c_result;
    if (dev->params.measure_mode == SHT2X_MEASURE_MODE_NO_HOLD) {
        i2c_result = read_sensor_poll(dev, hum_no_hold_cmd, &raw_value);
    } else {
        i2c_result = read_sensor(dev, hum_hold_cmd, &raw_value);
    }
    if (i2c_result != SHT2X_OK) {
        return 0;
    }
    return ((12500 * raw_value) / 65536) - 600;
}

static size_t _sht2x_add_ident_byte(uint8_t * buffer, size_t buflen, uint8_t b, size_t ix)
{
    if (ix < buflen) {
        buffer[ix++] = b;
    }
    return ix;
}

int sht2x_read_ident(const sht2x_t *dev, uint8_t * buffer, size_t buflen)
{
    uint8_t data1[8];        /* SNB_3, CRC, SNB_2, CRC, SNB_1, CRC, SNB_0, CRC */
    uint8_t data2[6];        /* SNC_1, SNC_0, CRC, SNA_1, SNA_0, CRC */
    size_t ix;
    int res;

    i2c_acquire(_BUS);
    res = i2c_read_regs(_BUS, _ADDR,
                        first_mem_addr, data1, sizeof(data1), I2C_REG16);
    i2c_release(_BUS);
    if (res < 0) {
        return res;
    }

    DEBUG("[SHT2x] ident (1): %02x %02x %02x %02x\n", data1[0], data1[1], data1[2], data1[3]);
    DEBUG("[SHT2x] ident (1): %02x %02x %02x %02x\n", data1[4], data1[5], data1[6], data1[7]);
    for (size_t ix = 0; ix < sizeof(data1); ix += 2) {
        if (sht2x_checkcrc(&data1[ix], 1, data1[ix + 1]) != 0) {
            DEBUG("[SHT2x] checksum error first (ix=%d)\n", ix);
            return SHT2X_ERR_CRC;
        }
    }

    i2c_acquire(_BUS);
    res = i2c_read_regs(_BUS, _ADDR,
                        second_mem_addr, data2, sizeof(data2), I2C_REG16);
    i2c_release(_BUS);
    if (res < 0) {
        return res;
    }
    DEBUG("[SHT2x] ident (2): %02x %02x %02x\n", data2[0], data2[1], data2[2]);
    DEBUG("[SHT2x] ident (2): %02x %02x %02x\n", data2[3], data2[4], data2[5]);
    for (size_t ix = 0; ix < sizeof(data2); ix += 3) {
        if (sht2x_checkcrc(&data2[ix], 2, data2[ix + 2]) != 0) {
            DEBUG("[SHT2x] checksum error, second (ix=%d)\n", ix);
            return SHT2X_ERR_CRC;
        }
    }

    /*
     * See Sensirion document Electronic_Identification_Code_SHT2x_V1-1_C2
     *
     * first memory address:
     *  SNB_3, CRC, SNB_2, CRC, SNB_1, CRC, SNB_0, CRC,
     * Second memory address:
     *  SNC_1, SNC_0, CRC, SNA_1, SNA_0, CRC
     *
     * To assemble the Identification code:
     *  SNA_1, SNA_0, SNB_3, SNB_2, SNB_1, SNB_0, SNC_1, SNC_0
     */
    if (buffer == NULL) {
        return 0;
    }
    ix = 0;
    ix = _sht2x_add_ident_byte(buffer, buflen, data2[3], ix);
    ix = _sht2x_add_ident_byte(buffer, buflen, data2[4], ix);
    ix = _sht2x_add_ident_byte(buffer, buflen, data1[0], ix);
    ix = _sht2x_add_ident_byte(buffer, buflen, data1[2], ix);
    ix = _sht2x_add_ident_byte(buffer, buflen, data1[4], ix);
    ix = _sht2x_add_ident_byte(buffer, buflen, data1[6], ix);
    ix = _sht2x_add_ident_byte(buffer, buflen, data2[0], ix);
    ix = _sht2x_add_ident_byte(buffer, buflen, data2[1], ix);
    return ix;
}

int sht2x_read_userreg(const sht2x_t *dev, uint8_t * userreg)
{
    cmd_t command = read_user_cmd;

    if (userreg) {
        int i2c_result;
        DEBUG("[SHT2x] read command: addr=%02x cmd=%02x\n", _ADDR, (uint8_t)command);
        i2c_acquire(_BUS);
        i2c_result = i2c_read_reg(_BUS, _ADDR, (uint8_t)command, userreg, 0);
        i2c_release(_BUS);
        if (i2c_result != 0) {
            return SHT2X_ERR_I2C_READ;
        }
    } else {
        return SHT2X_ERR_OTHER;
    }

    return SHT2X_OK;
}

int sht2x_write_userreg(const sht2x_t *dev, uint8_t userreg)
{
    cmd_t command = write_user_cmd;
    int i2c_result;
    DEBUG("[SHT2x] write command: addr=%02x cmd=%02x\n", _ADDR, (uint8_t)command);

    i2c_acquire(_BUS);
    i2c_result = i2c_write_reg(_BUS, _ADDR, (uint8_t)command, userreg, 0);
    i2c_release(_BUS);
    if (i2c_result != 0) {
        return SHT2X_ERR_I2C;
    }

    return SHT2X_OK;
}

/******************************************************************************
 * Local Functions
 ******************************************************************************/

/**
 * @brief       Read a sensor value from the given SHT2X device
 *
 * @param[in]  dev          Device descriptor of SHT2X device to read from
 * @param[in]  command      The SHT2x command (hold mode only)
 * @param[out] val          The raw sensor value (only valid if no error)
 *
 * @return                  SHT2X_OK value is returned in @p val
 * @return                  SHT2X_NODEV if sensor communication failed
 * @return                  SHT2X_ERR_OTHER if parameters are invalid
 * @return                  SHT2X_ERR_TIMEDOUT if sensor times out
 * @return                  SHT2X_ERR_CRC if the checksum is wrong
 */
static int read_sensor(const sht2x_t* dev, cmd_t command, uint16_t *val)
{
    uint8_t buffer[3];
    int i2c_result;

    /* Acquire exclusive access */
    i2c_acquire(_BUS);

    DEBUG("[SHT2x] write command: addr=%02x cmd=%02x\n", _ADDR, (uint8_t)command);
    (void)i2c_write_byte(_BUS, _ADDR, (uint8_t)command, 0);
    i2c_result = i2c_read_bytes(_BUS, _ADDR, buffer, sizeof(buffer), 0);
    i2c_release(_BUS);
    if (i2c_result != 0) {
        DEBUG("[Error] Cannot read SHT2x sensor data.\n");
        return SHT2X_ERR_I2C_READ;
    }

    DEBUG("[SHT2x] read: %02x %02x %02x\n", buffer[0], buffer[1], buffer[2]);
    if (val) {
        *val = (buffer[0] << 8) | buffer[1];
        *val &= ~0x0003;            /* clear two low bits (status bits) */
    }

    if (dev->params.is_crc_enabled) {
        /* byte #3 is the checksum */
        if (sht2x_checkcrc(buffer, 2, buffer[2]) != 0) {
            return SHT2X_ERR_CRC;
        }
    }

    return SHT2X_OK;
}

/**
 * @brief       Read a sensor value from the given SHT2X device, polling mode
 *
 * @param[in]  dev          Device descriptor of SHT2X device to read from
 * @param[in]  command      The SHT2x command (hold mode only)
 * @param[out] val          The raw sensor value (only valid if no error)
 *
 * @return                  SHT2X_OK value is returned in @p val
 * @return                  SHT2X_NODEV if sensor communication failed
 * @return                  SHT2X_ERR_OTHER if parameters are invalid
 * @return                  SHT2X_ERR_TIMEDOUT if sensor times out
 * @return                  SHT2X_ERR_CRC if the checksum is wrong
 */
static int read_sensor_poll(const sht2x_t* dev, cmd_t command, uint16_t *val)
{
    uint8_t buffer[3];
    int i2c_result;

    /* acquire the bus for exclusive access */
    i2c_acquire(_BUS);

    DEBUG("[SHT2x] write command: addr=%02x cmd=%02x\n", _ADDR, (uint8_t)command);
    (void)i2c_write_byte(_BUS, _ADDR, (uint8_t)command, 0);
    /* release the bus for measurement duration */
    i2c_release(_BUS);

    /* sleep for measurement duration */
    if (command == temp_no_hold_cmd) {
        sleep_during_temp_measurement(dev->params.resolution);
    } else {
        sleep_during_hum_measurement(dev->params.resolution);
    }

    /* reacquire the bus for exclusive access */
    i2c_acquire(_BUS);

    uint8_t ix = 0;
    for (; ix < MAX_RETRIES; ix++) {
        i2c_result = i2c_read_bytes(_BUS, _ADDR, buffer, sizeof(buffer), 0);
        if (i2c_result == 0) {
            break;
        }
    }

    i2c_release(_BUS);
    if (i2c_result != 0) {
        DEBUG("[Error] Cannot read SHT2x sensor data.\n");
        return SHT2X_ERR_I2C_READ;
    }

    DEBUG("[SHT2x] read: %02x %02x %02x\n", buffer[0], buffer[1], buffer[2]);
    if (val) {
        *val = (buffer[0] << 8) | buffer[1];
        *val &= ~0x0003;            /* clear two low bits (status bits) */
    }

    /* byte #3 is the checksum */
    if (dev->params.is_crc_enabled) {
        if (sht2x_checkcrc(buffer, 2, buffer[2]) != 0) {
            return SHT2X_ERR_CRC;
        }
    }

    return SHT2X_OK;
}

static const uint8_t POLYNOMIAL = 0x31;       /* P(x)=x^8+x^5+x^4+1 = 100110001 */
/**
 * @brief       Calculate 8-Bit checksum with given polynomial
 */
static uint8_t sht2x_checkcrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum)
{
    return crc8(data, nbrOfBytes, POLYNOMIAL, 0) != checksum;
}

/**
 * @brief       Sleep during measurement
 *
 * @param[in]   res     The resolution bits in the User Register
 *
 * @details     Sleep for the maximum time it takes to complete the measurement
 *              this depends on the resolution and is taken from the datasheet.
 *              Measurement time differs for temperature and humidity.
 *
 * @note        According to the data sheet, typical times are recommended for
 *              calculating energy consumption, while maximum values should be
 *              used for calculating waiting times in communication.
 */
static void sleep_during_temp_measurement(sht2x_res_t res)
{
    uint32_t amount_ms = 0;

    switch (res) {
        case SHT2X_RES_12_14BIT:
            amount_ms = 85;
            break;
        case SHT2X_RES_8_12BIT:
            amount_ms = 22;
            break;
        case SHT2X_RES_10_13BIT:
            amount_ms = 43;
            break;
        case SHT2X_RES_11_11BIT:
            amount_ms = 11;
            break;
    }
    ztimer_sleep(ZTIMER_MSEC, amount_ms);
}

static void sleep_during_hum_measurement(sht2x_res_t resolution)
{
    uint32_t amount_ms = 0;

    switch (resolution) {
        case SHT2X_RES_12_14BIT:
            amount_ms = 29;
            break;
        case SHT2X_RES_8_12BIT:
            amount_ms = 4;
            break;
        case SHT2X_RES_10_13BIT:
            amount_ms = 9;
            break;
        case SHT2X_RES_11_11BIT:
            amount_ms = 15;
            break;
    }
    ztimer_sleep(ZTIMER_MSEC, amount_ms);
}
