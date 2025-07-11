/*
 * SPDX-FileCopyrightText: 2015 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_hih6130
 * @{
 *
 * @file
 * @brief       Device driver implementation for Honeywell HumidIcon Digital
 *              Humidity/Temperature Sensors: HIH-6130/6131 Series
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stddef.h>
#include <stdint.h>

#include "hih6130.h"
#include "periph/i2c.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
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
#define MEASUREMENT_DELAY   (50LU * US_PER_MS)

/** @brief Trigger a new measurement on the sensor */
static inline int hih6130_measurement_request(const hih6130_t *dev)
{
    i2c_acquire(dev->i2c);

    /* An empty write request triggers a new measurement */
    if (i2c_write_bytes(dev->i2c, dev->addr, NULL, 0, 0) < 0) {
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

static inline int hih6130_get_humidity_temperature_raw(const hih6130_t *dev, uint16_t *humidity_raw, uint16_t *temperature_raw)
{
    int status;
    uint8_t buf[HIH6130_FULL_DATA_LENGTH];

    i2c_acquire(dev->i2c);

    if (i2c_read_bytes(dev->i2c, dev->addr, &buf[0], sizeof(buf), 0) < 0) {
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

int hih6130_get_humidity_temperature(const hih6130_t *dev,
        int32_t *humidity_milli_percent, int32_t *temperature_milli_celsius)
{
    uint16_t hum_raw, temp_raw;
    int status;

    if (hih6130_measurement_request(dev) != 0) {
        return -1;
    }

    xtimer_usleep(MEASUREMENT_DELAY);

    status = hih6130_get_humidity_temperature_raw(dev, &hum_raw, &temp_raw);

    if (status < 0) {
        return -1;
    }

    if (humidity_milli_percent != NULL) {
        *humidity_milli_percent= (int32_t)hum_raw * 100000 / 16383;
    }

    if (temperature_milli_celsius != NULL) {
        *temperature_milli_celsius = (int32_t)temp_raw * 165000 / 16383 - 40000;
    }

    return status;
}
