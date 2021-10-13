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
 * @brief       LTC4150 Device Driver
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "ltc4150.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void pulse_cb(void *_dev)
{
    uint64_t now;
    ltc4150_dir_t dir;
    ltc4150_dev_t *dev = _dev;

    if ((!gpio_is_valid(dev->params.polarity)) ||
        (!gpio_read(dev->params.polarity))
        ) {
        dev->discharged++;
        dir = LTC4150_DISCHARGE;
    }
    else {
        dev->charged++;
        dir = LTC4150_CHARGE;
    }

    now = xtimer_now_usec64();

    if (dev->params.recorders) {
        assert(dev->params.recorder_data);
        for (unsigned i = 0; dev->params.recorders[i] != NULL; i++) {
            dev->params.recorders[i]->pulse(dev, dir, now,
                                            dev->params.recorder_data[i]);
        }
    }

    dev->last_update_sec = now / US_PER_SEC;
}

int ltc4150_init(ltc4150_dev_t *dev, const ltc4150_params_t *params)
{
    if (!dev || !params) {
        return -EINVAL;
    }

    memset(dev, 0, sizeof(ltc4150_dev_t));
    dev->params = *params;

    if (gpio_is_valid(dev->params.shutdown)) {
        /* Activate LTC4150 */
        if (gpio_init(dev->params.shutdown, GPIO_OUT)) {
            DEBUG("[ltc4150] Failed to initialize shutdown pin");
            return -EIO;
        }
        gpio_set(dev->params.shutdown);
    }

    if (gpio_is_valid(dev->params.polarity)) {
        gpio_mode_t mode = (dev->params.flags & LTC4150_POL_EXT_PULL_UP) ?
                           GPIO_IN : GPIO_IN_PU;
        if (gpio_init(dev->params.polarity, mode)) {
            DEBUG("[ltc4150] Failed to initialize polarity pin");
            return -EIO;
        }
    }

    gpio_mode_t mode = (dev->params.flags & LTC4150_INT_EXT_PULL_UP) ?
                       GPIO_IN : GPIO_IN_PU;
    if (gpio_init_int(dev->params.interrupt, mode, GPIO_FALLING,
                      pulse_cb, dev)
        ) {
        DEBUG("[ltc4150] Failed to initialize interrupt pin");
        return -EIO;
    }

    ltc4150_reset_counters(dev);

    DEBUG("[ltc4150] Initialized successfully");
    return 0;
}

int ltc4150_reset_counters(ltc4150_dev_t *dev)
{
    uint64_t now = xtimer_now_usec64();

    if (!dev) {
        return -EINVAL;
    }

    gpio_irq_disable(dev->params.interrupt);

    dev->charged = 0;
    dev->discharged = 0;
    dev->last_update_sec = dev->start_sec = now / US_PER_SEC;

    if (dev->params.recorders) {
        assert(dev->params.recorder_data);
        for (unsigned i = 0; dev->params.recorders[i] != NULL; i++) {
            dev->params.recorders[i]->reset(dev, now, dev->params.recorder_data[i]);
        }
    }

    gpio_irq_enable(dev->params.interrupt);
    return 0;
}

int ltc4150_shutdown(ltc4150_dev_t *dev)
{
    if (!dev) {
        return -EINVAL;
    }

    gpio_irq_disable(dev->params.interrupt);

    if (gpio_is_valid(dev->params.shutdown)) {
        gpio_clear(dev->params.shutdown);
    }

    return 0;
}

void ltc4150_pulses2c(const ltc4150_dev_t *dev,
                      uint32_t *charged, uint32_t *discharged,
                      uint32_t raw_charged,
                      uint32_t raw_discharged)
{
    uint64_t tmp;

    if (charged) {
        tmp = raw_charged;
        tmp *= 3600000;
        tmp += dev->params.pulses_per_ah >> 1;
        tmp /= dev->params.pulses_per_ah;
        *charged = tmp;
    }

    if (discharged) {
        tmp = raw_discharged;
        tmp *= 3600000;
        tmp += dev->params.pulses_per_ah >> 1;
        tmp /= dev->params.pulses_per_ah;
        *discharged = tmp;
    }
}

int ltc4150_charge(ltc4150_dev_t *dev, uint32_t *charged, uint32_t *discharged)
{
    if (!dev) {
        return -EINVAL;
    }

    gpio_irq_disable(dev->params.interrupt);
    ltc4150_pulses2c(dev, charged, discharged, dev->charged, dev->discharged);
    gpio_irq_enable(dev->params.interrupt);
    return 0;
}

int ltc4150_avg_current(ltc4150_dev_t *dev, int16_t *dest)
{
    int32_t duration, charged, discharged;;
    int retval;

    retval = ltc4150_charge(dev, (uint32_t *)&charged, (uint32_t *)&discharged);
    if (retval) {
        return retval;
    }

    duration = dev->last_update_sec - dev->start_sec;
    if (!duration) {
        /* Called before one second of date or one pulse acquired. Prevent
         * division by zero by returning -EAGAIN.
         */
        return -EAGAIN;
    }

    /* From millicoloumb (=mAs) to E-01 mA */
    *dest = ((discharged - charged) * 10) / duration;

    return 0;
}
