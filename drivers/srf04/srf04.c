/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_srf04
 * @{
 *
 * @file
 * @brief       driver for srf04 ultra sonic range finder
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 *
 * @}
 */

#include "srf04.h"
#include "srf04_params.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define SRF04_DISTANCE      (584U)
#define SRF04_SAMPLE_PERIOD (50U * US_PER_MS)

static void _cb(void *arg)
{
    uint32_t t = xtimer_now_usec();

    srf04_t* dev = (srf04_t*)arg;
    if (dev->distance > SRF04_ERR_MEASURING) {
        dev->distance = SRF04_ERR_MEASURING;
        dev->time = t;
    } else {
        gpio_irq_disable(dev->p.echo);
        dev->distance = (t - dev->time);
    }
}

int srf04_init(srf04_t* dev, const srf04_params_t *params)
{
    dev->p = *params;

    dev->distance = SRF04_ERR_INVALID;
    dev->time = 0;

    if (gpio_init(dev->p.trigger, GPIO_OUT) != 0) {
        DEBUG("[srf04] Error: could not initialize GPIO trigger pin\n");
        return SRF04_ERR_GPIO;
    }

    if (gpio_init_int(dev->p.echo, GPIO_IN, GPIO_BOTH, _cb, (void*)dev) != 0) {
        DEBUG("[srf04] Error: could not initialize GPIO echo pin\n");
        return SRF04_ERR_GPIO;
    }

    gpio_irq_disable(dev->p.echo);

    return SRF04_OK;
}

void srf04_trigger(const srf04_t* dev)
{
    if (dev->distance == SRF04_ERR_MEASURING) {
        return;
    }

    gpio_irq_enable(dev->p.echo);

    gpio_set(dev->p.trigger);
    xtimer_usleep(10);
    gpio_clear(dev->p.trigger);
}

int srf04_read(const srf04_t* dev)
{
    return dev->distance;
}

int srf04_get_distance(const srf04_t* dev)
{
    /* trigger new reading */
    srf04_trigger(dev);
    /* give the sensor the required time for sampling */
    xtimer_usleep(SRF04_SAMPLE_PERIOD);
    /* get the result */
    if (dev->distance >= SRF04_OK) {
        return ((dev->distance * 100) / SRF04_DISTANCE);
    }
    return dev->distance;
}
