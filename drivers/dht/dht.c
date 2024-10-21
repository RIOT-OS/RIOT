/*
 * Copyright 2015 Ludwig Knüpfer
 *           2015 Christian Mehlis
 *           2016-2017 Freie Universität Berlin
 *           2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dht
 * @{
 *
 * @file
 * @brief       Device driver implementation for the DHT11, 21 and 22
 *              temperature and humidity sensor
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "assert.h"
#include "dht.h"
#include "dht_params.h"
#include "log.h"
#include "periph/gpio.h"
#include "time_units.h"
#include "ztimer.h"

#define ENABLE_DEBUG            0
#include "debug.h"

/* Every pulse send by the DHT longer than 40µs is interpreted as 1 */
#define READ_THRESHOLD          (40U)
/* If an expected pulse is not detected within 85µs, something is wrong */
#define SPIN_TIMEOUT            (85U)
/* The start signal by pulling data low for at least 18 ms for DHT11, at
 * most 20 ms (AM2301 / DHT22 / DHT21). Then release the bus and the
 * sensor should respond by pulling data low for 80 µs, then release for
 * 80µs before start sending data. */
#define START_LOW_TIME          (19U * US_PER_MS)
#define START_THRESHOLD         (75U)
/* DHTs have to wait for power 1 or 2 seconds depending on the model */
#define POWER_WAIT_TIMEOUT      (2U * US_PER_SEC)

enum {
    BYTEPOS_HUMIDITY_HIGH = 0,
    BYTEPOS_HUMIDITY_LOW = 1,
    BYTEPOS_TEMPERATURE_HIGH = 2,
    BYTEPOS_TEMPERATURE_LOW = 3,
    BYTEPOS_CHECKSUM = 4,
};

struct dht_data {
    gpio_t pin;
    gpio_mode_t in_mode;
    uint8_t data[5];
    int8_t bit_pos;
    uint8_t bit;
};

static void _wait_for_level(gpio_t pin, bool expected, uint32_t start)
{
    /* Calls to ztimer_now() can be relatively slow on low end platforms.
     * Mixing in a busy down-counting loop solves issues e.g. on AVR boards. */
    uint8_t pre_timeout = 0;
    while (((bool)gpio_read(pin) != expected) && (++pre_timeout
        || ztimer_now(ZTIMER_USEC) < start + SPIN_TIMEOUT)) {}
}

static int _send_start_signal(dht_t *dev)
{
    uint32_t start;
    gpio_init(dev->params.pin, GPIO_OUT);
    gpio_clear(dev->params.pin);
    ztimer_sleep(ZTIMER_USEC, START_LOW_TIME);
    /* sync on device */
    gpio_set(dev->params.pin);
    gpio_init(dev->params.pin, dev->params.in_mode);
    /* check device response (80 µs low then 80 µs high) */
    start = ztimer_now(ZTIMER_USEC);
    _wait_for_level(dev->params.pin, 0, start);
    if (ztimer_now(ZTIMER_USEC) - start > START_THRESHOLD) {
        DEBUG_PUTS("[dht] error: response low pulse > START_THRESHOLD");
        return -ENODEV;
    }
    _wait_for_level(dev->params.pin, 1, start);
    start = ztimer_now(ZTIMER_USEC);
    _wait_for_level(dev->params.pin, 0, start);
    if (ztimer_now(ZTIMER_USEC) - start < START_THRESHOLD) {
        DEBUG_PUTS("[dht] error: response high pulse < START_THRESHOLD");
        return -ENODEV;
    }
    return 0;
}

static void _bit_parse(struct dht_data *arg)
{
    int8_t pos = arg->bit_pos++;
    if (arg->bit) {
        arg->data[pos / 8] |= (0x80U >> (pos % 8));
    }
}

static void _busy_wait_read(struct dht_data *arg)
{
    uint32_t start = ztimer_now(ZTIMER_USEC);
    while (arg->bit_pos != 40) {
        _wait_for_level(arg->pin, 1, start);
        start = ztimer_now(ZTIMER_USEC);
        _wait_for_level(arg->pin, 0, start);
        arg->bit = (ztimer_now(ZTIMER_USEC) - start > READ_THRESHOLD) ? 1 : 0;
        _bit_parse(arg);
    }
}

static int _validate_checksum(uint8_t *data)
{
    uint8_t sum = 0;

    for (uint_fast8_t i = 0; i < 4; i++) {
        sum += data[i];
    }
    if (sum != data[BYTEPOS_CHECKSUM]) {
        return -EIO;
    }
    return 0;
}

