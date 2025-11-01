/*
 * SPDX-FileCopyrightText: 2020 Puhang Ding
 * SPDX-FileCopyrightText: 2020 Jan Schlichter
 * SPDX-FileCopyrightText: 2020 Nishchay Agrawal
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_scd30
 * @{
 * @file
 * @brief       Sensirion SCD30 sensor driver implementation
 *
 * @author      Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 * @author      Puhang Ding      <czarsir@gmail.com>
 * @author      Jan Schlichter   <schlichter@ibr.cs.tu-bs.de>
 * @}
 */

#include <string.h>
#include "periph/i2c.h"
#include "xtimer.h"
#include "byteorder.h"
#include "checksum/crc8.h"

#include "scd30.h"
#include "scd30_internal.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define SCD30_I2C           (dev->params.i2c_dev)
#define SCD30_I2C_ADDRESS   (dev->params.i2c_addr)

static int8_t _scd30_read_data(scd30_t *dev, scd30_measurement_t *result);
static bool _scd30_crc_check(uint8_t *buff, uint8_t len);
static bool _is_valid_interval(uint16_t *interval);
static bool _is_valid_press_comp(uint16_t *apc);
static inline float _reinterpret_float(uint32_t raw_val);
static float _raw_val_to_float(const uint8_t *buffer);

int8_t scd30_init(scd30_t *dev, const scd30_params_t *params)
{
    DEBUG("[scd30] init\n");
    int ret = 0;
    uint16_t version;

    dev->params = *params;

    /* Soft reset sensor */
    ret = scd30_reset(dev);
    if (ret < 0) {
        DEBUG("[scd30] Failed to reset sensor in init\n");
        return ret;
    }

    ret = scd30_get_param(dev, SCD30_VERSION, &version);
    DEBUG("[scd30] --- Version 0x%02x%02x\n", (version >> 8), version);
    if (ret < 0) {
        DEBUG("[scd30] Failed to get version\n");
        return ret;
    }

    DEBUG("[scd30] init: done.\n");

    return SCD30_OK;
}

int8_t scd30_set_param(const scd30_t *dev, uint16_t param, uint16_t val)
{
    uint8_t buffer[5];
    int ret = 0;

    buffer[0] = (uint8_t)(param >> 8);
    buffer[1] = (uint8_t)param;
    buffer[2] = (uint8_t)(val >> 8);
    buffer[3] = (uint8_t)val;
    buffer[4] = crc8(buffer + 2, 2, SCD30_CRC_FUNC, SCD30_CRC_START_VAL);
    i2c_acquire(SCD30_I2C);
    ret = i2c_write_bytes(SCD30_I2C, SCD30_I2C_ADDRESS, buffer, 5, 0);
    i2c_release(SCD30_I2C);
    if (ret != 0) {
        return SCD30_COM_FAILED;
    }
    else {
        xtimer_usleep(SCD30_READ_WRITE_SLEEP_US);
        return SCD30_OK;
    }
}

int8_t scd30_get_param(scd30_t *dev, uint16_t param, uint16_t *val)
{
    uint8_t buffer[3];
    int ret = 0;

    param = htons(param);
    i2c_acquire(SCD30_I2C);
    ret = i2c_write_bytes(SCD30_I2C, SCD30_I2C_ADDRESS, &param, 2, 0);
    if (ret != 0) {
        i2c_release(SCD30_I2C);
        return ret;
    }
    i2c_release(SCD30_I2C);

    xtimer_usleep(SCD30_READ_WRITE_SLEEP_US);

    i2c_acquire(SCD30_I2C);
    ret = i2c_read_bytes(SCD30_I2C, SCD30_I2C_ADDRESS, buffer, 3, 0);
    i2c_release(SCD30_I2C);

    if (ret != 0) {
        DEBUG("[scd30] Problem reading param %u from sensor\n", param);
    }

    if (!_scd30_crc_check(buffer, 2)) {
        return SCD30_CRC_ERROR;
    }

    *val = byteorder_bebuftohs(buffer);
    return SCD30_OK;
}

