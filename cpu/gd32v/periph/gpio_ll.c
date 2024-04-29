/*
 * Copyright (C) 2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_gd32v
 * @ingroup     drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief       GPIO Low-level API implementation for the GD32V GPIO peripheral
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <errno.h>

#include "cpu.h"
#include "bitarithm.h"
#include "periph/gpio_ll.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#ifdef MODULE_FMT
#include "fmt.h"
#else
static inline void print_str(const char *str)
{
    fputs(str, stdout);
}
#endif

uint16_t pin_used[GPIO_PORT_NUMOF] = {};

int gpio_ll_init(gpio_port_t port, uint8_t pin, gpio_conf_t conf)
{
    if ((conf.pull == GPIO_PULL_KEEP) ||
        (conf.state == GPIO_OUTPUT_OPEN_SOURCE) ||
        ((conf.state != GPIO_INPUT) && (conf.pull != GPIO_FLOATING))) {
        return -ENOTSUP;
    }

    unsigned state = irq_disable();

    periph_clk_en(APB2, (RCU_APB2EN_PAEN_Msk << gpio_port_num(port)));

    volatile uint32_t *ctrl = (pin < 8) ? &((GPIO_Type *)port)->CTL0
                                        : &((GPIO_Type *)port)->CTL1;
    volatile uint32_t *octl = &((GPIO_Type *)port)->OCTL;
    unsigned pos = ((pin % 8) * 4);

    /* reset configuration CTLx[1:0], MDx[1:0] (analogue, input mode) */
    *ctrl &= ~(0xf << pos);

    switch (conf.state) {
    case GPIO_DISCONNECT:
        pin_used[gpio_port_num(port)] &= ~(1 << pin);
        if (pin_used[gpio_port_num(port)] == 0) {
            periph_clk_dis(APB2, (RCU_APB2EN_PAEN_Msk << gpio_port_num(port)));
        }
        break;
    case GPIO_INPUT:
        pin_used[gpio_port_num(port)] |= 1 << pin;
        if (conf.pull == GPIO_FLOATING) {
            *ctrl |= 0x1 << (pos + 2);
        }
        else {
            *ctrl |= 0x2 << (pos + 2);
            if (conf.pull == GPIO_PULL_UP) {
                *octl |= 1 << pin;
            }
            else {
                *octl &= ~(1 << pin);
            }
        }
        break;
    case GPIO_OUTPUT_PUSH_PULL:
    case GPIO_OUTPUT_OPEN_DRAIN:
        pin_used[gpio_port_num(port)] |= 1 << pin;
        *ctrl |= (conf.slew_rate + 1) << pos;
        *ctrl |= (conf.state == GPIO_OUTPUT_OPEN_DRAIN ? 0x1 : 0x0) << (pos + 2);
        if (conf.initial_value) {
            gpio_ll_set(port, 1UL << pin);
        }
        else {
            gpio_ll_clear(port, 1UL << pin);
        }
        break;
    default:
        irq_restore(state);
        return -ENOTSUP;
    }

    irq_restore(state);

    return 0;
}

gpio_conf_t gpio_ll_query_conf(gpio_port_t port, uint8_t pin)
{
    gpio_conf_t result = { 0 };

    unsigned state = irq_disable();

    volatile uint32_t *ctrl_reg = (pin < 8) ? &((GPIO_Type *)port)->CTL0
                                            : &((GPIO_Type *)port)->CTL1;

    unsigned pos = ((pin % 8) * 4);

    uint32_t mode = (*ctrl_reg >> pos) & 0x3;
    uint32_t ctrl = (*ctrl_reg >> (pos + 2)) & 0x3;

    if (mode == 0) {
        result.state = GPIO_INPUT;
        switch (ctrl) {
        case 0:
            result.state = GPIO_DISCONNECT;
            break;
        case 1:
            result.pull = GPIO_FLOATING;
            break;
        case 2:
            result.pull = (((GPIO_Type *)port)->OCTL & (1UL << pin)) ? GPIO_PULL_UP
                                                                     : GPIO_PULL_DOWN;
            break;
        default:
            break;
        }
    }
    else {
        result.slew_rate = mode - 1;
        result.pull = GPIO_FLOATING;
        switch (ctrl) {
        case 0:
            result.state = GPIO_OUTPUT_PUSH_PULL;
            break;
        case 1:
            result.state = GPIO_OUTPUT_OPEN_DRAIN;
            break;
        default:
            result.state = GPIO_USED_BY_PERIPHERAL;
            break;
        }
    }

    if (result.state == GPIO_INPUT) {
        result.initial_value = (gpio_ll_read(port) >> pin) & 1UL;
    }
    else {
        result.initial_value = (gpio_ll_read_output(port) >> pin) & 1UL;
    }
    irq_restore(state);

    return result;
}

void gpio_ll_print_conf(gpio_conf_t conf)
{
    static const char *slew_strs[] = {
        [GPIO_SLEW_SLOWEST] = "slowest",
        [GPIO_SLEW_SLOW] = "medium",
        [GPIO_SLEW_FASTEST] = "fastest",
        "invalid"
    };

    gpio_ll_print_conf_common(conf);
    print_str(", slew: ");
    print_str(slew_strs[conf.slew_rate]);
}
