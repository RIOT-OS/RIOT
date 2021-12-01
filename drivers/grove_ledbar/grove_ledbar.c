/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_grove_ledbar
 *
 * @{
 * @file
 * @brief       Driver for the Grove ledbar
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "log.h"

#include "grove_ledbar.h"
#include "my9221.h"

#define DEV_LEDS            (dev->params.leds)
#define DEV_STATE(x)        (dev->state[x])

int grove_ledbar_init(grove_ledbar_t *dev, const grove_ledbar_params_t *params)
{
    assert(dev && params);

    return my9221_init((my9221_t *)dev, (my9221_params_t *)params);
}

void grove_ledbar_set(grove_ledbar_t *dev, uint8_t level)
{
    assert(dev);

    uint8_t frac = GROVE_LEDBAR_MAX / DEV_LEDS;

    for (unsigned i = 0; i < DEV_LEDS; ++i) {
        if (level > frac) {
            DEV_STATE(i) = MY9221_LED_ON;
            level -= frac;
        }
        else if (level > 0) {
            DEV_STATE(i) = ~(MY9221_LED_ON << ((level << 3) / frac));
            level = 0;
        }
        else {
            DEV_STATE(i) = MY9221_LED_OFF;
        }
    }
    my9221_set_state((my9221_t *)dev, NULL, 0);
}

void grove_ledbar_clear(grove_ledbar_t *dev)
{
    assert(dev);

    grove_ledbar_set(dev, 0);
}
