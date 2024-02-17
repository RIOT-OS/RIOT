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

#include <errno.h>

#include "cpu.h"
#include "periph/gpio_ll.h"
#include "periph_cpu.h"
#include "periph_conf.h"

int gpio_ll_init(gpio_port_t port, uint8_t pin, gpio_conf_t conf)
{
    GPIO_Mode_TypeDef mode;

    bool initial = conf.initial_value;

    switch (conf.state) {
    case GPIO_DISCONNECT:
        mode = gpioModeDisabled;
        initial = (conf.pull == GPIO_PULL_UP);
        break;
    case GPIO_INPUT:
        switch (conf.pull) {
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
        switch (conf.pull) {
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
        switch (conf.pull) {
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

gpio_conf_t gpio_ll_query_conf(gpio_port_t port, uint8_t pin)
{
    gpio_conf_t result = { 0 };
    GPIO_Mode_TypeDef mode = GPIO_PinModeGet(port, pin);

    result.pull = GPIO_FLOATING;

    switch (mode) {
    case gpioModePushPull:
        result.state = GPIO_OUTPUT_PUSH_PULL;
        break;
    case gpioModeWiredOr:
        result.state = GPIO_OUTPUT_OPEN_SOURCE;
        break;
    case gpioModeWiredOrPullDown:
        result.state = GPIO_OUTPUT_OPEN_SOURCE;
        result.pull = GPIO_PULL_DOWN;
        break;
    case gpioModeWiredAnd:
        result.state = GPIO_OUTPUT_OPEN_DRAIN;
        break;
    case gpioModeWiredAndPullUp:
        result.state = GPIO_OUTPUT_OPEN_DRAIN;
        result.pull = GPIO_PULL_UP;
        break;
    case gpioModeInput:
        result.state = GPIO_INPUT;
        break;
    case gpioModeInputPull:
        result.state = GPIO_INPUT;
        result.pull = GPIO_PinOutGet(port, pin) ?
            GPIO_PULL_UP :
            GPIO_PULL_DOWN;
        break;
    case gpioModeDisabled:
        /* Fall-through: There is no error reporting here */
    default:
        result.state = GPIO_DISCONNECT;
        if (GPIO_PinOutGet(port, pin)) {
            result.pull = GPIO_PULL_UP;
        }
        break;
    }

    result.initial_value = (gpio_ll_read_output(port) >> pin) & 1;

    return result;
}
