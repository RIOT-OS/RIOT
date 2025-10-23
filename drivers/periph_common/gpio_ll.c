/*
 * SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

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
#  include "fmt.h"
#else
#  include <stdio.h>
static inline void print_str(const char *str)
{
    fputs(str, stdout);
}
#endif

void gpio_ll_print_conf_common(const gpio_conf_t conf)
{
    const char *off_on[] = { "off", "on" };

    print_str("state: ");
    switch (conf.state) {
    case GPIO_INPUT:
        print_str("in");
        break;
    case GPIO_OUTPUT_PUSH_PULL:
        print_str("out-pp");
        break;
#if MODULE_PERIPH_GPIO_LL_OPEN_DRAIN
    case GPIO_OUTPUT_OPEN_DRAIN:
        print_str("out-od");
        break;
#endif
#if MODULE_PERIPH_GPIO_LL_OPEN_SOURCE
    case GPIO_OUTPUT_OPEN_SOURCE:
        print_str("out-os");
        break;
#endif
    case GPIO_USED_BY_PERIPHERAL:
        print_str("periph");
        break;
#if MODULE_PERIPH_GPIO_LL_DISCONNECT
    case GPIO_DISCONNECT:
        print_str("off");
        break;
#endif
    default:
        print_str("?");
        break;
    }

    if (conf.state != GPIO_OUTPUT_PUSH_PULL) {
        print_str(", pull: ");
        switch (conf.pull) {
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
    }

    print_str(", value: ");
    print_str(off_on[conf.initial_value]);
}

/* implement gpio_ll_print_conf as weak alias symbol for
 * gpio_ll_print_conf_common - so that platform specific implementations can
 * override gpio_ll_print_conf while reusing gpio_ll_print_conf_common()
 */
__attribute__((weak, alias("gpio_ll_print_conf_common")))
void gpio_ll_print_conf(gpio_conf_t conf);
