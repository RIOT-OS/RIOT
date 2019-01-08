/*
 * Copyright 2018 Otto-von-Guericke-Universität Magdeburg
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
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "ltc4150.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void update_data(ltc4150_dev_t *dev)
{
    uint32_t now = xtimer_now_usec64() / US_PER_SEC;

    /* For every ten seconds passed since last update: */
    while (dev->last_update_sec + 10 < now) {
        /* Update charged transferred last minute */
#ifdef MODULE_LTC4150_BIDIRECTIONAL
        dev->charged.last_min += dev->charged.counter;
        dev->charged.last_min -= dev->charged.ringbuf[dev->position];
        dev->charged.ringbuf[dev->position] = dev->charged.counter;
        dev->charged.counter = 0;
#endif

        dev->discharged.last_min += dev->discharged.counter;
        dev->discharged.last_min -= dev->discharged.ringbuf[dev->position];
        dev->discharged.ringbuf[dev->position] = dev->discharged.counter;
        dev->discharged.counter = 0;

        dev->position++;

        if (dev->position >= 6) {
            dev->position = 0;
        }

        dev->last_update_sec += 10;
    }
}

static void pulse_cb(void *_dev)
{
    ltc4150_dev_t *dev = _dev;

    update_data(dev);
#ifdef MODULE_LTC4150_BIDIRECTIONAL
    if (
        (dev->params.polarity == GPIO_UNDEF) ||
        (!gpio_read(dev->params.polarity))
        ) {
        dev->discharged.counter++;
        dev->discharged.total++;
    }
    else {
        dev->charged.counter++;
        dev->charged.total++;
    }
#else
    dev->discharged.counter++;
    dev->discharged.total++;
#endif
}

int ltc4150_init(ltc4150_dev_t *dev, const ltc4150_params_t *params)
{
    if (!dev || !params) {
        return -EINVAL;
    }

    memset(dev, 0x00, sizeof(ltc4150_dev_t));
    dev->params = *params;

    if (dev->params.shutdown != GPIO_UNDEF) {
        /* Activate LTC4150 */
        if (gpio_init(dev->params.shutdown, GPIO_OUT)) {
            DEBUG("[ltc4150] Failed to initialize shutdown pin");
            return -EIO;
        }
        gpio_set(dev->params.shutdown);
    }

#ifdef MODULE_LTC4150_BIDIRECTIONAL
    if (dev->params.polarity != GPIO_UNDEF) {
        gpio_mode_t mode = (dev->params.flags & LTC4150_POL_EXT_PULL_UP) ?
                           GPIO_IN : GPIO_IN_PU;
        if (gpio_init(dev->params.polarity, mode)) {
            DEBUG("[ltc4150] Failed to initialize polarity pin");
            return -EIO;
        }
    }
#endif

    gpio_mode_t mode = (dev->params.flags & LTC4150_INT_EXT_PULL_UP) ?
                       GPIO_IN : GPIO_IN_PU;
    if (gpio_init_int(dev->params.interrupt, mode, GPIO_FALLING,
                      pulse_cb, dev)
        ) {
        DEBUG("[ltc4150] Failed to initialize interrupt pin");
        return -EIO;
    }

    dev->last_update_sec = dev->start_sec = xtimer_now_usec64() / US_PER_SEC;

    DEBUG("[ltc4150] Initialized successfully");
    return 0;
}

int ltc4150_shutdown(ltc4150_dev_t *dev)
{
    if (!dev) {
        return -EINVAL;
    }

    gpio_irq_disable(dev->params.interrupt);

    if (dev->params.shutdown != GPIO_UNDEF) {
        gpio_clear(dev->params.shutdown);
    }

    return 0;
}

/**
 * @brief Convert the raw data (# pulses) acquired by the LTC4150 device to
 *        charge information in millicoulomb
 *
 * @param dev                 LTC4150 device the data was received from
 * @param[out] charged        Charge in charging direction is stored here
 * @param[out] discharged     Charge in discharging direction is stored here
 * @param[in] raw_charged     Number of pulses in charging direction
 * @param[in] raw_discharged  Number of pulses in discharging direction
 */
static void get_coulomb(const ltc4150_dev_t *dev,
                        int32_t *charged, int32_t *discharged,
#ifdef MODULE_LTC4150_BIDIRECTIONAL
                        uint32_t raw_charged,
#endif
                        uint32_t raw_discharged)
{
    if (charged != NULL) {
#ifdef MODULE_LTC4150_BIDIRECTIONAL
        *charged = raw_charged;
        *charged *= 3600000;
        *charged += dev->params.pulses_per_ah >> 1;
        *charged /= dev->params.pulses_per_ah;
#else
        *charged = 0;
#endif
    }

    if (discharged != NULL) {
        *discharged = raw_discharged;
        *discharged *= 3600000;
        *discharged += dev->params.pulses_per_ah >> 1;
        *discharged /= dev->params.pulses_per_ah;
    }
}

int ltc4150_total_charge(ltc4150_dev_t *dev,
                         int32_t *charged, int32_t *discharged)
{
    if (!dev) {
        return -EINVAL;
    }

    gpio_irq_disable(dev->params.interrupt);
    update_data(dev);
#ifdef MODULE_LTC4150_BIDIRECTIONAL
    get_coulomb(dev, charged, discharged,
                dev->charged.total, dev->discharged.total);
#else
    get_coulomb(dev, charged, discharged, dev->discharged.total);
#endif
    gpio_irq_enable(dev->params.interrupt);
    return 0;
}

int ltc4150_last_minute_charge(ltc4150_dev_t *dev, int32_t *charged,
                               int32_t *discharged)
{
    if (!dev) {
        return -EINVAL;
    }

    gpio_irq_disable(dev->params.interrupt);
    update_data(dev);
#ifdef MODULE_LTC4150_BIDIRECTIONAL
    get_coulomb(dev, charged, discharged,
                dev->charged.last_min, dev->discharged.last_min);
#else
    get_coulomb(dev, charged, discharged, dev->discharged.last_min);
#endif
    gpio_irq_enable(dev->params.interrupt);
    return 0;
}

int ltc4150_avg_current(ltc4150_dev_t *dev, int16_t *dest)
{
    uint32_t now;
    int32_t duration, charged, discharged;;
    int retval;

    retval = ltc4150_total_charge(dev, &charged, &discharged);
    if (retval) {
        return retval;
    }

    now = xtimer_now_usec64() / US_PER_SEC;
    duration = now - dev->start_sec;
    if (!duration) {
        /* Called before at least one second of data acquired. Prevent division
         * by zero by returning -EAGAIN.
         */
        return -EAGAIN;
    }

    /* From millicoloumb (=mAs) to E-01 mA */
    *dest = ((discharged - charged) * 10) / duration;

    return 0;
}
