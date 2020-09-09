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
 * @brief       RIOT adaption of the "debugpins" bsp module
 *
 * @author      Michael Frey <michael.frey@msasafety.com>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include "debugpins.h"
#include "openwsn_debugpins.h"

#include <stdint.h>
#include <string.h>

/* holds the internal configuration for debugpins */
static debugpins_config_t _configuration = {
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF
};

static void _set_checked(gpio_t pin)
{
    if (IS_USED(MODULE_OPENWSN_DEBUGPINS)) {
        if (gpio_is_valid(pin)){
            gpio_set(pin);
        }
    }
}

static void _clear_checked(gpio_t pin)
{
    if (IS_USED(MODULE_OPENWSN_DEBUGPINS)) {
        if (gpio_is_valid(pin)){
            gpio_clear(pin);
        }
    }
}

static void _toggle_checked(gpio_t pin)
{
    if (IS_USED(MODULE_OPENWSN_DEBUGPINS)) {
        if (gpio_is_valid(pin)){
            gpio_toggle(pin);
        }
    }
}

static void _init_checked(gpio_t pin)
{
    if (IS_USED(MODULE_OPENWSN_DEBUGPINS)) {
        if (gpio_is_valid(pin)){
            gpio_init(pin, GPIO_OUT);
        }
    }
}

void openwsn_debugpins_init(const debugpins_config_t *user_config)
{
    if (IS_USED(MODULE_OPENWSN_DEBUGPINS)) {
        memset(&_configuration, GPIO_UNDEF, sizeof(debugpins_config_t));

        if (user_config != NULL) {
            memcpy(&_configuration, user_config, sizeof(debugpins_config_t));
            debugpins_init();
        }
    }
    else {
        (void) user_config;
    }
}

void debugpins_init(void)
{
    _init_checked(_configuration.frame);
    _init_checked(_configuration.slot);
    _init_checked(_configuration.fsm);
    _init_checked(_configuration.task);
    _init_checked(_configuration.isr);
    _init_checked(_configuration.radio);

    debugpins_frame_clr();
    debugpins_slot_clr();
    debugpins_fsm_clr();
    debugpins_task_clr();
    debugpins_isr_clr();
    debugpins_radio_clr();
}

void debugpins_frame_toggle(void)
{
    _toggle_checked(_configuration.frame);
}

void debugpins_frame_clr(void)
{
    _clear_checked(_configuration.frame);
}

void debugpins_frame_set(void)
{
    _set_checked(_configuration.frame);
}

void debugpins_slot_toggle(void)
{
    _toggle_checked(_configuration.slot);
}

void debugpins_slot_clr(void)
{
    _clear_checked(_configuration.slot);
}

void debugpins_slot_set(void)
{
    _set_checked(_configuration.slot);
}

void debugpins_fsm_toggle(void)
{
    _toggle_checked(_configuration.fsm);
}

void debugpins_fsm_clr(void)
{
    _clear_checked(_configuration.fsm);
}

void debugpins_fsm_set(void)
{
    _set_checked(_configuration.fsm);
}

void debugpins_task_toggle(void)
{
    _toggle_checked(_configuration.task);
}

void debugpins_task_clr(void)
{
    _clear_checked(_configuration.task);
}

void debugpins_task_set(void)
{
    _set_checked(_configuration.task);
}

void debugpins_isr_toggle(void)
{
    _toggle_checked(_configuration.isr);
}

void debugpins_isr_clr(void)
{
    _clear_checked(_configuration.isr);
}

void debugpins_isr_set(void)
{
    _set_checked(_configuration.isr);
}

void debugpins_radio_toggle(void)
{
    _toggle_checked(_configuration.radio);
}

void debugpins_radio_clr(void)
{
    _clear_checked(_configuration.radio);
}

void debugpins_radio_set(void)
{
    _set_checked(_configuration.radio);
}
