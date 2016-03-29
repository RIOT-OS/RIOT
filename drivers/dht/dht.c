/*
 * Copyright 2015 Ludwig Knüpfer
 *           2015 Christian Mehlis
 *           2016 Freie Universität Berlin
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
 * @brief       Device driver implementation for the DHT 11 and 22
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
#include "xtimer.h"
#include "timex.h"
#include "periph/gpio.h"

#include "dht.h"
#include "dht_params.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define PULSE_WIDTH_THRESHOLD       (40U)

/**
 * @brief   Allocation of memory for device descriptors
 */
dht_t dht_devs[DHT_NUMOF];

static uint16_t read(gpio_t pin, int bits)
{
    uint32_t start, end;
    uint16_t res = 0;

    for (int i = 0; i < bits; i++) {
        res <<= 1;
        /* measure the length between the next rising and falling flanks (the
         * time the pin is high - smoke up :-) */
        while (!gpio_read(pin));
        start = xtimer_now();
        while (gpio_read(pin));
        end = xtimer_now();
        /* if the high phase was more than 40us, we got a 1 */
        if ((end - start) > PULSE_WIDTH_THRESHOLD) {
            res |= 0x0001;
        }
    }
    return res;
}

void dht_auto_init(void)
{
    for (unsigned i = 0; i < DHT_NUMOF; i++) {
        if (dht_init(&dht_devs[i], &dht_params[i]) < 0) {
            LOG_ERROR("Unable to initialize DHT sensor #%i\n", i);
        }
#ifdef MODULE_SAUL_REG
        for (int j = 0; j < 2; j++) {
            dht_saul_reg[i][j].dev = &dht_devs[i];
            saul_reg_add(&dht_saul_reg[i][j]);
        }
#endif
    }
}

int dht_init(dht_t *dev, const dht_params_t *params)
{
    DEBUG("dht_init\n");

    memcpy(dev, params, sizeof(dht_t));

    if (gpio_init(dev->pin, GPIO_OUT) == -1) {
        return -1;
    }
    gpio_set(dev->pin);

    xtimer_usleep(2000 * MS_IN_USEC);

    DEBUG("dht_init: success\n");
    return 0;
}

int dht_read(dht_t *dev, int16_t *temp, int16_t *hum)
{
    uint8_t csum, sum;
    uint16_t raw_hum, raw_temp;

    /* send init signal to device */
    gpio_clear(dev->pin);
    xtimer_usleep(20 * MS_IN_USEC);
    gpio_set(dev->pin);
    xtimer_usleep(40);

    /* sync on device */
    gpio_init(dev->pin, dev->in_mode);
    while (!gpio_read(dev->pin)) ;
    while (gpio_read(dev->pin)) ;

    /*
     * data is read in sequentially, highest bit first:
     *  40 .. 24  23   ..   8  7  ..  0
     * [humidity][temperature][checksum]
     */

    /* read the humidity, temperature, and checksum bits */
    raw_hum = read(dev->pin, 16);
    raw_temp = read(dev->pin, 16);
    csum = (uint8_t)read(dev->pin, 8);

    /* set pin high again - so we can trigger the next reading by pulling it low
     * again */
    gpio_init(dev->pin, GPIO_OUT);
    gpio_set(dev->pin);

    /* validate the checksum */
    sum = (raw_temp >> 8) + (raw_temp & 0xff) + (raw_hum >> 8) + (raw_hum & 0xff);
    if ((sum != csum) || (csum == 0)) {
        DEBUG("error: checksum invalid\n");
        return -1;
    }

    /* parse the RAW values */
    DEBUG("RAW values: temp: %7i hum: %7i\n", (int)raw_temp, (int)raw_hum);
    switch (dev->type) {
        case DHT11:
            *temp = (int16_t)((raw_temp >> 8) * 10);
            *hum = (int16_t)((raw_hum >> 8) * 10);
            break;
        case DHT22:
            *hum = (int16_t)raw_hum;
            /* if the high-bit is set, the value is negative */
            if (raw_temp & 0x8000) {
                *temp = (int16_t)((raw_temp & ~0x8000) * -1);
            }
            else {
                *temp = (int16_t)raw_temp;
            }
        default:
            return -2;
    }

    return 0;
}
