/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 INRIA
 *               2022 Otto-von-Guericke-Universität Magdeburg
 *               2023 Gerson Fernando Budke
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_gpio_ll_irq
 * @{
 *
 * @file
 * @brief       IRQ implementation of the GPIO Low-Level API for ATmega
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 * @author      Torben Petersen <petersen@ibr.cs.tu-bs.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include <avr/interrupt.h>
#include <errno.h>

#include "cpu.h"
#include "irq.h"
#include "periph/gpio_ll_irq.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG 0
#include "debug.h"

struct isr_ctx {
    gpio_ll_cb_t cb;
    void *arg;
};

static struct isr_ctx isr_ctx[GPIO_EXT_INT_NUMOF];

static void clear_pending_irqs(uint8_t exti)
{
#if defined(EIFR)
    EIFR |= 1 << exti;
#elif defined(GIFR)
    GIFR |= 1 << (INTF0 + exti);
#else
#  error "No support for AVR with neither EIFR nor GIFR"
#endif
}

void gpio_ll_irq_mask(gpio_port_t port, uint8_t pin)
{
    uint8_t exti = atmega_pin2exti(gpio_port_num(port), pin);
#if defined(EIMSK)
    EIMSK &= ~(1 << exti);
#elif defined(GICR)
    GICR &= ~(1 << (INT0 + exti));
#endif
}

void gpio_ll_irq_unmask(gpio_port_t port, uint8_t pin)
{
    uint8_t exti = atmega_pin2exti(gpio_port_num(port), pin);
#if defined(EIMSK)
    EIMSK |= 1 << exti;
#elif defined(GICR)
    GICR |= 1 << (INT0 + exti);
#endif
}

void gpio_ll_irq_unmask_and_clear(gpio_port_t port, uint8_t pin)
{
    uint8_t exti = atmega_pin2exti(gpio_port_num(port), pin);
    clear_pending_irqs(exti);
#if defined(EIMSK)
    EIMSK |= 1 << exti;
#elif defined(GICR)
    GICR |= 1 << (INT0 + exti);
#endif
}

static void set_trigger(uint8_t exti, gpio_irq_trig_t trig)
{
    exti <<= 1;
#if defined(EICRA)
    volatile uint8_t *eicr = &EICRA;
#elif defined(MCUCR)
    volatile uint8_t *eicr = &MCUCR;
#endif

#ifdef EICRB
    if (exti >= 8) {
        eicr = & EICRB;
        exti -= 8;
    }
#endif

    /* being a bit more verbose here to avoid two read-modify-write cycles,
     * as the compiler won't optimize access to volatile memory */
    uint8_t tmp = *eicr;
    tmp &= ~(0x3 << exti);
    tmp |= trig << exti;
    *eicr = tmp;
}

int gpio_ll_irq(gpio_port_t port, uint8_t pin, gpio_irq_trig_t trig,
                gpio_ll_cb_t cb, void *arg)
{
    int port_num = gpio_port_num(port);
    assert((trig != GPIO_TRIGGER_LEVEL_HIGH) && cb);
    if (!atmega_has_pin_exti(port_num, pin)) {
        return -ENOTSUP;
    }

    uint8_t exti = atmega_pin2exti(port_num, pin);
    unsigned irq_state = irq_disable();

    /* set callback */
    isr_ctx[exti].cb = cb;
    isr_ctx[exti].arg = arg;

    /* setup IRQ */
    set_trigger(exti, trig);
    clear_pending_irqs(exti);
#if defined(EIMSK)
    EIMSK |= 1 << exti;
#elif defined(GICR)
    GICR |= 1 << (INT0 + exti);
#endif

    irq_restore(irq_state);

    return 0;
}

static void isr_exti(uint8_t exti)
{
    isr_ctx[exti].cb(isr_ctx[exti].arg);
}

#ifdef INT0_vect
AVR8_ISR(INT0_vect, isr_exti, 0);
#endif

#ifdef INT1_vect
AVR8_ISR(INT1_vect, isr_exti, 1);
#endif

#ifdef INT2_vect
AVR8_ISR(INT2_vect, isr_exti, 2);
#endif

#ifdef INT3_vect
AVR8_ISR(INT3_vect, isr_exti, 3);
#endif

#ifdef INT4_vect
AVR8_ISR(INT4_vect, isr_exti, 4);
#endif

#ifdef INT5_vect
AVR8_ISR(INT5_vect, isr_exti, 5);
#endif

#ifdef INT6_vect
AVR8_ISR(INT6_vect, isr_exti, 6);
#endif

#ifdef INT7_vect
AVR8_ISR(INT7_vect, isr_exti, 7);
#endif
