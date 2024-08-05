/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_msp430
 * @ingroup     drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief       Peripheral GPIO Low-Level API implementation for the MSP430 GPIO peripheral
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include <errno.h>
#include <stdbool.h>

#include "periph/gpio_ll.h"

gpio_port_t gpio_port(uword_t num)
{
    switch (num) {
    case 1:
        return (gpio_port_t)&PORT_1.base;
    case 2:
        return (gpio_port_t)&PORT_2.base;
    case 3:
        return (gpio_port_t)&PORT_3.base;
    case 4:
        return (gpio_port_t)&PORT_4.base;
    case 5:
        return (gpio_port_t)&PORT_5.base;
    case 6:
        return (gpio_port_t)&PORT_6.base;
    default:
        return 0;
    }
}

uword_t gpio_port_num(gpio_port_t port)
{
    /* we need to hard-code the values here to have constant initializers in
     * the look-up table :-/ */
    static const uint8_t map[] = {
        0x20, /* == GPIO_PORT_1 */
        0x28, /* == GPIO_PORT_2 */
        0x10, /* == GPIO_PORT_3 */
        0x1C, /* == GPIO_PORT_4 */
        0x30, /* == GPIO_PORT_5 */
        0x34, /* == GPIO_PORT_6 */
#if CPU_FAM_MSP430_F2XX_G2XX
        0x38, /* == GPIO_PORT_7 */
        0x39, /* == GPIO_PORT_8 */
#endif
    };

    for (unsigned i = 0; i < ARRAY_SIZE(map); i++) {
        if (port == (gpio_port_t)map[i]) {
            return i + 1;
        }
    }
    return 0;
}

#if CPU_FAM_MSP430_F2XX_G2XX
static void _set_pull(unsigned port_num, uword_t mask, bool enable)
{
    volatile uint8_t *ren_reg;
    switch (port_num) {
    case 1:
        ren_reg = &PORT_1.REN;
        break;
    case 2:
        ren_reg = &PORT_2.REN;
        break;
    default:
        ren_reg = &P3REN;
        ren_reg += (port_num - 3);
    }

    if (enable) {
        *ren_reg |= mask;
    }
    else {
        *ren_reg &= ~(mask);
    }
}

static bool _get_pull(unsigned port_num, uword_t mask)
{
    volatile uint8_t *ren_reg;
    switch (port_num) {
    case 1:
        ren_reg = &PORT_1.REN;
        break;
    case 2:
        ren_reg = &PORT_2.REN;
        break;
    default:
        ren_reg = &P3REN;
        ren_reg += (port_num - 3);
    }

    return*ren_reg & mask;
}
#endif

int gpio_ll_init(gpio_port_t port, uint8_t pin, gpio_conf_t conf)
{
    msp430_port_t *p = (void *)port;

    if (!IS_ACTIVE(CPU_FAM_MSP430_F2XX_G2XX) && (conf.pull != GPIO_FLOATING)) {
        return -ENOTSUP;
    }

    switch (conf.state) {
    case GPIO_INPUT:
    case GPIO_OUTPUT_PUSH_PULL:
        break;
    default:
        return -ENOTSUP;
    }

    uword_t mask = 1U << pin;
    bool initial_value = conf.initial_value;

#if CPU_FAM_MSP430_F2XX_G2XX
    unsigned port_num = gpio_port_num(port);
    switch (conf.pull) {
    default:
        return -ENOTSUP;
    case GPIO_FLOATING:
        _set_pull(port_num, mask, false);
        break;
    case GPIO_PULL_UP:
        _set_pull(port_num, mask, true);
        initial_value = true;
        break;
    case GPIO_PULL_DOWN:
        _set_pull(port_num, mask, true);
        initial_value = false;
        break;
    }
#endif

    if (initial_value) {
        gpio_ll_set(port, mask);
    }
    else {
        gpio_ll_clear(port, mask);
    }

    if (conf.state == GPIO_OUTPUT_PUSH_PULL) {
        /* No need to disable IRQs, the BIS.B instruction is atomically */
        p->DIR |= mask;
    }
    else {
        /* No need to disable IRQs, the BIC.B instruction is atomically */
        p->DIR &= ~mask;
    }

    return 0;
}

gpio_conf_t gpio_ll_query_conf(gpio_port_t port, uint8_t pin)
{
    msp430_port_t *p = (void *)port;
    uword_t mask = 1U << pin;
    gpio_conf_t result = { 0 };
    if (p->DIR & mask) {
        result.state = GPIO_OUTPUT_PUSH_PULL;
        result.initial_value = gpio_ll_read_output(port) & mask;
    }
    else {
        result.state = GPIO_INPUT;
        result.initial_value = gpio_ll_read(port) & mask;
#if CPU_FAM_MSP430_F2XX_G2XX
        uword_t port_num = gpio_port_num(port);
        if (_get_pull(port_num, mask)) {
            if (gpio_ll_read_output(port) & mask) {
                result.pull = GPIO_PULL_UP;
            }
            else {
                result.pull = GPIO_PULL_DOWN;
            }
        }
#endif
    }

    return result;
}