int8_t scd30_read_triggered(scd30_t *dev, scd30_measurement_t *result)
{
    uint16_t state = 0;
    uint16_t curr_interval;
    uint32_t initial_time;
    int ret;

    ret = scd30_get_param(dev, SCD30_INTERVAL, &curr_interval);
    if (ret != 0) {
        return ret;
    }
    if (curr_interval != SCD30_MIN_INTERVAL) {
        ret = scd30_set_param(dev, SCD30_INTERVAL, SCD30_MIN_INTERVAL);
        if (ret != 0) {
            DEBUG("[scd30] Error setting interval to min\n");
            return ret;
        }
    }

    /* Doing this to reduce the traffic on the I2C bus as
     * compared when constantly polling the device for status
     */
    xtimer_sleep(SCD30_MIN_INTERVAL);

    initial_time = xtimer_now_usec();
    ret = scd30_get_param(dev, SCD30_STATUS, &state);
    if (ret != 0) {
        return ret;
    }

    while (state != 1 &&
           (xtimer_now_usec() - initial_time) < SCD30_DATA_RDY_TIMEOUT) {
        ret = scd30_get_param(dev, SCD30_STATUS, &state);
        if (ret != 0) {
            return ret;
        }
    }
    ret = _scd30_read_data(dev, result);
    if (ret != SCD30_OK) {
        DEBUG("[scd30] Error reading data \n");
        return ret;
    }

    if (curr_interval != SCD30_MIN_INTERVAL) {
        ret = scd30_set_param(dev, SCD30_INTERVAL, curr_interval);
        if (ret != 0) {
            DEBUG("[scd30] Error resetting interval to original value\n");
            return ret;
        }
    }
    return SCD30_OK;
}

uint8_t scd30_read_periodic(scd30_t *dev, scd30_measurement_t *result)
{
    uint16_t state = 0;
    uint32_t initial_time;
    int ret = 0;

    initial_time = xtimer_now_usec();
    ret = scd30_get_param(dev, SCD30_STATUS, &state);
    if (ret != 0) {
        return ret;
    }

    while (state != 1 &&
           (xtimer_now_usec() - initial_time) < SCD30_DATA_RDY_TIMEOUT) {
        scd30_get_param(dev, SCD30_STATUS, &state);
    }

    if (state != 1) {
        return SCD30_NO_NEW_DATA;
    }
    ret = _scd30_read_data(dev, result);
    if (ret != SCD30_OK) {
        DEBUG("[scd30] Error reading values");
        return ret;
    }
    return SCD30_OK;
}

int scd30_start_periodic_measurement(scd30_t *dev, uint16_t *interval,
                                     uint16_t *apc)
{
    int ret = 0;

    /* Check if input is valid */
    if (!_is_valid_interval(interval)) {
        DEBUG("[scd30] Interval value out of range\n");
        return SCD30_INVALID_VALUE;
    }
    if (!_is_valid_press_comp(apc)) {
        DEBUG("[scd30] Ambient pressure compensation value out of range\n");
        return SCD30_INVALID_VALUE;
    }

    ret = scd30_set_param(dev, SCD30_INTERVAL, *interval);
    if (ret != 0) {
        return ret;
    }
    ret = scd30_set_param(dev, SCD30_START, *apc);
    if (ret != 0) {
        return ret;
    }
    return SCD30_OK;
}

int8_t scd30_stop_measurements(const scd30_t *dev)
{
    const uint16_t cmd = htons(SCD30_STOP);
    int ret;

    i2c_acquire(SCD30_I2C);
    ret = i2c_write_bytes(SCD30_I2C, SCD30_I2C_ADDRESS, &cmd, 2, 0);
    i2c_release(SCD30_I2C);
    if (ret != 0) {
        DEBUG("[scd30]: Error stopping measurements.\n");
    }
    return SCD30_OK;
}

int8_t scd30_reset(scd30_t *dev)
{
    const uint16_t cmd = htons(SCD30_SOFT_RESET);
    int ret;

    i2c_acquire(SCD30_I2C);
    ret = i2c_write_bytes(SCD30_I2C, SCD30_I2C_ADDRESS, &cmd, 2, 0);
    i2c_release(SCD30_I2C);

    if (ret != 0) {
        DEBUG("[scd30]: Error resetting sensor.\n");
        return ret;
    }
    xtimer_usleep(SCD30_RESET_SLEEP_US);
    return SCD30_OK;
}

