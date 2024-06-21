/*
 * Copyright (C) 2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
 */

#ifndef GPIO_LL_ARCH_H
#define GPIO_LL_ARCH_H

#include "architecture.h"
#include "periph/gpio_ll.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

/**
 * @brief   Number of ports available on GD32VF103
 */
#define GPIO_PORT_NUMOF 5

#ifdef GPIOA_BASE
#  define GPIO_PORT_A       GPIOA_BASE
#  define GPIO_PORT_0       GPIO_PORT_A
#endif

#ifdef GPIOB_BASE
#  define GPIO_PORT_B       GPIOB_BASE
#  define GPIO_PORT_1       GPIO_PORT_B
#endif

#ifdef GPIOC_BASE
#  define GPIO_PORT_C       GPIOC_BASE
#  define GPIO_PORT_2       GPIO_PORT_C
#endif

#ifdef GPIOD_BASE
#  define GPIO_PORT_D       GPIOD_BASE
#  define GPIO_PORT_3       GPIO_PORT_D
#endif

#ifdef GPIOE_BASE
#  define GPIO_PORT_E       GPIOE_BASE
#  define GPIO_PORT_4       GPIO_PORT_E
#endif

#ifdef GPIOF_BASE
#  define GPIO_PORT_F       GPIOF_BASE
#  define GPIO_PORT_5       GPIO_PORT_F
#endif

#ifdef GPIOG_BASE
#  define GPIO_PORT_G       GPIOG_BASE
#  define GPIO_PORT_6       GPIO_PORT_G
#endif

#ifdef GPIOH_BASE
#  define GPIO_PORT_H       GPIOH_BASE
#  define GPIO_PORT_7       GPIO_PORT_H
#endif

#ifdef GPIOI_BASE
#  define GPIO_PORT_I       GPIOI_BASE
#  define GPIO_PORT_8       GPIO_PORT_I
#endif

#ifdef GPIOJ_BASE
#  define GPIO_PORT_J       GPIOJ_BASE
#  define GPIO_PORT_9       GPIO_PORT_J
#endif

#ifdef GPIOK_BASE
#  define GPIO_PORT_K       GPIOK_BASE
#  define GPIO_PORT_10      GPIO_PORT_K
#endif

static inline gpio_port_t gpio_port(uword_t num)
{
#if defined(CPU_FAM_STM32MP1)
    return GPIOA_BASE + (num << 12);
#else
    return GPIOA_BASE + (num << 10);
#endif
}

static inline uword_t gpio_port_num(gpio_port_t port)
{
#if defined(CPU_FAM_STM32MP1)
    return (port - GPIOA_BASE) >> 12;
#else
    return (port - GPIOA_BASE) >> 10;
#endif
}

static inline uword_t gpio_ll_read(gpio_port_t port)
{
    return ((GPIO_Type *)port)->ISTAT;
}

static inline uword_t gpio_ll_read_output(gpio_port_t port)
{
    return ((GPIO_Type *)port)->OCTL;
}

static inline void gpio_ll_set(gpio_port_t port, uword_t mask)
{
    ((GPIO_Type *)port)->BOP = mask;
}

static inline void gpio_ll_clear(gpio_port_t port, uword_t mask)
{
    ((GPIO_Type *)port)->BOP = mask << 16;
}

static inline void gpio_ll_toggle(gpio_port_t port, uword_t mask)
{
    unsigned irq_state = irq_disable();
    ((GPIO_Type *)port)->OCTL ^= mask;
    irq_restore(irq_state);
}

static inline void gpio_ll_write(gpio_port_t port, uword_t value)
{
    ((GPIO_Type *)port)->OCTL = value;
}

static inline gpio_port_t gpio_get_port(gpio_t pin)
{
    return pin & 0xfffffff0UL;
}

static inline uint8_t gpio_get_pin_num(gpio_t pin)
{
    return pin & 0xfUL;
}

static inline gpio_port_t gpio_port_pack_addr(void *addr)
{
    return (gpio_port_t)addr;
}

static inline void * gpio_port_unpack_addr(gpio_port_t port)
{
    if (port < GPIOA_BASE) {
        return (void *)port;
    }

    return NULL;
}

static inline bool is_gpio_port_num_valid(uint_fast8_t num)
{
    return num < GPIO_PORT_NUMOF;
}

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* GPIO_LL_ARCH_H */
/** @} */
