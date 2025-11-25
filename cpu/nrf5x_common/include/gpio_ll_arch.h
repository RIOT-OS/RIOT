/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *               2015-2016 Freie Universität Berlin
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @ingroup     drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief       CPU specific part of the Peripheral GPIO Low-Level API
 *
 * @note        This GPIO driver implementation supports only one pin to be
 *              defined as external interrupt.
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef GPIO_LL_ARCH_H
#define GPIO_LL_ARCH_H

#include <assert.h>

#include "cpu.h"
#include "irq.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

#define PORT_BIT            (1 << 5)
#define PIN_MASK            (0x1f)
#define NRF5X_IO_AREA_START (0x40000000UL)

/* Compatibility wrapper defines for nRF9160 */
#ifdef NRF_P0_S
#define NRF_P0 NRF_P0_S
#endif

#if defined(CPU_FAM_NRF51)
#define GPIO_PORT(num)      ((gpio_port_t)NRF_GPIO)
#define GPIO_PORT_NUM(port) 0
#elif defined(NRF_P1)
#define GPIO_PORT(num)      ((num) ? (gpio_port_t)NRF_P1 : (gpio_port_t)NRF_P0)
#define GPIO_PORT_NUM(port) ((port == (gpio_port_t)NRF_P1) ? 1 : 0)
#else
#define GPIO_PORT(num)      ((gpio_port_t)NRF_P0)
#define GPIO_PORT_NUM(port) 0
#endif

static inline uword_t gpio_ll_read(gpio_port_t port)
{
    NRF_GPIO_Type *p = (NRF_GPIO_Type *)port;
    return p->IN;
}

static inline uword_t gpio_ll_read_output(gpio_port_t port)
{
    NRF_GPIO_Type *p = (NRF_GPIO_Type *)port;
    return p->OUT;
}

static inline void gpio_ll_set(gpio_port_t port, uword_t mask)
{
    NRF_GPIO_Type *p = (NRF_GPIO_Type *)port;
    p->OUTSET = mask;
}

static inline void gpio_ll_clear(gpio_port_t port, uword_t mask)
{
    NRF_GPIO_Type *p = (NRF_GPIO_Type *)port;
    p->OUTCLR = mask;
}

static inline void gpio_ll_toggle(gpio_port_t port, uword_t mask)
{
    NRF_GPIO_Type *p = (NRF_GPIO_Type *)port;
    unsigned state = irq_disable();
    p->OUT ^= mask;
    irq_restore(state);
}

static inline void gpio_ll_write(gpio_port_t port, uword_t value)
{
    NRF_GPIO_Type *p = (NRF_GPIO_Type *)port;
    p->OUT = value;
}

static inline gpio_port_t gpio_get_port(gpio_t pin)
{
#if defined(NRF_P1)
    return GPIO_PORT(pin >> 5);
#else
    (void)pin;
    return GPIO_PORT(0);
#endif
}

static inline uint8_t gpio_get_pin_num(gpio_t pin)
{
#if defined(NRF_P1)
    return pin & PIN_MASK;
#else
    return (uint8_t)pin;
#endif
}

static inline gpio_port_t gpio_port_pack_addr(void *addr)
{
    return (gpio_port_t)addr;
}

static inline void * gpio_port_unpack_addr(gpio_port_t port)
{
    /* NRF5X_IO_AREA_START is the start of the memory mapped I/O area. Both data
     * and flash are mapped before it. So if it is an I/O address, it
     * cannot be a packed data address and (hopefully) is a GPIO port */
    if (port >= NRF5X_IO_AREA_START) {
        return NULL;
    }

    return (void *)port;
}

static inline bool is_gpio_port_num_valid(uint_fast8_t num)
{
    switch (num) {
    default:
        return false;
    case 0:
#if defined(NRF_P1)
    case 1:
#endif
        return true;
    }
}

#endif /* DOXYGEN */
#ifdef __cplusplus
}
#endif

#endif /* GPIO_LL_ARCH_H */
/** @} */
