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

#include "debugpins.h"
#include "debugpins_riot.h"

#include <stdint.h>
#include <string.h>

/** holds the internal configuration for debug pins */
static debugpins_config_t configuration = {
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF
};

/**
 * Sets the debug pins for a specific board for OpenWSN
 *
 * @param[in] user_config A configuration of GPIO pins used for debugging. Unused pins need to be defined as GPIO_UNDEF
 */
void debugpins_riot_init(const debugpins_config_t *user_config)
{
    memset(&configuration, GPIO_UNDEF, sizeof(debugpins_config_t));

    if (user_config != NULL) {
        memcpy(&configuration, user_config, sizeof(debugpins_config_t));
        debugpins_init();
    }
}

void debugpins_init(void)
{
    gpio_init(configuration.frame, GPIO_OUT);
    gpio_init(configuration.slot, GPIO_OUT);
    gpio_init(configuration.fsm, GPIO_OUT);
    gpio_init(configuration.task, GPIO_OUT);
    gpio_init(configuration.isr, GPIO_OUT);
    gpio_init(configuration.radio, GPIO_OUT);

    debugpins_frame_clr();
    debugpins_slot_clr();
    debugpins_fsm_clr();
    debugpins_task_clr();
    debugpins_isr_clr();
    debugpins_radio_clr();
}

void debugpins_frame_toggle(void)
{
    if (configuration.frame != GPIO_UNDEF) {
        gpio_toggle(configuration.frame);
    }
}

void debugpins_frame_clr(void)
{
    if (configuration.frame != GPIO_UNDEF) {
        gpio_clear(configuration.frame);
    }
}

void debugpins_frame_set(void)
{
    if (configuration.frame != GPIO_UNDEF) {
        gpio_set(configuration.frame);
    }
}

void debugpins_slot_toggle(void)
{
    if (configuration.slot != GPIO_UNDEF) {
        gpio_toggle(configuration.slot);
    }
}

void debugpins_slot_clr(void)
{
    if (configuration.slot != GPIO_UNDEF) {
        gpio_clear(configuration.slot);
    }
}

void debugpins_slot_set(void)
{
    if (configuration.slot != GPIO_UNDEF) {
        gpio_set(configuration.slot);
    }
}

void debugpins_fsm_toggle(void)
{
    if (configuration.fsm != GPIO_UNDEF) {
        gpio_toggle(configuration.fsm);
    }
}

void debugpins_fsm_clr(void)
{
    if (configuration.fsm != GPIO_UNDEF) {
        gpio_clear(configuration.fsm);
    }
}

void debugpins_fsm_set(void)
{
    if (configuration.fsm != GPIO_UNDEF) {
        gpio_set(configuration.fsm);
    }
}

void debugpins_task_toggle(void)
{
    if (configuration.task != GPIO_UNDEF) {
        gpio_toggle(configuration.task);
    }
}

void debugpins_task_clr(void)
{
    if (configuration.task != GPIO_UNDEF) {
        gpio_clear(configuration.task);
    }
}

void debugpins_task_set(void)
{
    if (configuration.task != GPIO_UNDEF) {
        gpio_set(configuration.task);
    }
}

void debugpins_isr_toggle(void)
{
    if (configuration.isr != GPIO_UNDEF) {
        gpio_toggle(configuration.isr);
    }
}

void debugpins_isr_clr(void)
{
    if (configuration.isr != GPIO_UNDEF) {
        gpio_clear(configuration.isr);
    }
}

void debugpins_isr_set(void)
{
    if (configuration.isr != GPIO_UNDEF) {
        gpio_set(configuration.isr);
    }
}

void debugpins_radio_toggle(void)
{
    if (configuration.radio != GPIO_UNDEF) {
        gpio_toggle(configuration.radio);
    }
}

void debugpins_radio_clr(void)
{
    if (configuration.radio != GPIO_UNDEF) {
        gpio_clear(configuration.radio);
    }
}

void debugpins_radio_set(void)
{
    if (configuration.radio != GPIO_UNDEF) {
        gpio_set(configuration.radio);
    }
}
