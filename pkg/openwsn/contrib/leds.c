/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 *
 * @author      Michael Frey <michael.frey@msasafety.com>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include "leds.h"
#include "openwsn_leds.h"

#include "clk.h"
#include "board.h"
#include "periph/gpio.h"

#include <stdint.h>
#include <string.h>

/** holds the internal configuration for debug pins */
static leds_config_t _configuration = {
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_LED_HIGH,
};

static void _toggle_checked(gpio_t pin)
{
    if (IS_USED(MODULE_OPENWSN_LEDS)) {
        if (gpio_is_valid(pin)) {
            gpio_toggle(pin);
        }
    }
}

static void _init_checked(gpio_t pin)
{
    if (IS_USED(MODULE_OPENWSN_LEDS)) {
        if (gpio_is_valid(pin)) {
            gpio_init(pin, GPIO_OUT);
        }
    }
}

static void _write_checked(gpio_t pin, uint8_t on_state)
{
    if (IS_USED(MODULE_OPENWSN_LEDS)) {
        if (gpio_is_valid(pin)) {
            gpio_write(pin, on_state);
        }
    }
}

static uint8_t _is_on_checked(gpio_t pin)
{
    if (IS_USED(MODULE_OPENWSN_LEDS)) {
        uint8_t ret = 0;

        if (gpio_is_valid(pin)) {
            ret = gpio_read(pin);
        }
        return ret;
    }
    else {
        return 0;
    }
}

static void _blink_checked(gpio_t pin)
{
    if (IS_USED(MODULE_OPENWSN_LEDS)) {
        /* toggle for ~10s if ztimer is used */
        for (uint8_t i = 0; i < 100; i++) {
            _toggle_checked(pin);
            uint32_t runs = coreclk() / 50;
            for (uint32_t i = 0; i < runs; i++) {
                /* Make sure for loop is not optimized out */
                __asm__ ("");
            }
        }
    }
}

void ledpins_riot_init(const leds_config_t *user_config)
{
    if (IS_USED(MODULE_OPENWSN_LEDS)) {
        if (user_config != NULL) {
            memcpy(&_configuration, user_config, sizeof(leds_config_t));
            leds_init();
        }
    }
    else {
        (void)user_config;
    }
}

void leds_init(void)
{
    _init_checked(_configuration.error);
    _init_checked(_configuration.sync);
    _init_checked(_configuration.radio);
    _init_checked(_configuration.debug);
    leds_all_off();
}

void leds_error_on(void)
{
    _write_checked(_configuration.error, _configuration.led_on);
}

void leds_error_off(void)
{
    _write_checked(_configuration.error, ~_configuration.led_on);
}

void leds_error_toggle(void)
{
    _toggle_checked(_configuration.error);
}

uint8_t leds_error_isOn(void)
{
    return _is_on_checked(_configuration.error);
}

void leds_error_blink(void)
{
    _blink_checked(_configuration.error);
}

void leds_radio_on(void)
{
    _write_checked(_configuration.radio, _configuration.led_on);
}

void leds_radio_off(void)
{
    _write_checked(_configuration.radio, ~_configuration.led_on);
}

void leds_radio_toggle(void)
{
    _toggle_checked(_configuration.radio);
}

uint8_t leds_radio_isOn(void)
{
    return _is_on_checked(_configuration.radio);
}

void leds_sync_on(void)
{
    _write_checked(_configuration.sync, _configuration.led_on);
}

void leds_sync_off(void)
{
    _write_checked(_configuration.sync, ~_configuration.led_on);
}

void leds_sync_toggle(void)
{
    _toggle_checked(_configuration.sync);
}

uint8_t leds_sync_isOn(void)
{
    return _is_on_checked(_configuration.sync);
}

void leds_debug_on(void)
{
    _write_checked(_configuration.debug, _configuration.led_on);
}

void leds_debug_off(void)
{
    _write_checked(_configuration.debug, ~_configuration.led_on);
}

void leds_debug_toggle(void)
{
    _toggle_checked(_configuration.debug);
}

uint8_t leds_debug_isOn(void)
{
    return _is_on_checked(_configuration.debug);
}

void leds_all_on(void)
{
    leds_error_on();
    leds_radio_on();
    leds_sync_on();
    leds_debug_on();
}

void leds_all_off(void)
{
    leds_error_off();
    leds_radio_off();
    leds_sync_off();
    leds_debug_off();
}

void leds_all_toggle(void)
{
    leds_error_toggle();
    leds_radio_toggle();
    leds_sync_toggle();
    leds_debug_toggle();
}

void leds_circular_shift(void)
{
    /** not implemented */
}

void leds_increment(void)
{
    /** not implemented */
}
