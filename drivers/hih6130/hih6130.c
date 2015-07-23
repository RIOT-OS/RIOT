/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_hih6130
 * @{
 *
 * @file
 * @brief       Device driver implementation for Honeywell HumidIcon Digital
 *              Humidity/Temperature Sensors: HIH-6130/6131 Series
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include <stddef.h>
#include <stdint.h>

#include "hih6130.h"
#include "periph/i2c.h"
#include "timex.h"
#include "vtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* Humidity is stored in the first 2 bytes of data */
#define HIH6130_HUMIDITY_DATA_LENGTH (2)
/* Humidity + temperature data is 4 bytes long */
#define HIH6130_FULL_DATA_LENGTH (4)
/* Bit mask for the status bits in the first byte transferred */
#define HIH6130_STATUS_MASK      (0xc0)
/* Bit mask for the humidity data */
#define HIH6130_HUMIDITY_MASK    (0x3fff)
/* Temperature data is left adjusted within the word */
#define HIH6130_TEMPERATURE_SHIFT (2)

enum {
    HIH6130_STATUS_OK           = 0x00,
    /**
     * stale data: data that has already been fetched since the last measurement
     * cycle, or data fetched before the first measurement has been completed.
     */
    HIH6130_STATUS_STALE_DATA   = 0x40,
    HIH6130_STATUS_COMMAND_MODE = 0x80,
    HIH6130_STATUS_DIAGNOSTIC   = 0xc0,
};

/** @brief Delay between requesting a measurement and data becoming ready */
static const timex_t measurement_delay = {
        .seconds = 0, .microseconds = 50 * MS_IN_USEC, };

/** @brief Trigger a new measurement on the sensor */
static inline int hih6130_measurement_request(hih6130_t *dev)
{
    i2c_acquire(dev->i2c);

    /* An empty write request triggers a new measurement */
    if (i2c_write_bytes(dev->i2c, dev->addr, (char *)NULL, 0) < 0) {
        i2c_release(dev->i2c);
        return -1;
    }

    i2c_release(dev->i2c);

    return 0;
}

void hih6130_init(hih6130_t *dev, i2c_t i2c, uint8_t address)
{
    /* write device descriptor */
    dev->i2c = i2c;
    dev->addr = address;
}

static inline int hih6130_get_humidity_temperature_raw(hih6130_t *dev, uint16_t *humidity_raw, uint16_t *temperature_raw)
{
    int status;
    uint8_t buf[HIH6130_FULL_DATA_LENGTH];

    i2c_acquire(dev->i2c);

    if (i2c_read_bytes(dev->i2c, dev->addr, (char*)&buf[0], sizeof(buf)) != sizeof(buf)) {
        i2c_release(dev->i2c);
        return -1;
    }

    i2c_release(dev->i2c);

    /* data is in big-endian format, with status bits in the first byte. */
    switch (buf[0] & HIH6130_STATUS_MASK) {
        case HIH6130_STATUS_OK:
            status = 0;
            break;
        case HIH6130_STATUS_STALE_DATA:
            status = 1;
            break;
        default:
            return -2;
    }

    *humidity_raw    = ((buf[0] << 8) | buf[1]) & HIH6130_HUMIDITY_MASK;
    *temperature_raw = (((buf[2] << 8) | buf[3]) >> HIH6130_TEMPERATURE_SHIFT);

    return status;
}

int hih6130_get_humidity_temperature_float(hih6130_t *dev,
    float *relative_humidity_percent, float *temperature_celsius)
{
    uint16_t hum_raw, temp_raw;
    int status;

    if (hih6130_measurement_request(dev) != 0) {
        return -1;
    }

    vtimer_sleep(measurement_delay);

    status = hih6130_get_humidity_temperature_raw(dev, &hum_raw, &temp_raw);

    if (status < 0) {
        return -1;
    }

    if (relative_humidity_percent != NULL) {
        *relative_humidity_percent = hum_raw * (100.f / 16383.f);
    }
    if (temperature_celsius != NULL) {
        *temperature_celsius = temp_raw * (165.f / 16383.f) - 40.f;
    }

    return status;
}
