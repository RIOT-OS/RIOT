/*
 * Copyright 2015 Ludwig Ortmann
 * Copyright 2015 Christian Mehlis
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_dht
 * @{
 *
 * @file
 * @brief       Device driver implementation for the DHT 11 and 22
 *              temperature and humidity sensor
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "hwtimer.h"
#include "timex.h"
#include "periph/gpio.h"

#include "dht.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/***********************************************************************
 * internal API declaration
 **********************************************************************/

static void dht_read_data(gpio_t dev, uint32_t *data, uint8_t *checksum);
static int dht_test_checksum(uint32_t data, uint8_t checksum);
void dht_parse_11(dht_data_t *data, float *outhum, float *outtemp);
void dht_parse_22(dht_data_t *data, float *outhum, float *outtemp);

/***********************************************************************
 * internal API implementation
 **********************************************************************/

void dht_parse_11(dht_data_t *data, float *outhum, float *outtemp)
{
    *outhum = data->humidity >> 8;
    *outtemp = data->temperature >> 8;
}

void dht_parse_22(dht_data_t *data, float *outhum, float *outtemp)
{
    *outhum = data->humidity / 10;

    /* the highest bit indicates a negative value */
    if (data->temperature & 0x8000) {
        *outtemp = (data->temperature & 0x7FFF) / -10;
    }
    else {
        *outtemp = data->temperature / 10;
    }
}

static int dht_test_checksum(uint32_t data, uint8_t checksum)
{
    uint8_t sum;
    sum  = (data >>  0) & 0x000000FF;
    sum += (data >>  8) & 0x000000FF;
    sum += (data >> 16) & 0x000000FF;
    sum += (data >> 24) & 0x000000FF;

    return ((checksum == sum) && (checksum != 0));
}

static void dht_read_data(gpio_t dev, uint32_t *data, uint8_t *checksum)
{
    /* send init signal to device */
    gpio_clear(dev);
    hwtimer_wait(HWTIMER_TICKS(20 * MS_IN_USEC));
    gpio_set(dev);
    hwtimer_wait(HWTIMER_TICKS(40));

    /* sync on device */
    gpio_init(dev, GPIO_DIR_IN, GPIO_PULLUP);
    while (!gpio_read(dev)) ;
    while (gpio_read(dev)) ;

    /*
     * data is read in sequentially, highest bit first:
     *  40 .. 24  23   ..   8  7  ..  0
     * [humidity][temperature][checksum]
     */

    /* read all the bits */
    uint64_t les_bits = 0x00000000000000;
    for (int c = 0; c < 40; c++) {
        les_bits <<= 1; /* this is a nop in the first iteration, but
                           we must not shift the last bit */
        /* wait for start of bit */
        while (!gpio_read(dev)) ;
        unsigned long start = hwtimer_now();
        /* wait for end of bit */
        while (gpio_read(dev)) ;
        /* calculate bit length (long 1, short 0) */
        unsigned long stop = hwtimer_now();
        /* compensate for overflow if needed */
        if (stop < start) {
            stop = HWTIMER_MAXTICKS - stop;
            start = 0;
        }
        if ((stop - start) > 40) {
            /* read 1, set bit */
            les_bits |= 0x0000000000000001;
        }
        else {
            /* read 0, don't set bit */
        }
    }

    *checksum = les_bits & 0x00000000000000FF;
    *data = (les_bits >> 8) & 0x00000000FFFFFFFF;

    gpio_init(dev, GPIO_DIR_OUT, GPIO_PULLUP);
    gpio_set(dev);
}

/***********************************************************************
 * public API implementation
 **********************************************************************/

int dht_init(dht_t *dev, dht_type_t type, gpio_t gpio)
{
    DEBUG("dht_init\n");

    dev->gpio = gpio;
    dev->type = type;

    if (gpio_init(gpio, GPIO_DIR_OUT, GPIO_PULLUP) == -1) {
        return -1;
    }
    gpio_set(gpio);

    hwtimer_wait(HWTIMER_TICKS(2000 * MS_IN_USEC));

    DEBUG("dht_init: success\n");
    return 0;
}


int dht_read_raw(dht_t *dev, dht_data_t *outdata)
{
    uint32_t data;
    uint8_t checksum;

    /* read raw data */
    dht_read_data(dev->gpio, &data, &checksum);

    /* check checksum */
    int ret = dht_test_checksum(data, checksum);
    if (!ret) {
        DEBUG("checksum fail\n");
    }

    outdata->humidity = data >> 16;
    outdata->temperature = data & 0x0000FFFF;

    return (ret - 1); /* take that logic! */
}

void dht_parse(dht_t *dev, dht_data_t *data, float *outrelhum, float *outtemp)
{
    switch (dev->type) {
        case (DHT11):
            dht_parse_11(data, outrelhum, outtemp);
            break;

        case DHT22:
            dht_parse_22(data, outrelhum, outtemp);
            break;

        default:
            DEBUG("unknown DHT type\n");
    }
}

int dht_read(dht_t *dev, float *outrelhum, float *outtemp)
{
    /* read data, fail on error */
    dht_data_t data;
    if (dht_read_raw(dev, &data) == -1) {
        return -1;
    }

    dht_parse(dev, &data, outrelhum, outtemp);
    return 0;
}
