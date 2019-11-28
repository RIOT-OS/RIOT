/*
 * Copyright (C) 2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_apds9007
 * @{
 *
 * @file
 * @brief       Driver for the APDS9007 Light Sensor.
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include <string.h>

#include "apds9007.h"
#include "xtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"


void apds9007_set_active(const apds9007_t *dev)
{
    gpio_write(dev->p.gpio, 0);
}

void apds9007_set_idle(const apds9007_t *dev)
{
    gpio_write(dev->p.gpio, 1);
}

void apds9007_init(apds9007_t *dev, const apds9007_params_t *params)
{
    dev->p.gpio = params->gpio;
    dev->p.adcline = params->adcline;
    dev->p.adcres  = params->adcres;

    gpio_init(params->gpio, GPIO_OUT);
    adc_init(params->adcline);
    apds9007_set_idle(dev);
}

int apds9007_read(const apds9007_t *dev, int16_t *light)
{
    apds9007_set_active(dev);
    xtimer_usleep(APDS9007_STABILIZATION_TIME);

    /* This is raw voltage value, which needs to be converted to lux
     * considering passive components */
    *light = (int16_t) adc_sample(dev->p.adcline, dev->p.adcres);
    apds9007_set_idle(dev);
    if (*light == -1) {
        return APDS9007_ADCERR;
    }
    return APDS9007_OK;
}
