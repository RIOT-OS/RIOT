/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "periph/gpio_ll.h"

/* Optimizing for low stack usage by not using printf(), which on newlib is
 * prohibitively costly. This will allow developers to use this for debugging
 * even in ISR - hopefully without increasing the ISR stack size.
 *
 * If module fmt is used, there is a stack friendly print_str() provided.
 * Otherwise, fall back to fputs(), which is still way more stack friendly than
 * printf().
 */
#ifdef MODULE_FMT
#include "fmt.h"
#else
static inline void print_str(const char *str)
{
    fputs(str, stdout);
}
#endif

const gpio_conf_t gpio_ll_in = {
    .state              = GPIO_INPUT,
    .pull               = GPIO_FLOATING,
    .schmitt_trigger    = true,
};

const gpio_conf_t gpio_ll_in_pd = {
    .state              = GPIO_INPUT,
    .pull               = GPIO_PULL_DOWN,
    .schmitt_trigger    = true,
};

const gpio_conf_t gpio_ll_in_pu = {
    .state              = GPIO_INPUT,
    .pull               = GPIO_PULL_UP,
    .schmitt_trigger    = true,
};

const gpio_conf_t gpio_ll_in_pk = {
    .state              = GPIO_INPUT,
    .pull               = GPIO_PULL_KEEP,
    .schmitt_trigger    = true,
};

const gpio_conf_t gpio_ll_out = {
    .state              = GPIO_OUTPUT_PUSH_PULL,
    .drive_strength     = GPIO_DRIVE_STRONGEST,
    .slew_rate          = GPIO_SLEW_FASTEST,
    .initial_value      = false,
};

const gpio_conf_t gpio_ll_pd = {
    .state              = GPIO_OUTPUT_OPEN_DRAIN,
    .pull               = GPIO_FLOATING,
    .drive_strength     = GPIO_DRIVE_STRONGEST,
    .slew_rate          = GPIO_SLEW_FASTEST,
    .initial_value      = true,
};

const gpio_conf_t gpio_ll_pd_pu = {
    .state              = GPIO_OUTPUT_OPEN_DRAIN,
    .pull               = GPIO_PULL_UP,
    .drive_strength     = GPIO_DRIVE_STRONGEST,
    .slew_rate          = GPIO_SLEW_FASTEST,
    .initial_value      = true,
};

void gpio_ll_print_conf_common(const gpio_conf_t *conf)
{
    assert(conf);
    const char *off_on[] = { "off", "on" };

    print_str("state: ");
    switch (conf->state) {
    case GPIO_OUTPUT_PUSH_PULL:
        print_str("out-pp");
        break;
    case GPIO_OUTPUT_OPEN_DRAIN:
        print_str("out-od");
        break;
    case GPIO_OUTPUT_OPEN_SOURCE:
        print_str("out-os");
        break;
    case GPIO_INPUT:
        print_str("in");
        break;
    case GPIO_USED_BY_PERIPHERAL:
        print_str("periph");
        break;
    case GPIO_DISCONNECT:
        print_str("off");
        break;
    }

    if (conf->state != GPIO_INPUT) {
        if (GPIO_DRIVE_NUMOF > 1) {
            print_str(", drive: ");
            if (conf->drive_strength == GPIO_DRIVE_WEAK) {
                print_str("weak");
            }
            else if (conf->drive_strength == GPIO_DRIVE_WEAKEST) {
                print_str("weakest");
            }
            else if (conf->drive_strength == GPIO_DRIVE_STRONG) {
                print_str("strong");
            }
            else {
                print_str("strongest");
            }
        }
        if (GPIO_SLEW_NUMOF > 1) {
            print_str(", slew: ");
            if (conf->slew_rate == GPIO_SLEW_SLOW) {
                print_str("slow");
            }
            else if (conf->slew_rate == GPIO_SLEW_SLOWEST) {
                print_str("slowest");
            }
            else if (conf->slew_rate == GPIO_SLEW_FAST) {
                print_str("fast");
            }
            else {
                print_str("fastest");
            }
        }
    }

    if (conf->state != GPIO_OUTPUT_PUSH_PULL) {
        print_str(", pull: ");
        switch (conf->pull) {
        default:
        case GPIO_FLOATING:
            print_str("none");
            break;
        case GPIO_PULL_UP:
            print_str("up");
            break;
        case GPIO_PULL_DOWN:
            print_str("down");
            break;
        case GPIO_PULL_KEEP:
            print_str("keep");
            break;
        }

        if ((conf->pull != GPIO_FLOATING) && (GPIO_PULL_NUMOF > 1)) {
            print_str(" (");
            if (conf->pull_strength == GPIO_PULL_WEAK) {
                print_str("weak");
            }
            else if (conf->pull_strength == GPIO_PULL_WEAKEST) {
                print_str("weakest");
            }
            else if (conf->pull_strength == GPIO_PULL_STRONG) {
                print_str("strong");
            }
            else {
                print_str("strongest");
            }
            print_str(")");
        }

        print_str(", schmitt trigger: ");
        print_str(off_on[conf->schmitt_trigger]);
    }

    print_str(", value: ");
    print_str(off_on[conf->initial_value]);
}

/* implement gpio_ll_print_conf as weak alias symbol for
 * gpio_ll_print_conf_common - so that platform specific implementations can
 * override gpio_ll_print_conf while reusing gpio_ll_print_conf_common()
 */
__attribute__((weak, alias("gpio_ll_print_conf_common")))
void gpio_ll_print_conf(const gpio_conf_t *conf);
