/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_sgp30
 * @{
 *
 * @file
 * @brief       Device driver implementation for the sensors
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */
#include <string.h>

#include "checksum/crc8.h"
#include "irq.h"
#include "sgp30.h"
#include "sgp30_constants.h"
#include "sgp30_params.h"
#include "ztimer.h"

#define ENABLE_DEBUG    0
#include "debug.h"

/**
 * @name      Address pointer values for all SGP30 I2C commands
 * @{
 */
typedef enum {
    SGP30_CMD_INIT_AIR_QUALITY          = 0x2003,   /**< Start measurement mode */
    SGP30_CMD_MEASURE_AIR_QUALITY       = 0x2008,   /**< Measure air quality values */
    SGP30_CMD_GET_BASELINE              = 0x2015,   /**< Get baseline values */
    SGP30_CMD_SET_BASELINE              = 0x201e,   /**< Set baseline values CMD */
    SGP30_CMD_SET_HUMIDITY              = 0x2061,   /**< Set absolute humidity value
                                                         for compensation */
    SGP30_CMD_MEASURE_TEST              = 0x2032,   /**< Perform on-chip self test */
    SGP30_CMD_GET_FEATURE_SET_VERSION   = 0x202f,   /**< Get feature set version */
    SGP30_CMD_MEASURE_RAW_SIGNALS       = 0x2050,   /**< Read raw H2 and Ethanol signals */
    SGP30_CMD_READ_SERIAL               = 0x3682,   /**< Read serial number */
    SGP30_CMD_SOFT_RESET                = 0x0006,   /**< Perform General Call reset */
} sgp30_cmd_t;
/** @} */

/**
 * @name      Max delays for SGP30 I2C commands completion
 * @{
 */
#define SGP30_DELAY_INIT_AIR_QUALITY        (10 * US_PER_MS)
#define SGP30_DELAY_MEASURE_AIR_QUALITY     (12 * US_PER_MS)
#define SGP30_DELAY_GET_BASELINE            (10 * US_PER_MS)
#define SGP30_DELAY_SET_BASELINE            (10 * US_PER_MS)
#define SGP30_DELAY_SET_HUMIDITY            (10 * US_PER_MS)
#define SGP30_DELAY_MEASURE_TEST            (220 * US_PER_MS)
#define SGP30_DELAY_GET_FEATURE_SET_VERSION (2 * US_PER_MS)
#define SGP30_DELAY_MEASURE_RAW_SIGNALS     (25 * US_PER_MS)
#define SGP30_DELAY_READ_SERIAL             (500)
#define SGP30_DELAY_SOFT_RESET              (60 * US_PER_MS)
/** @} */

/* Device power up time */
#define SGP30_POWER_UP_TIME         (60 * US_PER_MS)
/* Polynomial used in crc check */
#define SGP30_CRC8_POLYNOMIAL       0x31
/* Seed used in crc check */
#define SGP30_CRC8_SEED             0xFF

static inline uint8_t _crc8(const void *buf, size_t len)
{
    return crc8(buf, len, SGP30_CRC8_POLYNOMIAL, SGP30_CRC8_SEED);
}

void _set_uint16_and_crc(uint8_t *buf, uint16_t *val)
{
    buf[0] = *val >> 8;
    buf[1] = *val & 0xFF;
    buf[2] = _crc8(buf, sizeof(uint16_t));
}

int _get_uint16_and_check_crc(uint8_t *buf, uint16_t *val)
{
    if (_crc8(buf, sizeof(uint16_t)) == buf[2]) {
        *val = (buf[0] << 8) + buf[1];
        return 0;
    }
    return -EBADMSG;
}

static int _rx_tx_data(sgp30_t *dev, uint16_t cmd, uint8_t *data,
                       size_t len, uint32_t delay, bool do_read)
{
    int res = 0;

    i2c_acquire(dev->params.i2c_dev);

    uint8_t frame_cmd[sizeof(cmd) + len];
    frame_cmd[0] = cmd >> 8;
    frame_cmd[1] = cmd & 0xFF;

    if (len == 0 || do_read) {
        res = i2c_write_bytes(dev->params.i2c_dev, SGP30_I2C_ADDRESS,
                              &frame_cmd[0], sizeof(cmd), 0);
    }
    if (res == 0 && do_read) {
        /* delay for command completion */
        if (!irq_is_in()) {
            ztimer_sleep(ZTIMER_USEC, delay);
        }
        else {
            ztimer_spin(ZTIMER_USEC, delay);
        }
        res = i2c_read_bytes(dev->params.i2c_dev, SGP30_I2C_ADDRESS,
                             data, len, 0);

    }
    else if (res == 0 && !do_read) {
        memcpy(&frame_cmd[2], data, len);
        res = i2c_write_bytes(dev->params.i2c_dev, SGP30_I2C_ADDRESS,
                              frame_cmd, sizeof(cmd) + len, 0);
        if (res) {
            DEBUG_PUTS("[sgp30]: failed to write data");
        }
    }

    i2c_release(dev->params.i2c_dev);

    return res;
}

int _read_measurements(sgp30_t *dev, sgp30_data_t *data)
{
    uint8_t frame[6];

    if (_rx_tx_data(dev, SGP30_CMD_MEASURE_AIR_QUALITY, frame, sizeof(frame),
                    SGP30_DELAY_MEASURE_AIR_QUALITY, true)) {
        return -EPROTO;
    }
    if (_get_uint16_and_check_crc(&frame[0], &data->eco2) ||
        _get_uint16_and_check_crc(&frame[3], &data->tvoc)) {
        return -EBADMSG;
    }
    return 0;
}

#ifdef MODULE_SGP30_STRICT
static void _read_cb(void *arg)
{
    sgp30_t *dev = (sgp30_t *)arg;

    if (!dev->ready) {
        dev->ready = true;
    }
    _read_measurements(dev, &dev->_data);
    dev->_data.timestamp = ztimer_now(ZTIMER_USEC);
    ztimer_set(ZTIMER_USEC, &dev->_timer, SGP30_RECOMMENDED_SAMPLING_PERIOD);
}
#endif

int sgp30_start_air_quality(sgp30_t *dev)
{
    int ret = _rx_tx_data(dev, SGP30_CMD_INIT_AIR_QUALITY, NULL, 0,
                          SGP30_DELAY_INIT_AIR_QUALITY, false);

#ifdef MODULE_SGP30_STRICT
    if (ret == 0) {
        ztimer_set(ZTIMER_USEC, &dev->_timer, SGP30_AIR_QUALITY_INIT_DELAY_US);
    }
#endif
    return ret ? -EPROTO : 0;
}

int sgp30_init(sgp30_t *dev, const sgp30_params_t *params)
{
    assert(dev && params);
    dev->params = *params;
#ifdef MODULE_SGP30_STRICT
    dev->ready = false;
    dev->_timer.callback = _read_cb;
    dev->_timer.arg = dev;
#endif

    /* delay while powering up */
    ztimer_sleep(ZTIMER_USEC, SGP30_POWER_UP_TIME);

    /* read future set */
    uint16_t version;
    sgp30_read_future_set(dev, &version);
    if (version < SGP30_REQUIRED_FEATURE_SET) {
        DEBUG("[sgp30]: wrong version number, %04x instead of %04x\n",
              version, SGP30_REQUIRED_FEATURE_SET);
        return -1;
    }

    /* read serial id */
    uint8_t serial[SGP30_SERIAL_ID_LEN];
    if (sgp30_read_serial_number(dev, serial, SGP30_SERIAL_ID_LEN)) {
        DEBUG_PUTS("[sgp30]: could not read serial number");
        return -1;
    }
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        DEBUG_PUTS("[sgp30]: serial");
        for (uint8_t i = 0; i < SGP30_SERIAL_ID_LEN; i++) {
            DEBUG("%02x ", serial[i]);
        }
        DEBUG_PUTS("\n");
    }

       /* start air quality measurement */
    if (sgp30_start_air_quality(dev)) {
        DEBUG_PUTS("[sgp30]: could not start air quality measurements ");
        return -1;
    }

    return 0;
}

