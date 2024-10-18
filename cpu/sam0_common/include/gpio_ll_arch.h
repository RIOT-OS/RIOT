/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *               2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam0_common
 * @ingroup         drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief           CPU specific part of the Peripheral GPIO Low-Level API
 *
 * @author          Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef GPIO_LL_ARCH_H
#define GPIO_LL_ARCH_H

#include "architecture.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

/* Provide base address of the GPIO peripheral via APB */
#if defined(PORT_SEC)
#  define GPIO_APB_BASE PORT_SEC
#else
#  define GPIO_APB_BASE PORT
#endif

/* Provide base address of the GPIO peripheral via IOBUS */
#if defined(PORT_IOBUS_SEC)
#  define GPIO_IOBUS_BASE PORT_IOBUS_SEC
#elif defined(PORT_IOBUS)
#  define GPIO_IOBUS_BASE PORT_IOBUS
#else
#  define GPIO_IOBUS_BASE GPIO_APB_BASE /* no IOBUS present, fall back to APB */
#endif

#define GPIO_PORT_NUMBERING_ALPHABETIC  1

#if PORT_GROUPS >= 1
#  define GPIO_PORT_0   ((uintptr_t)&GPIO_IOBUS_BASE->Group[0])
#endif
#if PORT_GROUPS >= 2
#  define GPIO_PORT_1   ((uintptr_t)&GPIO_IOBUS_BASE->Group[1])
#endif
#if PORT_GROUPS >= 3
#  define GPIO_PORT_2   ((uintptr_t)&GPIO_IOBUS_BASE->Group[2])
#endif
#if PORT_GROUPS >= 4
#  define GPIO_PORT_3   ((uintptr_t)&GPIO_IOBUS_BASE->Group[3])
#endif
#if PORT_GROUPS >= 5
#  define GPIO_PORT_4   ((uintptr_t)&GPIO_IOBUS_BASE->Group[4])
#endif
#if PORT_GROUPS >= 5
#  define GPIO_PORT_4   ((uintptr_t)&GPIO_IOBUS_BASE->Group[4])
#endif
#if PORT_GROUPS >= 6
#  define GPIO_PORT_5   ((uintptr_t)&GPIO_IOBUS_BASE->Group[5])
#endif
#if PORT_GROUPS >= 7
#  define GPIO_PORT_6   ((uintptr_t)&GPIO_IOBUS_BASE->Group[6])
#endif
#if PORT_GROUPS >= 8
#  define GPIO_PORT_7   ((uintptr_t)&GPIO_IOBUS_BASE->Group[7])
#endif

/**
 * @brief   Get a GPIO port by number
 */
#define GPIO_PORT(num)  ((uintptr_t)&GPIO_IOBUS_BASE->Group[(num)])

/**
 * @brief   Get a GPIO port number by gpio_port_t value
 */
#define GPIO_PORT_NUM(port) \
    (((port) - (uintptr_t)&GPIO_IOBUS_BASE->Group[0]) / sizeof(GPIO_IOBUS_BASE->Group[0]))

static inline gpio_port_t gpio_port(uword_t num)
{
    return (uintptr_t)&GPIO_IOBUS_BASE->Group[num];
}

static inline uword_t gpio_port_num(gpio_port_t port)
{
    return (port - (uintptr_t)&GPIO_IOBUS_BASE->Group[0]) / sizeof(GPIO_IOBUS_BASE->Group[0]);
}

static inline PortGroup *sam0_gpio_iobus2ap(PortGroup *iobus)
{
    const uintptr_t iobus_base = (uintptr_t)GPIO_IOBUS_BASE;
    const uintptr_t apb_base = (uintptr_t)GPIO_APB_BASE;

    return (PortGroup *)((uintptr_t)iobus - (iobus_base - apb_base));
}

static inline uword_t gpio_ll_read(gpio_port_t port)
{
    PortGroup *p = (PortGroup *)port;
    if (!IS_USED(MODULE_PERIPH_GPIO_FAST_READ)) {
        p = sam0_gpio_iobus2ap(p);
    }
    return p->IN.reg;
}

static inline uword_t gpio_ll_read_output(gpio_port_t port)
{
    PortGroup *p = (PortGroup *)port;
    return p->OUT.reg;
}

static inline void gpio_ll_set(gpio_port_t port, uword_t mask)
{
    PortGroup *p = (PortGroup *)port;
    p->OUTSET.reg = mask;
}

static inline void gpio_ll_clear(gpio_port_t port, uword_t mask)
{
    PortGroup *p = (PortGroup *)port;
    p->OUTCLR.reg = mask;
}

static inline void gpio_ll_toggle(gpio_port_t port, uword_t mask)
{
    PortGroup *p = (PortGroup *)port;
    p->OUTTGL.reg = mask;
}

static inline void gpio_ll_write(gpio_port_t port, uword_t mask)
{
    PortGroup *p = (PortGroup *)port;
    p->OUT.reg = mask;
}

static inline void gpio_ll_switch_dir_output(gpio_port_t port, uword_t outputs)
{
    PortGroup *p = (PortGroup *)port;
    p->DIRSET.reg = outputs;
}

static inline void gpio_ll_switch_dir_input(gpio_port_t port, uword_t inputs)
{
    PortGroup *p = (PortGroup *)port;
    p->DIRCLR.reg = inputs;
}

static inline gpio_port_t gpio_get_port(gpio_t pin)
{
    return (gpio_port_t)(pin & ~(0x1f));
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
    if (port < GPIO_PORT(0)) {
        return (void *)port;
    }
    if (port > GPIO_PORT(ARRAY_SIZE(GPIO_IOBUS_BASE->Group))) {
        return (void *)port;
    }

    return NULL;
}

static inline bool is_gpio_port_num_valid(uint_fast8_t num)
{
    return (num < ARRAY_SIZE(GPIO_IOBUS_BASE->Group));
}

#endif /* DOXYGEN */
#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_LL_ARCH_H */
