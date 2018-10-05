/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_htu21d
 * @brief       Device driver for the HTU21D humidity and temperature sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "htu21d_regs.h"
#include "htu21d.h"

#include "log.h"
#include "xtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG

#define ASSERT_PARAM(cond) \
    if (!(cond)) { \
        DEBUG("[htu21d] %s: %s\n", \
              __func__, "parameter condition (" # cond ") not fulfilled"); \
        assert(cond); \
    }

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[htu21d] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params.dev, d->params.addr, ## __VA_ARGS__);

#else /* ENABLE_DEBUG */

#define ASSERT_PARAM(cond) assert(cond);
#define DEBUG_DEV(f, d, ...)

#endif /* ENABLE_DEBUG */

#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[htu21d] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->params.dev, d->params.addr, ## __VA_ARGS__);

#define EXEC_RET(f) { \
    int _r; \
    if ((_r = f) != HTU21D_OK) { \
        DEBUG("[htu21d] %s: error code %d\n", __func__, _r); \
        return _r; \
    } \
}

#define EXEC_RET_CODE(f, c) { \
    int _r; \
    if ((_r = f) != HTU21D_OK) { \
        DEBUG("[htu21d] %s: error code %d\n", __func__, _r); \
        return c; \
    } \
}

/** Forward declaration of functions for internal use */

static int _user_reg_write(const htu21d_t *dev, uint8_t data);
static int _user_reg_read(const htu21d_t *dev, uint8_t *data);
static int _send_command(const htu21d_t *dev, uint8_t cmd);
static int _read_bytes(const htu21d_t* dev, uint8_t *data, uint8_t len);
static int _fetch_raw (htu21d_t *dev, int16_t *raw);
static uint8_t _crc8 (uint8_t data[], int len);

int htu21d_init(htu21d_t *dev, const htu21d_params_t *params)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);
    DEBUG_DEV("params=%p", dev, params);

    /* init sensor data structure */
    dev->params = *params;
    dev->measure_tmp = false;
    dev->measure_hum = false;

    /* reset the sensor and wait the start-up time*/
    EXEC_RET(_send_command(dev, HTU21D_SOFT_RESET));
    xtimer_usleep (15 * US_PER_MS);

    /* read user register */
    htu21d_user_reg_t user_reg;
    EXEC_RET(_user_reg_read(dev, (uint8_t *)&user_reg));

    switch (params->res) {
        case HTU21D_RES_RH12_T14:
                user_reg.resolution_1 = 0;
                user_reg.resolution_0 = 0;
                break;
        case HTU21D_RES_RH11_T11:
                user_reg.resolution_1 = 1;
                user_reg.resolution_0 = 1;
                break;
        case HTU21D_RES_RH10_T13:
                user_reg.resolution_1 = 1;
                user_reg.resolution_0 = 0;
                break;
        case HTU21D_RES_RH8_T12:
                user_reg.resolution_1 = 0;
                user_reg.resolution_0 = 1;
                break;
    }
    EXEC_RET(_user_reg_write(dev, user_reg.val));

    return HTU21D_OK;
}

static const uint8_t _times_tmp[] = { 85, 22, 43, 11 };
static const uint8_t _times_hum[] = { 29, 4, 9, 15 };

int htu21d_measure_tmp(htu21d_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    if (dev->measure_tmp || dev->measure_hum) {
        /* measurement can't be started if sensor is already measuring */
        return -HTU21D_ERROR_MEASURING;
    }

    EXEC_RET(_send_command(dev, HTU21D_TMP_MEAS));
    dev->measure_tmp = true;
    return _times_tmp[dev->params.res];
}

int htu21d_measure_hum(htu21d_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    if (dev->measure_tmp || dev->measure_hum) {
        /* measurement can't be started if sensor is already measuring */
        return -HTU21D_ERROR_MEASURING;
    }

    EXEC_RET(_send_command(dev, HTU21D_HUM_MEAS));
    dev->measure_hum = true;
    return _times_hum[dev->params.res];
}

int htu21d_fetch_tmp(htu21d_t *dev, int16_t* tmp, int16_t *raw_tmp)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("tmp=%p raw_tmp=%p", dev, tmp, raw_tmp);

    if (!dev->measure_tmp) {
        return -HTU21D_ERROR_NOT_MEASURING;
    }

    int16_t raw;
    EXEC_RET(_fetch_raw(dev, &raw));

    if (raw_tmp) {
        *raw_tmp = raw;
    }

    if (tmp) {
        *tmp =((17572 * raw) / 65536) - 4685;
    }

    return HTU21D_OK;
}

int htu21d_fetch_hum(htu21d_t *dev, int16_t* hum, int16_t *raw_hum)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("hum=%p raw_hum=%p", dev, hum, raw_hum);

    if (!dev->measure_hum) {
        return -HTU21D_ERROR_NOT_MEASURING;
    }

    int16_t raw;
    EXEC_RET(_fetch_raw(dev, &raw));

    if (raw_hum) {
        *raw_hum = raw;
    }

    if (hum) {
        *hum = ((12500 * raw) / 65536) - 600;
    }

    return HTU21D_OK;
}

