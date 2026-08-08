/*
 * SPDX-FileCopyrightText: 2026 RIOT Contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "debug.h"
#include "log.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "byteorder.h"
#include "checksum/crc8.h"
#include "periph/i2c.h"
#include "sht4x.h"
#include "ztimer.h"

#define ASSERT_PARAM(cond) \
    do { \
        if (!(cond)) { \
            DEBUG("[sht4x] %s: %s\n", __func__, "parameter condition (" #cond ") not fulfilled"); \
            assert(cond); \
        } \
    } while (0)

#define DEBUG_DEV(f, d, ...) \
    DEBUG("[sht4x] %s dev=%d addr=%02x: " f "\n", __func__, d->i2c_dev, d->i2c_addr, ## __VA_ARGS__)

#define ERROR_DEV(f, d, ...) \
    LOG_ERROR("[sht4x] dev=%d addr=%02x: " f "\n", d->i2c_dev, d->i2c_addr, ## __VA_ARGS__)

#define SHT4X_CMD_RESET       0x94
#define SHT4X_CMD_READ_SERIAL 0x89

typedef struct __attribute__((packed)) {
    uint16_t temp_raw;
    uint8_t temp_crc;
    uint16_t rh_raw;
    uint8_t rh_crc;
} sht4x_data_t;

static const uint8_t SHT4X_MEASURE_CMD[3] = {
    0xE0, /* low repeatability */
    0xF6, /* medium repeatability */
    0xFD  /* high repeatability */
};

static const uint16_t SHT4X_MEASURE_WAIT_US[3] = {
    1600,
    4500,
    8300
};

static const uint8_t SHT4X_HEATER_CMD[3][2] = {
    { 0x39, 0x32 }, /* high power long, short */
    { 0x2F, 0x24 }, /* medium power long, short */
    { 0x1E, 0x15 }  /* low power long, short */
};

static const uint32_t SHT4X_HEATER_WAIT_US[2] = {
    1000000,
    100000
};

static inline uint8_t _crc8(const void *buf, size_t len)
{
    return crc8(buf, len, 0x31, 0xFF);
}

static int _send_command(sht4x_dev_t *dev, uint8_t cmd)
{
    int res;

    ASSERT_PARAM(dev != NULL);

    DEBUG_DEV("send command 0x%02x", dev, cmd);

    i2c_acquire(dev->i2c_dev);
    res = i2c_write_byte(dev->i2c_dev, dev->i2c_addr, cmd, 0);
    i2c_release(dev->i2c_dev);

    if (res != 0) {
        DEBUG_DEV("could not send command 0x%02x, reason=%d", dev, cmd, res);
        return -SHT4X_ERROR_I2C;
    }

    return SHT4X_OK;
}

static int _read_sample(sht4x_dev_t *dev, void *buf, size_t len)
{
    int res;

    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(buf != NULL);

    i2c_acquire(dev->i2c_dev);
    res = i2c_read_bytes(dev->i2c_dev, dev->i2c_addr, buf, len, 0);
    i2c_release(dev->i2c_dev);

    if (res != 0) {
        DEBUG_DEV("could not read %zu bytes, reason=%d", dev, len, res);
        return -SHT4X_ERROR_I2C;
    }

    return SHT4X_OK;
}

/* raw -> centi-units: val = raw * range / 0xFFFF - offset */
static int16_t _convert(uint16_t raw, int32_t range, int32_t offset)
{
    return (int32_t)((int64_t)raw * range / 0xFFFF) - offset;
}

static int _measure(sht4x_dev_t *dev, uint8_t cmd, uint32_t wait_us,
                     int16_t *temp, int16_t *hum)
{
    sht4x_data_t data;

    if (_send_command(dev, cmd) != SHT4X_OK) {
        return -SHT4X_ERROR_I2C;
    }

    ztimer_sleep(ZTIMER_USEC, wait_us);

    if (_read_sample(dev, &data, sizeof(data)) != SHT4X_OK) {
        return -SHT4X_ERROR_I2C;
    }

    if (_crc8(&data.temp_raw, sizeof(data.temp_raw)) != data.temp_crc) {
        DEBUG_DEV("temperature CRC failed", dev);
        return -SHT4X_ERROR_CRC;
    }

    if (_crc8(&data.rh_raw, sizeof(data.rh_raw)) != data.rh_crc) {
        DEBUG_DEV("humidity CRC failed", dev);
        return -SHT4X_ERROR_CRC;
    }

    if (temp != NULL) {
        /* accuracy ±0.1 °C, resolution below that is not meaningful */
        *temp = _convert(ntohs(data.temp_raw), 17500, 4500);
    }

    if (hum != NULL) {
        /* accuracy ±1 %RH, so a whole percent is sufficient resolution */
        int16_t rh = _convert(ntohs(data.rh_raw), 125, 6);
        *hum = (rh < 0) ? 0 : (rh > 100) ? 100 : rh;
    }

    return SHT4X_OK;
}

int sht4x_init(sht4x_dev_t *dev, const sht4x_params_t *params)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);

    dev->i2c_dev = params->i2c_dev;
    dev->i2c_addr = params->i2c_addr;
    dev->repeat = params->repeat;
    dev->heater_power = SHT4X_HEATER_POWER_HIGH;
    dev->heater_duration = SHT4X_HEATER_DURATION_LONG;

    if (_send_command(dev, SHT4X_CMD_RESET) != SHT4X_OK) {
        DEBUG_DEV("reset failed", dev);
        return -SHT4X_ERROR_I2C;
    }

    ztimer_sleep(ZTIMER_USEC, 1000);
    DEBUG_DEV("sensor initialized", dev);

    return SHT4X_OK;
}

int sht4x_read(sht4x_dev_t *dev, int16_t *temp, int16_t *hum)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(temp != NULL || hum != NULL);

    return _measure(dev, SHT4X_MEASURE_CMD[dev->repeat],
                     SHT4X_MEASURE_WAIT_US[dev->repeat], temp, hum);
}

int sht4x_read_with_heater(sht4x_dev_t *dev, int16_t *temp, int16_t *hum)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(temp != NULL || hum != NULL);

    return _measure(dev, SHT4X_HEATER_CMD[dev->heater_power][dev->heater_duration],
                     SHT4X_HEATER_WAIT_US[dev->heater_duration], temp, hum);
}

int sht4x_set_heater_power(sht4x_dev_t *dev, sht4x_heater_power_t power)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(power <= SHT4X_HEATER_POWER_LOW);

    dev->heater_power = power;

    return SHT4X_OK;
}

int sht4x_set_heater_duration(sht4x_dev_t *dev, sht4x_heater_duration_t duration)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(duration <= SHT4X_HEATER_DURATION_SHORT);

    dev->heater_duration = duration;

    return SHT4X_OK;
}