int sgp30_reset(sgp30_t *dev)
{
    assert(dev);
    int ret = _rx_tx_data(dev, SGP30_CMD_SOFT_RESET, NULL, 0, SGP30_DELAY_SOFT_RESET, false);
#ifdef MODULE_SGP30_STRICT
    if (ret == 0) {
        ztimer_remove(ZTIMER_USEC, &dev->_timer);
        dev->ready = false;
    }
#endif
    return ret ? -EPROTO : 0;
}

int sgp30_read_serial_number(sgp30_t *dev, uint8_t *buf, size_t len)
{
    (void) len;
    assert(dev && buf && (len == SGP30_SERIAL_ID_LEN));
    uint8_t frame[9];
    if (_rx_tx_data(dev, SGP30_CMD_READ_SERIAL, (uint8_t *)frame, sizeof(frame),
                    SGP30_DELAY_READ_SERIAL, true)) {
        DEBUG_PUTS("[sgp30]: fail read");
        return -EPROTO;
    }
    /* the serial id is in big endian format */
    uint16_t tmp[SGP30_SERIAL_ID_LEN/2];
    if (_get_uint16_and_check_crc(&frame[0], &tmp[2]) ||
        _get_uint16_and_check_crc(&frame[3], &tmp[1]) ||
        _get_uint16_and_check_crc(&frame[6], &tmp[0])) {
        DEBUG_PUTS("[sgp30]: wrong crc");
        return -EBADMSG;
    }
    memcpy(buf, tmp, SGP30_SERIAL_ID_LEN);
    return 0;
}

