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
#include "soc/soc.h"
#include "soc/gpio_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

#define GPIO_PORT(num)          ((gpio_port_t)(&_esp32_ports[num]))
#define GPIO_PORT_NUM(port)     (((_esp32_port_t*)port == _esp32_ports) ? 0 : 1)

/* GPIO port descriptor type */
typedef struct {
    volatile uint32_t *out;         /* address of GPIO_OUT/GPIO_OUT1 */
    volatile uint32_t *out_w1ts;    /* address of GPIO_OUT_W1TS/GPIO_OUT1_W1TC */
    volatile uint32_t *out_w1tc;    /* address of GPIO_OUT_W1TC/GPIO_OUT1_W1TC */
    volatile uint32_t *in;          /* address of GPIO_IN/GPIO_IN1 */
    volatile uint32_t *enable;      /* address of GPIO_ENABLE/GPIO_ENABLE1 */
    volatile uint32_t *enable_w1ts; /* address of GPIO_ENABLE_W1TS/GPIO_ENABLE1_W1TS */
    volatile uint32_t *enable_w1tc; /* address of GPIO_ENABLE_W1TC/GPIO_ENABLE1_W1TC */
    volatile uint32_t *status_w1tc; /* address of GPIO_STATUS_W1TC/GPIO_STATUS1_W1TC */
}  _esp32_port_t;

/* GPIO port table */
extern const _esp32_port_t _esp32_ports[];

static inline uword_t gpio_ll_read(gpio_port_t port)
{
    /* return 0 for unconfigured pins, the current level at the pin otherwise */
    const _esp32_port_t *p = (_esp32_port_t *)port;
    return *p->in;
}

static inline uword_t gpio_ll_read_output(gpio_port_t port)
{
    /* return output register bits */
    const _esp32_port_t *p = (_esp32_port_t *)port;
    return *p->out;
}

static inline void gpio_ll_set(gpio_port_t port, uword_t mask)
{
    /* set output register bits for configured pins in the mask */
    const _esp32_port_t *p = (_esp32_port_t *)port;
    *p->out_w1ts = mask;
}

static inline void gpio_ll_clear(gpio_port_t port, uword_t mask)
{
    /* clear output register bits for configured pins in the mask */
    const _esp32_port_t *p = (_esp32_port_t *)port;
    *p->out_w1tc = mask;
}

static inline void gpio_ll_toggle(gpio_port_t port, uword_t mask)
{
    /* toggle output register bits for configured pins in the mask */
    const _esp32_port_t *p = (_esp32_port_t *)port;
    *p->out ^= mask;
}

static inline void gpio_ll_write(gpio_port_t port, uword_t value)
{
    /* write output register bits for configured pins in the mask */
    const _esp32_port_t *p = (_esp32_port_t *)port;
    *p->out = value;
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
    const _esp32_port_t *p = (_esp32_port_t *)port;

    /* return NULL if port is one of the port descriptors in GPIO port table */
#if GPIO_PORT_NUMOF > 1
    return ((p == &_esp32_ports[0]) ||
            (p == &_esp32_ports[1])) ? NULL : (void *)port;
#else
    return (p == _esp32_ports) ? NULL : (void *)port;
#endif
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
