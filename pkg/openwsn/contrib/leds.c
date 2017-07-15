/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
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
 *
 * @}
 */

#include "leds.h"
#include "ledpins_riot.h"

#include "board.h"
#include "periph/gpio.h"

#include <stdint.h>
#include <string.h>

/** holds the internal configuration for debug pins */
static ledpins_config_t configuration = {
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
};


/**
 * Provides a simple delay by means of iterating over a 32-bit variable
 */
static void leds_delay(void);

/**
 * Sets the led pins for a specific board for OpenWSN
 *
 * @param[in] user_config A configuration of GPIO pins used for debugging. Unused pins need to be defined as GPIO_UNDEF
 */
void ledpins_riot_init(const ledpins_config_t *user_config)
{
    if (user_config != NULL) {
        memcpy(&configuration, user_config, sizeof(ledpins_config_t));
        leds_init();
    }
}

/**
 * Configure four leds as output which provide error, sync, debug, and
 * radio status information.
 */
void leds_init(void)
{
    if (configuration.error != GPIO_UNDEF) {
        gpio_init(configuration.error, GPIO_OUT);
    }
    else if (configuration.sync != GPIO_UNDEF) {
        gpio_init(configuration.sync, GPIO_OUT);
    }
    else if (configuration.radio != GPIO_UNDEF) {
        gpio_init(configuration.radio, GPIO_OUT);
    }
    else if (configuration.user != GPIO_UNDEF) {
        gpio_init(configuration.user, GPIO_OUT);
    }
    leds_all_off();
}

void leds_error_on(void)
{
    if (configuration.error != GPIO_UNDEF) {
        gpio_set(configuration.error);
    }
}

void leds_error_off(void)
{
    if (configuration.error != GPIO_UNDEF) {
        gpio_clear(configuration.error);
    }
}

void leds_error_toggle(void)
{
    if (configuration.error != GPIO_UNDEF) {
        gpio_toggle(configuration.error);
    }
}

uint8_t leds_error_isOn(void)
{
    uint8_t ret = 0;

    if (configuration.error != GPIO_UNDEF) {
        ret = gpio_read(configuration.error);
    }
    return ret;
}

/**
 * Let the error led blink for "10 seconds".
 */
void leds_error_blink(void)
{
    for (uint8_t i = 0; i < 16; i++) {
        leds_error_toggle();
        leds_delay();
    }
}

void leds_radio_on(void)
{
    if (configuration.radio != GPIO_UNDEF) {
        gpio_set(configuration.radio);
    }
}

void leds_radio_off(void)
{
    if (configuration.radio != GPIO_UNDEF) {
        gpio_clear(configuration.radio);
    }
}

void leds_radio_toggle(void)
{
    if (configuration.radio != GPIO_UNDEF) {
        gpio_toggle(configuration.radio);
    }
}

uint8_t leds_radio_isOn(void)
{
    uint8_t ret = 0;

    if (configuration.radio != GPIO_UNDEF) {
        ret = gpio_read(configuration.radio);
    }
    return ret;
}

void leds_sync_on(void)
{
    if (configuration.sync != GPIO_UNDEF) {
        gpio_set(configuration.sync);
    }
}

void leds_sync_off(void)
{
    if (configuration.sync != GPIO_UNDEF) {
        gpio_clear(configuration.sync);
    }
}

void leds_sync_toggle(void)
{
    if (configuration.sync != GPIO_UNDEF) {
        gpio_toggle(configuration.sync);
    }
}

uint8_t leds_sync_isOn(void)
{
    uint8_t ret = 0;

    if (configuration.sync != GPIO_UNDEF) {
        ret = gpio_read(configuration.sync);
    }
    return ret;
}

void leds_debug_on(void)
{
    if (configuration.user != GPIO_UNDEF) {
        gpio_set(configuration.user);
    }
}

void leds_debug_off(void)
{
    if (configuration.user != GPIO_UNDEF) {
        gpio_clear(configuration.user);
    }
}

void leds_debug_toggle(void)
{
    if (configuration.user != GPIO_UNDEF) {
        gpio_toggle(configuration.user);
    }
}

uint8_t leds_debug_isOn(void)
{
    uint8_t ret = 0;

    if (configuration.user != GPIO_UNDEF) {
        ret = gpio_read(configuration.user);
    }
    return ret;
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
    leds_error_on();
    leds_delay();
    leds_error_off();
    leds_delay();
    leds_radio_on();
    leds_delay();
    leds_radio_off();
    leds_delay();
    leds_sync_on();
    leds_delay();
    leds_sync_off();
    leds_delay();
    leds_debug_on();
    leds_delay();
    leds_debug_off();
}

/**
 * Turns on and off the leds as a binary counter.
 */
void leds_increment(void)
{
    /** not implemented */
    return;
}

static void leds_delay(void)
{
    for (uint32_t i = 0; i < 0x7FFF8; i++) ;
}