int htu21d_read (htu21d_t *dev,
                 int16_t *tmp, int16_t *hum,
                 int16_t *raw_tmp, int16_t *raw_hum)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("tmp=%p hum=%p tmp_raw=%p raw_hum=%p",
              dev, tmp, hum, raw_tmp, raw_hum);

    int res;

    if (tmp || raw_tmp) {
        /* start temperature measurement */
        if ((res = htu21d_measure_tmp(dev)) <= 0) {
            DEBUG_DEV("could not start temperature measurement", dev);
            return res;
        }
        /* wait for results and fetch them */
        do {
            /* wait for results */
            xtimer_usleep(5 * US_PER_MS);
            /* try to fetch data, returns with error if still not available */
            res = htu21d_fetch_tmp(dev, tmp, raw_tmp);
            /* return if error code is not HTU21D_ERROR_MEASURING */
            if (res != HTU21D_OK && res != -HTU21D_ERROR_MEASURING) {
                return res;
            }
        } while (res != HTU21D_OK);
    }
    if (hum || raw_hum) {
        /* start humidity measurement */
        if ((res = htu21d_measure_hum(dev)) <= 0) {
            DEBUG_DEV("could not start humidity measurement", dev);
            return res;
        }
        do {
            /* wait for results */
            xtimer_usleep(5 * US_PER_MS);
            /* try to fetch data, returns with error if still not available */
            res = htu21d_fetch_hum(dev, hum, raw_hum);
            /* return if error code is not HTU21D_ERROR_MEASURING */
            if (res != HTU21D_OK && res != -HTU21D_ERROR_MEASURING) {
                return res;
            }
        } while (res != HTU21D_OK);
    }

    return HTU21D_OK;
}

/** Functions for internal use only */

int _fetch_raw(htu21d_t *dev, int16_t *raw)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(raw != NULL);
    DEBUG_DEV("raw=%p", dev, raw);

    if (!dev->measure_tmp && !dev->measure_hum) {
        return -HTU21D_ERROR_NOT_MEASURING;
    }

    uint8_t data[3];
    int res = _read_bytes(dev, data, 3);

    if (res == HTU21D_OK) {
        /* data MSB @ lower address */
        *raw = (data[0] << 8) | data[1];
        dev->measure_tmp = false;
        dev->measure_hum = false;
        /* check crc */
        if (_crc8(data,2) != data[2]) {
            DEBUG_DEV("CRC check for data failed", dev);
            return -HTU21D_ERROR_CRC;
        }
    }
    else if (res == -HTU21D_ERROR_I2C_NO_ACK) {
        return -HTU21D_ERROR_MEASURING;
    }

    return res;
}

static int _write_byte(const htu21d_t* dev, uint8_t data, uint8_t flags)
{
    int res = HTU21D_OK;

    DEBUG_DEV("send byte 0x%02x", dev, data);

    if (i2c_acquire(dev->params.dev) != HTU21D_OK) {
        DEBUG_DEV ("could not aquire I2C bus", dev);
        return -HTU21D_ERROR_I2C;
    }

    res = i2c_write_byte(dev->params.dev, dev->params.addr, data, flags);
    i2c_release(dev->params.dev);

    if (res != HTU21D_OK) {
        DEBUG_DEV("could not write byte 0x%02x to sensor, reason=%d",
                  dev, data, res);
        return -HTU21D_ERROR_I2C;
    }

    return res;
}

static int _read_bytes(const htu21d_t* dev, uint8_t *data, uint8_t len)
{
    int res = HTU21D_OK;

    if (i2c_acquire(dev->params.dev) != HTU21D_OK) {
        DEBUG_DEV ("could not aquire I2C bus", dev);
        return -HTU21D_ERROR_I2C;
    }

    res = i2c_read_bytes(dev->params.dev, dev->params.addr, (void*)data, len, 0);
    i2c_release(dev->params.dev);

    if (res == HTU21D_OK) {
        if (ENABLE_DEBUG) {
            printf("[htu21d] %s bus=%d addr=%02x: read following bytes: ",
                   __func__, dev->params.dev, dev->params.addr);
            for (int i=0; i < len; i++)
                printf("%02x ", data[i]);
            printf("\n");
        }
        return HTU21D_OK;
    }
    else if (res == -EIO) {
        DEBUG_DEV("no ack from sensor", dev);
        return -HTU21D_ERROR_I2C_NO_ACK;
    }
    else {
        DEBUG_DEV("could not read %d bytes from sensor, reason %d (%s)",
                  dev, len, res, strerror(res * -1));
        return -HTU21D_ERROR_I2C;
    }
}

static int _send_command(const htu21d_t *dev, uint8_t cmd)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("send command 0x%02x", dev, cmd);

    return _write_byte(dev, cmd, 0);
}

static int _user_reg_read(const htu21d_t *dev, uint8_t *data)
{
    EXEC_RET(_write_byte(dev, HTU21D_READ_USER_REG, I2C_NOSTOP));
    EXEC_RET(_read_bytes(dev, data, 1));
    return HTU21D_OK;
}

static int _user_reg_write(const htu21d_t *dev, uint8_t data)
{
    EXEC_RET(_write_byte(dev, HTU21D_WRITE_USER_REG, I2C_NOSTOP));
    EXEC_RET(_write_byte(dev, data, I2C_NOSTART));
    return HTU21D_OK;
}

static const uint8_t g_polynom = 0x31;

static uint8_t _crc8 (uint8_t data[], int len)
{
    /* initialization value */
    uint8_t crc = 0;

    /* iterate over all bytes */
    for (int i=0; i < len; i++)
    {
        crc ^= data[i];

        for (int i = 0; i < 8; i++)
        {
            bool xor = crc & 0x80;
            crc = crc << 1;
            crc = xor ? crc ^ g_polynom : crc;
        }
    }

    return crc;
}
