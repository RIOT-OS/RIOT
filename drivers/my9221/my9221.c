/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_my9221
 *
 * @{
 * @file
 * @brief       Driver for the MY9221 LED controller
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "log.h"
#include "periph/gpio.h"
#include "xtimer.h"

#include "my9221.h"
#include "my9221_internal.h"

#define DEV_DIR             (dev->params.dir)
#define DEV_LEDS            (dev->params.leds)
#define DEV_STATE(x)        (dev->state[x])
#define PIN_CLK             (dev->params.clk)
#define PIN_DAT             (dev->params.dat)

/**
 * @brief Write a single data to the LED controller
 */
static void _write(my9221_t *dev, uint16_t data)
{
    assert(dev);

    for (unsigned i = 0; i < 16; ++i) {
        (data & 0x8000) ? gpio_set(PIN_DAT) : gpio_clear(PIN_DAT);
        gpio_toggle(PIN_CLK);
        data <<= 1;
    }
}

/**
 * @brief Load data into the latch register of the LED controller
 */
static void _latch(my9221_t *dev)
{
    assert(dev);

    gpio_clear(PIN_DAT);
    xtimer_usleep(MY9221_LATCH_WAIT);

    for (unsigned i = 0; i < MY9221_LATCH_LOOP; ++i) {
        gpio_set(PIN_DAT);
        gpio_clear(PIN_DAT);
    }
}

/**
 * @brief Write state data of all LEDs to the controller
 */
static void _set_state(my9221_t *dev)
{
    assert(dev);

    _write(dev, MY9221_CMDMODE);
    for (unsigned i = 0; i < DEV_LEDS; ++i) {
        if (DEV_DIR == MY9221_DIR_REV) {
            /* Write LED state in reverse order */
            _write(dev, DEV_STATE(DEV_LEDS-i-1));
        }
        else {
            /* Write LED state in forward order */
            _write(dev, DEV_STATE(i));
        }
    }
    /* set unused LED pins to off */
    for (unsigned j = DEV_LEDS; j < MY9221_LED_MAX; ++j) {
        _write(dev, MY9221_LED_OFF);
    }

    _latch(dev);
}

int my9221_init(my9221_t *dev, const my9221_params_t *params)
{
    assert(dev);
    assert(params);
    /* write config params to device descriptor */
    dev->params = *params;
    /* init clock and data pins as output */
    gpio_init(PIN_CLK, GPIO_OUT);
    gpio_init(PIN_DAT, GPIO_OUT);
    /* reset state, i.e., all LEDs off */
    memset(dev->state, 0, sizeof(dev->state));
    _set_state(dev);

    return MY9221_OK;
}

void my9221_set_state(my9221_t *dev, const uint8_t *state, uint8_t len)
{
    if ((state != NULL) && (len > 0)) {
        if (len > MY9221_LED_MAX) {
            len = MY9221_LED_MAX;
        }
        memcpy(dev->state, state, len);
    }
    _set_state(dev);
}

void my9221_set_led(my9221_t *dev, const uint8_t led, const uint8_t alpha)
{
    assert(dev);
    assert(led < DEV_LEDS);

    DEV_STATE(led) = ~(MY9221_LED_ON << ((uint16_t)(alpha << 3) >> 8));
    _set_state(dev);
}

void my9221_toggle_led(my9221_t *dev, const uint8_t led)
{
    assert(dev);
    assert(led < DEV_LEDS);

    DEV_STATE(led) = DEV_STATE(led) ? MY9221_LED_OFF : MY9221_LED_ON;
    _set_state(dev);
}