int sgp30_read_future_set(sgp30_t *dev, uint16_t *version)
{
    uint8_t frame[3];

    if (_rx_tx_data(dev, SGP30_CMD_GET_FEATURE_SET_VERSION, frame, sizeof(frame),
                    SGP30_DELAY_GET_FEATURE_SET_VERSION, true)) {
        return -EPROTO;
    }
    if (_get_uint16_and_check_crc(&frame[0], version)) {
        DEBUG_PUTS("[sgp30]: wrong crc");
        return -EBADMSG;
    }
    return 0;
}

int sgp30_set_absolute_humidity(sgp30_t *dev, uint32_t a_humidity)
{
    /* max value is (255g/m3+255/256g/m3), or 255999 mg/m3*/
    if (a_humidity > 256000LU) {
        return -1;
    }

    /* scale down to g/m^3 */
    uint16_t humidity_scaled =
        (uint16_t)(((uint64_t)a_humidity * 256 * 16777) >> 24);

    uint8_t frame[3];
    _set_uint16_and_crc(&frame[0], &humidity_scaled);
    int ret = _rx_tx_data(dev, SGP30_CMD_SET_HUMIDITY, frame, sizeof(frame),
                          SGP30_DELAY_SET_HUMIDITY, false);
    return ret ? -EPROTO : 0;
}

int sgp30_set_baseline(sgp30_t *dev, sgp30_data_t *data)
{
    uint8_t frame[6];

    _set_uint16_and_crc(&frame[0], &data->eco2);
    _set_uint16_and_crc(&frame[3], &data->tvoc);

    int ret = _rx_tx_data(dev, SGP30_CMD_SET_BASELINE, frame, sizeof(frame),
                          SGP30_DELAY_SET_BASELINE, false);
    return ret ? -EPROTO : 0;
}

int sgp30_get_baseline(sgp30_t *dev, sgp30_data_t *data)
{
    uint8_t frame[6];

    if (_rx_tx_data(dev, SGP30_CMD_GET_BASELINE, frame, sizeof(frame),
                    SGP30_DELAY_GET_BASELINE, true)) {
        return -EPROTO;
    }
    if (_get_uint16_and_check_crc(&frame[0], &data->eco2) ||
        _get_uint16_and_check_crc(&frame[3], &data->tvoc)) {
        return -EBADMSG;
    }
    return 0;
}

int sgp30_read_measurements(sgp30_t *dev, sgp30_data_t *data)
{
#ifdef MODULE_SGP30_STRICT
    if (dev->ready) {
        unsigned state = irq_disable();
        memcpy(data, &dev->_data, sizeof(sgp30_data_t));
        irq_restore(state);
        return 0;
    }
    else {
        return -EAGAIN;
    }
    return 0;
#else
    return _read_measurements(dev, data);
#endif
}

int sgp30_read_raw_measurements(sgp30_t *dev, sgp30_raw_data_t *data)
{
    uint8_t frame[6];

    if (_rx_tx_data(dev, SGP30_CMD_MEASURE_RAW_SIGNALS, frame, sizeof(frame),
                    SGP30_DELAY_MEASURE_RAW_SIGNALS, true)) {
        return -EPROTO;
    }
    if (_get_uint16_and_check_crc(&frame[0], &data->raw_ethanol) ||
        _get_uint16_and_check_crc(&frame[3], &data->raw_h2)) {
        DEBUG_PUTS("[sgp30]: wrong crc");
        return -EBADMSG;
    }
    return 0;
}

#ifdef MODULE_SGP30_STRICT
bool sgp30_ready(sgp30_t *dev)
{
    return dev->ready;
}
#endif