/**
 *      intern function to read data
 *
 *      @param  dev      device
 *      @param  result   struct in which data read is stored
 *      @return          SCD30_OK on success
 */
static int8_t _scd30_read_data(scd30_t *dev, scd30_measurement_t *result)
{
    uint8_t buffer[18];
    int ret = 0;
    const uint16_t cmd = htons(SCD30_DATA);

    i2c_acquire(SCD30_I2C);
    ret = i2c_write_bytes(SCD30_I2C, SCD30_I2C_ADDRESS, &cmd, 2, 0);
    if (ret != 0) {
        i2c_release(SCD30_I2C);
        return ret;
    }
    ret = i2c_read_bytes(SCD30_I2C, SCD30_I2C_ADDRESS, buffer, 18, 0);
    if (ret != 0) {
        DEBUG("[scd30] read_data: ret %i.\n", ret);
        i2c_release(SCD30_I2C);
        return ret;
    }
    i2c_release(SCD30_I2C);

    if (!_scd30_crc_check(buffer, 2)) {
        return SCD30_CRC_ERROR;
    }
    if (!_scd30_crc_check(buffer + 3, 2)) {
        return SCD30_CRC_ERROR;
    }
    if (!_scd30_crc_check(buffer + 6, 2)) {
        return SCD30_CRC_ERROR;
    }
    if (!_scd30_crc_check(buffer + 9, 2)) {
        return SCD30_CRC_ERROR;
    }
    if (!_scd30_crc_check(buffer + 12, 2)) {
        return SCD30_CRC_ERROR;
    }
    if (!_scd30_crc_check(buffer + 15, 2)) {
        return SCD30_CRC_ERROR;
    }

    result->co2_concentration = _raw_val_to_float(&buffer[0]);
    result->temperature = _raw_val_to_float(&buffer[6]);
    result->relative_humidity = _raw_val_to_float(&buffer[12]);

    return SCD30_OK;
}

/**
 *      check if crc is valid (Assuming crc value to compare with is stored at
 *      end of buffer)
 *
 *      @param  buff  buffer of which crc value has to be checked
 *      @param  len   length of buffer
 *      @return       true on success
 */
static bool _scd30_crc_check(uint8_t *buff, uint8_t len)
{
    return crc8(buff, len, SCD30_CRC_FUNC, SCD30_CRC_START_VAL) == buff[len];
}

/**
 *      check if interval is within bounds
 *
 *      @param  interval   Pointer to interval to check
 *      @return            true if interval within bounds
 */
static bool _is_valid_interval(uint16_t *interval)
{
    if (*interval < SCD30_MIN_INTERVAL || *interval > SCD30_MAX_INTERVAL) {
        return false;
    }
    return true;
}

/**
 *      check if pressure compensation is within allowed values
 *
 *      @param  apc        Pointer to Pressure compensation to check
 *      @return            true if value within bounds
 */
static bool _is_valid_press_comp(uint16_t *apc)
{
    if ((*apc < SCD30_MIN_PRESSURE_COMP || *apc > SCD30_MAX_PRESSURE_COMP) &&
        *apc != 0) {
        return false;
    }
    return true;
}

/**
 *      convert IEEE754 stored in uint32_t to actual float value
 *
 *      @param  raw_val   Value to convert
 *      @return           Converted value
 */
static inline float _reinterpret_float(uint32_t raw_val)
{
    union {
        float float_val;
        uint32_t int_val;
    } to_float = { .int_val = raw_val };

    return to_float.float_val;
}

/**
 *      convert value from buffer storing IEEE754 represented
 *      float to actual float value
 *
 *      @param  buffer   buffer with value
 *      @return          Converted value
 */
static float _raw_val_to_float(const uint8_t *buffer)
{
    uint32_t tmp = byteorder_bebuftohl(buffer);

    return _reinterpret_float(tmp);
}
