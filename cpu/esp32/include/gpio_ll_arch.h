/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief       CPU specific part of the Peripheral GPIO Low-Level API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef GPIO_LL_ARCH_H
#define GPIO_LL_ARCH_H

#include "gpio_arch.h"
#include "irq.h"
#include "soc/gpio_reg.h"
#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

#define GPIO_PORT(num)          (num)
#if GPIO_PORT_NUMOF > 1
#  define GPIO_PORT_NUM(port)   (port)
#else
#  define GPIO_PORT_NUM(port)   0
#endif

static inline uword_t gpio_ll_read(gpio_port_t port)
{
    static_assert(GPIO_PORT_NUMOF < 3);
    volatile uword_t *in = (uint32_t *)GPIO_IN_REG;
    /* return 0 for unconfigured pins, the current level at the pin otherwise */
#if GPIO_PORT_NUM > 1
    if (GPIO_PORT_NUM(port) != 0) {
        in = (uint32_t *)GPIO_IN1_REG;
    }
#endif

    return *in;
}

static inline uword_t gpio_ll_read_output(gpio_port_t port)
{
    static_assert(GPIO_PORT_NUMOF < 3);
    volatile uword_t *out = (uint32_t *)GPIO_OUT_REG;
#if GPIO_PORT_NUM > 1
    if (GPIO_PORT_NUM(port) != 0) {
        out = (uint32_t *)GPIO_OUT1_REG;
    }
#endif

    return *out;
}

static inline void gpio_ll_set(gpio_port_t port, uword_t mask)
{
    static_assert(GPIO_PORT_NUMOF < 3);
    volatile uword_t *out_w1ts = (uint32_t *)GPIO_OUT_W1TS_REG;
    if (GPIO_PORT_NUM(port) != 0) {
#if GPIO_PORT_NUM > 1
        out_w1ts = (uint32_t)GPIO_OUT1_W1TS;
#endif
    }

    *out_w1ts = mask;
}

static inline void gpio_ll_clear(gpio_port_t port, uword_t mask)
{
    static_assert(GPIO_PORT_NUMOF < 3);
    volatile uword_t *out_w1tc = (uint32_t *)GPIO_OUT_W1TC_REG;
    if (GPIO_PORT_NUM(port) != 0) {
#if GPIO_PORT_NUM > 1
        out_w1tc = (uint32_t)GPIO_OUT1_W1TC;
#endif
    }

    *out_w1tc = mask;
}

static inline void gpio_ll_toggle(gpio_port_t port, uword_t mask)
{
    static_assert(GPIO_PORT_NUMOF < 3);
    volatile uword_t *out = (uint32_t *)GPIO_OUT_REG;
#if GPIO_PORT_NUM > 1
    if (GPIO_PORT_NUM(port) != 0) {
        out = (uint32_t *)GPIO_OUT1_REG;
    }
#endif
    unsigned irq_state = irq_disable();
    *out ^= mask;
    irq_restore(irq_state);
}

static inline void gpio_ll_write(gpio_port_t port, uword_t value)
{
    static_assert(GPIO_PORT_NUMOF < 3);
    volatile uword_t *out = (uint32_t *)GPIO_OUT_REG;
#if GPIO_PORT_NUM > 1
    if (GPIO_PORT_NUM(port) != 0) {
        out = (uint32_t *)GPIO_OUT1_REG;
    }
#endif
    *out = value;
}

static inline gpio_port_t gpio_get_port(gpio_t pin)
{
    return GPIO_PORT(pin >> 5);
}

static inline uint8_t gpio_get_pin_num(gpio_t pin)
{
    return pin & 0x1f;
}

static inline gpio_port_t gpio_port_pack_addr(void *addr)
{
    return (gpio_port_t)addr;
}

static inline void * gpio_port_unpack_addr(gpio_port_t port)
{
    if (port <= 1) {
        return NULL;
    }

    return (void *)port;
}

static inline bool is_gpio_port_num_valid(uint_fast8_t num)
{
    return (num < GPIO_PORT_NUMOF);
}

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* GPIO_LL_ARCH_H */
