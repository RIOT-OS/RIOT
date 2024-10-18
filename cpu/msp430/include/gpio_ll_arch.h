/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430
 * @ingroup     drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief       CPU specific part of the Peripheral GPIO Low-Level API
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef GPIO_LL_ARCH_H
#define GPIO_LL_ARCH_H

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

/* the memory layout of all GPIO peripherals is compatible, but the location
 * in the address space is pretty much random */

#define GPIO_PORT_1     ((gpio_port_t)&PORT_1.base)
#define GPIO_PORT_2     ((gpio_port_t)&PORT_2.base)
#define GPIO_PORT_3     ((gpio_port_t)&PORT_3.base)
#define GPIO_PORT_4     ((gpio_port_t)&PORT_4.base)
#define GPIO_PORT_5     ((gpio_port_t)&PORT_5.base)
#define GPIO_PORT_6     ((gpio_port_t)&PORT_6.base)
/* Port 7 and 8 have different memory layout and are only available on F2xx/G2xx
 * MCUs */
#if defined(CPU_FAM_MSP430_F2XX_G2XX)
#  define GPIO_PORT_7   ((gpio_port_t)&PORT_7)
#  define GPIO_PORT_8   ((gpio_port_t)&PORT_8)
#endif

/* IMPORTANT IMPLEMENTATION INFO
 * =============================
 *
 * - MSP430 F2xx/G2xx do have PORT 7 and PORT 8, but those have an incompatible
 *   memory layout compared to the other ports. Hence, they need extra handling.
 *   However, constant folding should get ride of the branch and overhead if the
 *   GPIO port is a compile time constant
 * - MSP430 has bit manipulation instructions that work on memory. E.g.
 *   `BIC.B %[mask], @%[ptr]` will implement `*ptr &= ~(mask)` in a single
 *   instruction. Same for setting or XORing bits. Hence, the code below
 *   may often look like it is missing `irq_disable()` ... `irq_restore()`, but
 *   in fact will be atomic due to the MSP430 instruction set.
 */

gpio_port_t gpio_port(uword_t num);

static inline uword_t gpio_ll_read(gpio_port_t port)
{
#if defined(CPU_FAM_MSP430_F2XX_G2XX)
    if (port >= (uintptr_t)(&PORT_7)) {
        const msp430_port_p7_p8_t *p = (void *)port;
        return p->IN;
    }
#endif
    const msp430_port_t *p = (void *)port;
    return p->IN;
}

static inline uword_t gpio_ll_read_output(gpio_port_t port)
{
#if defined(CPU_FAM_MSP430_F2XX_G2XX)
    if (port >= (uintptr_t)(&PORT_7)) {
        const msp430_port_p7_p8_t *p = (void *)port;
        return p->OD;
    }
#endif
    const msp430_port_t *p = (void *)port;
    return p->OD;
}

static inline void gpio_ll_set(gpio_port_t port, uword_t mask)
{
#if defined(CPU_FAM_MSP430_F2XX_G2XX)
    if (port >= (uintptr_t)(&PORT_7)) {
        msp430_port_p7_p8_t *p = (void *)port;
        p->OD |= mask;
        return;
    }
#endif
    msp430_port_t *p = (void *)port;
    p->OD |= mask;
}

static inline void gpio_ll_clear(gpio_port_t port, uword_t mask)
{
#if defined(CPU_FAM_MSP430_F2XX_G2XX)
    if (port >= (uintptr_t)(&PORT_7)) {
        msp430_port_p7_p8_t *p = (void *)port;
        p->OD &= ~(mask);
        return;
    }
#endif
    msp430_port_t *p = (void *)port;
    p->OD &= ~(mask);
}

static inline void gpio_ll_toggle(gpio_port_t port, uword_t mask)
{
#if defined(CPU_FAM_MSP430_F2XX_G2XX)
    if (port >= (uintptr_t)(&PORT_7)) {
        msp430_port_p7_p8_t *p = (void *)port;
        p->OD ^= mask;
        return;
    }
#endif
    msp430_port_t *p = (void *)port;
    p->OD ^= mask;
}

static inline void gpio_ll_write(gpio_port_t port, uword_t value)
{
#if defined(CPU_FAM_MSP430_F2XX_G2XX)
    if (port >= (uintptr_t)(&PORT_7)) {
        msp430_port_p7_p8_t *p = (void *)port;
        p->OD = value;
        return;
    }
#endif
    msp430_port_t *p = (void *)port;
    p->OD = value;
}

static inline gpio_port_t gpio_get_port(gpio_t pin)
{
    return gpio_port(gpio_get_pin_num(pin));
}

static inline uint8_t gpio_get_pin_num(gpio_t pin)
{
    return pin >> 8;
}

static inline void gpio_ll_switch_dir_output(gpio_port_t port, uword_t outputs)
{
#if defined(CPU_FAM_MSP430_F2XX_G2XX)
    if (port >= (uintptr_t)(&PORT_7)) {
        msp430_port_p7_p8_t *p = (void *)port;
        p->DIR |= outputs;
        return;
    }
#endif
    msp430_port_t *p = (void *)port;
    p->DIR |= outputs;
}

static inline void gpio_ll_switch_dir_input(gpio_port_t port, uword_t inputs)
{
#if defined(CPU_FAM_MSP430_F2XX_G2XX)
    if (port >= (uintptr_t)(&PORT_7)) {
        msp430_port_p7_p8_t *p = (void *)port;
        p->DIR &= ~(inputs);
        return;
    }
#endif
    msp430_port_t *p = (void *)port;
    p->DIR &= ~(inputs);
}

static inline gpio_port_t gpio_port_pack_addr(void *addr)
{
    return (gpio_port_t)addr;
}

static inline void * gpio_port_unpack_addr(gpio_port_t port)
{
    if (port < RAMSTART) {
        return NULL;
    }

    return (void *)port;
}

static inline bool is_gpio_port_num_valid(uint_fast8_t num)
{
#if defined(CPU_FAM_MSP430_F2XX_G2XX)
    return (num > 0) && (num <= 8);
#else
    return (num > 0) && (num <= 6);
#endif
}

uword_t gpio_port_num(gpio_port_t port);

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_LL_ARCH_H */
