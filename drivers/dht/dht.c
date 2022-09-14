/*
 * Copyright 2015 Ludwig Knüpfer
 *           2015 Christian Mehlis
 *           2016-2017 Freie Universität Berlin
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

#define ENABLE_DEBUG            1
#include "debug.h"

/* Every pulse send by the DHT longer than 40µs is interpreted as 1 */
#define PULSE_WIDTH_THRESHOLD   (40U)
/* The communication should take at most 41 * (70 us + 50 us) = 4,920 us
 * theoretically. If it didn't complete after 10,000 us, we can be sure it
 * failed */
#define TIMEOUT_US              (10000U)
/* The DHT sensor cannot measure more than once a second */
#define DATA_HOLD_TIME          (US_PER_SEC)
/* The start signal by pulling data low for at least 18ms and then up for
 * 20-40µs*/
#define START_LOW_TIME          (20U * US_PER_MS)
#define START_HIGH_TIME         (40U)

#define INIT_DELAY_SEC          (2U)

enum {
    BYTEPOS_HUMIDITY_INTEGRAL = 0,
    BYTEPOS_HUMIDITY_FRACTUAL = 1,
    BYTEPOS_TEMPERATURE_INTEGRAL = 2,
    BYTEPOS_TEMPERATURE_FRACTUAL = 3,
    BYTEPOS_CHECKSUM = 4,
};

static inline void _reset(dht_t *dev)
{
    gpio_init(dev->params.pin, GPIO_OUT);
    gpio_set(dev->params.pin);
}

struct dht_irq_data {
    mutex_t sync;
    uint32_t time;
    gpio_t pin;
    uint8_t data[5];
    int8_t bit_pos;
};

int dht_init(dht_t *dev, const dht_params_t *params)
{
    DEBUG("[dht] dht_init\n");

    /* check parameters and configuration */
    assert(dev && params);

    memset(dev, 0, sizeof(dht_t));
    dev->params = *params;

    _reset(dev);

    if (IS_USED(MODULE_ZTIMER_SEC)) {
        ztimer_sleep(ZTIMER_SEC, INIT_DELAY_SEC);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer_sleep(ZTIMER_MSEC, INIT_DELAY_SEC * MS_PER_SEC);
    }
    else {
        ztimer_sleep(ZTIMER_USEC, INIT_DELAY_SEC * US_PER_SEC);
    }

    DEBUG("[dht] dht_init: success\n");
    return 0;
}

static void gpio_cb(void *_arg)
{
    struct dht_irq_data *arg = _arg;
    uint32_t now = ztimer_now(ZTIMER_USEC);

    if (arg->bit_pos >= 40) {
        /* stray IRQ, ignoring */
        return;
    }

    if (gpio_read(arg->pin)) {
        arg->time = now;
        return;
    }

    int8_t pos = arg->bit_pos++;
    if (pos == -1) {
        /* start signal received, ignore */
        return;
    }

    unsigned bit = (now - arg->time) > PULSE_WIDTH_THRESHOLD;

    if (bit) {
        arg->data[pos >> 3] |= (0x80U >> (pos & 7));
    }

    if (pos == 39) {
        /* last bit received, signal that we are done now */
        mutex_unlock(&arg->sync);
    }
};

static void timeout_cb(void *_arg)
{
    struct dht_irq_data *arg = _arg;

    /* something went wrong, unblocking waiting thread */
    mutex_unlock(&arg->sync);
};

int dht_read(dht_t *dev, int16_t *temp, int16_t *hum)
{
    assert(dev);

    uint32_t now_us = ztimer_now(ZTIMER_USEC);
    if ((now_us - dev->last_read_us) > DATA_HOLD_TIME) {
        /* send init signal to device */
        gpio_init(dev->params.pin, GPIO_OUT);
        gpio_clear(dev->params.pin);
        DEBUG("[dht] pre-sleep: %" PRIu32 "\n", ztimer_now(ZTIMER_USEC));
        ztimer_sleep(ZTIMER_USEC, START_LOW_TIME);
        gpio_set(dev->params.pin);
        DEBUG("[dht] post-sleep: %" PRIu32 "\n", ztimer_now(ZTIMER_USEC));
        ztimer_sleep(ZTIMER_USEC, START_HIGH_TIME);

        struct dht_irq_data data = {
            .sync = MUTEX_INIT_LOCKED,
            .pin = dev->params.pin,
            /* first bit is start signal, only afterwards actual data is
             * received */
            .bit_pos = -1,
        };
        ztimer_t timeout = {
            .callback = timeout_cb,
            .arg = &data,
        };

        ztimer_set(ZTIMER_USEC, &timeout, TIMEOUT_US);

        /* actual reception is done in IRQ */
        if (gpio_init_int(dev->params.pin, dev->params.in_mode, GPIO_BOTH,
                          gpio_cb, &data)) {
            ztimer_remove(ZTIMER_USEC, &timeout);
            /* IRQs not supported on given GPIO pin or with given mode */
            return -ENOTSUP;
        }

        /* wait for IRQ handler to collect all bits */
        mutex_lock(&data.sync);
        /* remove timer (doesn't hurt if it fired by now in case of timeout) */
        ztimer_remove(ZTIMER_USEC, &timeout);
        gpio_irq_disable(dev->params.pin);

        /* Bring device back to defined state - so we can trigger the next reading
         * by pulling the data pin low again */
        _reset(dev);

        if (data.bit_pos == -1) {
            DEBUG_PUTS("[dht] Not a single bit received, no DHT connected?");
            return -ENXIO;
        }

        if (data.bit_pos != 40) {
            DEBUG_PUTS("[dht] Timeout before all bits received");
            return -ETIMEDOUT;
        }

        DEBUG("[dht] RAW data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
              (unsigned)data.data[0], (unsigned)data.data[1],
              (unsigned)data.data[2], (unsigned)data.data[3],
              (unsigned)data.data[4]);

        /* validate the checksum */
        uint8_t sum = 0;
        for (uint_fast8_t i = 0; i < 4; i++) {
            sum += data.data[i];
        }

        if (sum != data.data[BYTEPOS_CHECKSUM]) {
            DEBUG("[dht] error: checksum doesn't match\n");
            return -EIO;
        }

        dev->last_val.humidity = data.data[BYTEPOS_HUMIDITY_INTEGRAL] * 10
                               + data.data[BYTEPOS_HUMIDITY_FRACTUAL];
        /* MSB for integral temperature byte gives sign, remaining is abs() of
         * value (beware: this is not two's complement!) */
        bool is_negative = data.data[BYTEPOS_TEMPERATURE_INTEGRAL] & 0x80;
        data.data[BYTEPOS_TEMPERATURE_INTEGRAL] &= 0x7f;
        dev->last_val.temperature = data.data[BYTEPOS_TEMPERATURE_INTEGRAL] * 10
                                  + data.data[BYTEPOS_TEMPERATURE_FRACTUAL];

        if (is_negative) {
            dev->last_val.temperature = -dev->last_val.temperature;
        }

        /* update time of last measurement */
        dev->last_read_us = now_us;
    }

    if (temp) {
        *temp = dev->last_val.temperature;
    }

    if (hum) {
        *hum = dev->last_val.humidity;
    }

    return 0;
}
