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

#include <stdint.h>
#include <string.h>

#include "log.h"
#include "assert.h"
#include "xtimer.h"
#include "timex.h"
#include "periph/gpio.h"

#include "dht.h"
#include "dht_params.h"

#define ENABLE_DEBUG            0
#include "debug.h"

/* Every pulse send by the DHT longer than 40µs is interpreted as 1 */
#define PULSE_WIDTH_THRESHOLD   (40U)
/* If an expected pulse is not detected within 1000µs, something is wrong */
#define TIMEOUT                 (1000U)
/* The DHT sensor cannot measure more than once a second */
#define DATA_HOLD_TIME          (US_PER_SEC)
/* The start signal by pulling data low for at least 18ms and then up for
 * 20-40µs*/
#define START_LOW_TIME          (20U * US_PER_MS)
#define START_HIGH_TIME         (40U)

static inline void _reset(dht_t *dev)
{
    gpio_init(dev->params.pin, GPIO_OUT);
    gpio_set(dev->params.pin);
}

/**
 * @brief   Wait until the pin @p pin has level @p expect
 *
 * @param   pin     GPIO pin to wait for
 * @param   expect  Wait until @p pin has this logic level
 * @param   timeout Timeout in µs
 *
 * @retval  0       Success
 * @retval  -1      Timeout occurred before level was reached
 */
static inline int _wait_for_level(gpio_t pin, bool expect, unsigned timeout)
{
    while (((gpio_read(pin) > 0) != expect) && timeout) {
        xtimer_usleep(1);
        timeout--;
    }

    return (timeout > 0) ? 0 : -1;
}

static int _read(uint8_t *dest, gpio_t pin)
{
    DEBUG("[dht] read\n");
    uint16_t res = 0;

    for (int i = 0; i < 8; i++) {
        uint32_t start, end;
        res <<= 1;
        /* measure the length between the next rising and falling flanks (the
         * time the pin is high - smoke up :-) */
        if (_wait_for_level(pin, 1, TIMEOUT)) {
            return -1;
        }
        start = xtimer_now_usec();

        if (_wait_for_level(pin, 0, TIMEOUT)) {
            return -1;
        }
        end = xtimer_now_usec();

        /* if the high phase was more than 40us, we got a 1 */
        if ((end - start) > PULSE_WIDTH_THRESHOLD) {
            res |= 0x0001;
        }
    }

    *dest = res;
    return 0;
}

int dht_init(dht_t *dev, const dht_params_t *params)
{
    DEBUG("[dht] dht_init\n");

    /* check parameters and configuration */
    assert(dev && params);

    memset(dev, 0, sizeof(dht_t));
    dev->params = *params;

    _reset(dev);

    xtimer_msleep(2000);

    DEBUG("[dht] dht_init: success\n");
    return DHT_OK;
}

int dht_read(dht_t *dev, int16_t *temp, int16_t *hum)
{
    uint8_t csum;
    uint8_t raw_temp_i, raw_temp_d, raw_hum_i, raw_hum_d;

    assert(dev);

    uint32_t now_us = xtimer_now_usec();
    if ((now_us - dev->last_read_us) > DATA_HOLD_TIME) {
        /* send init signal to device */
        gpio_clear(dev->params.pin);
        xtimer_usleep(START_LOW_TIME);
        gpio_set(dev->params.pin);
        xtimer_usleep(START_HIGH_TIME);

        /* sync on device */
        gpio_init(dev->params.pin, dev->params.in_mode);
        if (_wait_for_level(dev->params.pin, 1, TIMEOUT)) {
            _reset(dev);
            return DHT_TIMEOUT;
        }

        if (_wait_for_level(dev->params.pin, 0, TIMEOUT)) {
            _reset(dev);
            return DHT_TIMEOUT;
        }

        /*
         * data is read in sequentially, highest bit first:
         *  40 .. 24  23   ..   8  7  ..  0
         * [humidity][temperature][checksum]
         */

        /* read the humidity, temperature, and checksum bits */
        if (_read(&raw_hum_i, dev->params.pin)) {
            _reset(dev);
            return DHT_TIMEOUT;
        }
        if (_read(&raw_hum_d, dev->params.pin)) {
            _reset(dev);
            return DHT_TIMEOUT;
        }
        if (_read(&raw_temp_i, dev->params.pin)) {
            _reset(dev);
            return DHT_TIMEOUT;
        }
        if (_read(&raw_temp_d, dev->params.pin)) {
            _reset(dev);
            return DHT_TIMEOUT;
        }

        if (_read(&csum, dev->params.pin)) {
            _reset(dev);
            return DHT_TIMEOUT;
        }

        /* Bring device back to defined state - so we can trigger the next reading
         * by pulling the data pin low again */
        _reset(dev);

        /* validate the checksum */
        uint8_t sum = (raw_temp_i) + (raw_temp_d) + (raw_hum_i) + (raw_hum_d);
        if (sum != csum) {
            DEBUG("[dht] error: checksum doesn't match\n");
            return DHT_NOCSUM;
        }

        /* parse the RAW values */
        DEBUG("[dht] RAW values: temp: %2i.%i hum: %2i.%i\n", (int)raw_temp_i,
            (int)raw_temp_d, (int)raw_hum_i, (int)raw_hum_d);

        dev->last_val.humidity = raw_hum_i * 10 + raw_hum_d;
        /* MSB set means negative temperature on DHT22. Will always be 0 on DHT11 */
        if (raw_temp_i & 0x80) {
            dev->last_val.temperature = -((raw_temp_i & ~0x80) * 10 + raw_temp_d);
        }
        else {
            dev->last_val.temperature = raw_temp_i * 10 + raw_temp_d;
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

    return DHT_OK;
}
