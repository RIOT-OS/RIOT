/*
 * Copyright 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ltc4150
 * @{
 *
 * @file
 * @brief       Track the drawn charged of the last minute
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include <string.h>

#include "ltc4150.h"
#include "xtimer.h"

static void init_or_reset(ltc4150_dev_t *dev, uint64_t now_usec, void *arg);
static void pulse(ltc4150_dev_t *dev, ltc4150_dir_t dir, uint64_t now_usec,
                  void *arg);

const ltc4150_recorder_t ltc4150_last_minute = {
    .reset = init_or_reset,
    .pulse = pulse,
};

static void init_or_reset(ltc4150_dev_t *dev, uint64_t now_usec, void *arg)
{
    (void)dev;
    ltc4150_last_minute_data_t *data = arg;

    memset(data, 0, sizeof(ltc4150_last_minute_data_t));
    data->last_rotate_sec = now_usec / US_PER_SEC;
}

static void update_ringbuffer(ltc4150_last_minute_data_t *data,
                              uint64_t now_usec)
{
    uint32_t now_sec = (now_usec / US_PER_SEC);

    /* Note: This expression should be correct even when time overflows */
    while (now_sec - data->last_rotate_sec > 10) {
        data->last_rotate_sec += 10;
        data->charged += data->buf_charged[data->ring_pos];
        data->discharged += data->buf_discharged[data->ring_pos];
        if (++data->ring_pos >= ARRAY_SIZE(data->buf_charged)) {
            data->ring_pos = 0;
        }
        data->charged -= data->buf_charged[data->ring_pos];
        data->discharged -= data->buf_discharged[data->ring_pos];
        data->buf_charged[data->ring_pos] = 0;
        data->buf_discharged[data->ring_pos] = 0;
    }
}

static void pulse(ltc4150_dev_t *dev, ltc4150_dir_t dir, uint64_t now_usec,
                  void *arg)
{
    (void)dev;
    ltc4150_last_minute_data_t *data = arg;
    update_ringbuffer(data, now_usec);

    switch (dir) {
        case LTC4150_CHARGE:
            data->buf_charged[data->ring_pos]++;
            break;
        default:
        case LTC4150_DISCHARGE:
            data->buf_discharged[data->ring_pos]++;
            break;
    }
}

int ltc4150_last_minute_charge(ltc4150_dev_t *dev,
                               ltc4150_last_minute_data_t *d,
                               uint32_t *charged, uint32_t *discharged)
{
    if (!dev || !d) {
        return -EINVAL;
    }

    gpio_irq_disable(dev->params.interrupt);
    update_ringbuffer(d, xtimer_now_usec64());
    ltc4150_pulses2c(dev, charged, discharged, d->charged, d->discharged);
    gpio_irq_enable(dev->params.interrupt);

    return 0;
}
