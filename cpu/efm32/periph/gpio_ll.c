/*
 * Copyright (C) 2022 Christian Amsüss
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief       Peripheral GPIO Low-Level API implementation for the EFM32 family
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "cpu.h"
#include "periph/gpio_ll.h"
#include "periph_cpu.h"
#include "periph_conf.h"

int gpio_ll_init(gpio_port_t port, uint8_t pin, const gpio_conf_t *conf)
{
    GPIO_Mode_TypeDef mode;

    bool initial = conf->initial_value;

    switch (conf->state) {
    case GPIO_DISCONNECT:
        /* ignoring pull */
        mode = gpioModeDisabled;
        break;
    case GPIO_INPUT:
        switch (conf->pull) {
        case GPIO_FLOATING:
            mode = gpioModeInput;
            break;
        case GPIO_PULL_UP:
            initial = true;
            mode = gpioModeInputPull;
            break;
        case GPIO_PULL_DOWN:
            initial = false;
            mode = gpioModeInputPull;
            break;
        default:
            return -ENOTSUP;
        }
        break;
    case GPIO_OUTPUT_PUSH_PULL:
        /* ignoring pull */
        mode = gpioModePushPull;
        break;
    case GPIO_OUTPUT_OPEN_DRAIN:
        switch (conf->pull) {
        case GPIO_FLOATING:
            mode = gpioModeWiredAnd;
            break;
        case GPIO_PULL_UP:
            mode = gpioModeWiredAndPullUp;
            break;
        default: /* including PULL_DOWN, which makes no sense here*/
            return -ENOTSUP;
        }
        break;
    case GPIO_OUTPUT_OPEN_SOURCE:
        switch (conf->pull) {
        case GPIO_FLOATING:
            mode = gpioModeWiredOr;
            break;
        case GPIO_PULL_DOWN:
            mode = gpioModeWiredOrPullDown;
            break;
        default: /* including PULL_UP, which makes no sense here*/
            return -ENOTSUP;
        }
        break;
    case GPIO_USED_BY_PERIPHERAL:
        /* Needs to be configured to what the peripheral actually needs
         * instead (eg. DISABLED for analog input, some output for timers and
         * UARTs etc); fall-through */
    default:
        /* Some probably are by the hardware, but not yet by this
         * implementation */
        return -ENOTSUP;
    }

    GPIO_PinModeSet(port, pin, mode, initial);

    return 0;
}

void gpio_ll_query_conf(gpio_conf_t *dest, gpio_port_t port, uint8_t pin)
{
    memset(dest, 0, sizeof(*dest));

    GPIO_Mode_TypeDef mode = GPIO_PinModeGet(port, pin);

    dest->pull = GPIO_FLOATING;

    switch (mode) {
    case gpioModePushPull:
        dest->state = GPIO_OUTPUT_PUSH_PULL;
        break;
    case gpioModeWiredOr:
        dest->state = GPIO_OUTPUT_OPEN_SOURCE;
        break;
    case gpioModeWiredOrPullDown:
        dest->state = GPIO_OUTPUT_OPEN_SOURCE;
        dest->pull = GPIO_PULL_DOWN;
        break;
    case gpioModeWiredAnd:
        dest->state = GPIO_OUTPUT_OPEN_DRAIN;
        break;
    case gpioModeWiredAndPullUp:
        dest->state = GPIO_OUTPUT_OPEN_DRAIN;
        dest->pull = GPIO_PULL_UP;
        break;
    case gpioModeInput:
        dest->state = GPIO_INPUT;
        break;
    case gpioModeInputPull:
        dest->state = GPIO_INPUT;
        dest->pull = GPIO_PinOutGet(port, pin) ?
            GPIO_PULL_UP :
            GPIO_PULL_DOWN;
        break;
    case gpioModeDisabled:
        /* Fall-through: There is no error reporting here */
    default:
        dest->state = GPIO_DISCONNECT;
        break;
    }

    /* as good as any */
    dest->slew_rate = GPIO_SLEW_FAST;

    /* It's always on as long as they're in a mode in which it matters, judging
     * from https://www.silabs.com/documents/public/application-notes/an0027.pdf */
    dest->schmitt_trigger = true;

    dest->initial_value = (gpio_ll_read_output(port) >> pin) & 1;

    /* Using 'strong' her already as that fits with what the hardware has
     * (lowest, low, standard, high) */
    dest->drive_strength = GPIO_DRIVE_STRONG;
}