static int _parse_raw_values(dht_t *dev, uint8_t *data)
{
    bool is_negative;

    switch (dev->params.type) {
    case DHT11:
    case DHT11_2022:
        DEBUG_PUTS("[dht] parse raw values with DHT11 data format");
        dev->last_val.humidity = data[BYTEPOS_HUMIDITY_HIGH] * 10
                               + data[BYTEPOS_HUMIDITY_LOW];
        /* MSB for integral temperature byte gives sign, remaining is
         * abs() of value (beware: this is not two's complement!) */
        is_negative = data[BYTEPOS_TEMPERATURE_LOW] & 0x80;
        data[BYTEPOS_TEMPERATURE_LOW] &= ~0x80;
        /* 2022-12 aosong.com data sheet uses interprets low bits as
         * 0.01°C per LSB */
        if (dev->params.type == DHT11_2022) {
            data[BYTEPOS_TEMPERATURE_LOW] /= 10;
        }
        if (data[BYTEPOS_TEMPERATURE_LOW] >= 10) {
            return -ERANGE;
        }
        dev->last_val.temperature = data[BYTEPOS_TEMPERATURE_HIGH] * 10
                                  + data[BYTEPOS_TEMPERATURE_LOW];
        break;
    /* AM2301 == DHT21 == DHT22 (same value in enum),
     * so all are handled here */
    case DHT22:
        DEBUG_PUTS("[dht] parse raw values with DHT22 data format");
        dev->last_val.humidity = (int16_t)(
                                    (data[BYTEPOS_HUMIDITY_HIGH] << 8)
                                    | data[BYTEPOS_HUMIDITY_LOW]);
        is_negative = data[BYTEPOS_TEMPERATURE_HIGH] & 0x80;
        data[BYTEPOS_TEMPERATURE_HIGH] &= ~0x80;
        dev->last_val.temperature = (int16_t)(
                                    (data[BYTEPOS_TEMPERATURE_HIGH] << 8)
                                    | data[BYTEPOS_TEMPERATURE_LOW]);
        break;
    default:
        return -ENOSYS;     /* ENOSYS 38 Function not implemented */
    }

    if (is_negative) {
        dev->last_val.temperature = -dev->last_val.temperature;
    }

    return 0;
}

int dht_init(dht_t *dev, const dht_params_t *params)
{
    int16_t timeout;

    DEBUG_PUTS("[dht] dht_init");

    /* check parameters and configuration */
    assert(dev && params);
    /* AM2301 == DHT21 == DHT22 (same value in enum) */
    assert((params->type == DHT11) || (params->type == DHT11_2022)
        || (params->type == DHT22));

    memset(dev, 0, sizeof(dht_t));
    dev->params = *params;

    /* The 2-second delay mentioned in the datasheet is only required
     * after a power cycle. */
    timeout = POWER_WAIT_TIMEOUT / US_PER_MS;
    gpio_init(dev->params.pin, GPIO_IN);
    while (!gpio_read(dev->params.pin) && timeout--) {
        ztimer_sleep(ZTIMER_USEC, US_PER_MS);
    }
    if (timeout < 0) {
        DEBUG_PUTS("[dht] dht_init: error: Invalid cross-device link");
        return -EXDEV;
    }
    else {
        DEBUG("\n[dht] dht_init: power-up duration: %" PRIi16 " ms\n",
                (int16_t)(POWER_WAIT_TIMEOUT / US_PER_MS - timeout));
    }
    /* The previous test does not ensure the sensor presence in case an
     * external pull-up resistor is used. */
    while (_send_start_signal(dev) == -ENODEV
        && (timeout -= START_LOW_TIME / US_PER_MS) > 0) {}
    if (timeout < 0) {
        DEBUG_PUTS("[dht] dht_init: error: No such device");
        return -ENODEV;
    }
    else {
        DEBUG("\n[dht] dht_init: presence check duration: %" PRIi16 " ms\n",
                (int16_t)(POWER_WAIT_TIMEOUT / US_PER_MS - timeout));
    }

    DEBUG_PUTS("[dht] dht_init: success");
    return 0;
}

int dht_read(dht_t *dev, int16_t *temp, int16_t *hum)
{
    int ret;

    assert(dev);

    struct dht_data data = {
        .pin = dev->params.pin,
        .in_mode = dev->params.in_mode,
        .bit_pos = 0,
    };

    if (_send_start_signal(dev) == -ENODEV) {
        DEBUG_PUTS("[dht] error: No response from device");
        return -ENODEV;
    }

    /* read the data */
    _busy_wait_read(&data);

    if (_validate_checksum(data.data) == -EIO) {
        DEBUG("[dht] error: checksum doesn't match\n"
              "[dht] RAW data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
              (unsigned)data.data[0], (unsigned)data.data[1],
              (unsigned)data.data[2], (unsigned)data.data[3],
              (unsigned)data.data[4]);
        return -EIO;
    }

    if ((ret = _parse_raw_values(dev, data.data)) < 0) {
        if (ret == -ENOSYS) {
            DEBUG_PUTS("[dht] error: data format not implemented");
        }
        else if (ret == -ERANGE) {
            DEBUG_PUTS("[dht] error: invalid temperature low byte");
        }
        return ret;
    }

    if (hum != NULL) {
        *hum = dev->last_val.humidity;
    }
    if (temp != NULL) {
        *temp = dev->last_val.temperature;
    }

    return 0;
}
