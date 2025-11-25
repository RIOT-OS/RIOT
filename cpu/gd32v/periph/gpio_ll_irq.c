/*
 * Copyright (C) 2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_gd32v
 * @ingroup     drivers_periph_gpio_ll_irq
 * @{
 *
 * @file
 * @brief       IRQ implementation of the GPIO Low-Level API for GD32V
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <errno.h>

#include "cpu.h"
#include "bitarithm.h"
#include "periph/gpio_ll_irq.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Number of available external interrupt lines
 */
#define GPIO_ISR_CHAN_NUMOF             (16U)
#define GPIO_ISR_CHAN_MASK              (0xFFFF)

struct _gpio_isr_ctx {
    gpio_ll_cb_t cb;
    void *arg;
};

/**
 * @brief   Allocate memory for one callback and argument per EXTI channel
 */
static struct _gpio_isr_ctx _exti_ctx[GPIO_ISR_CHAN_NUMOF];

/* keep the state of level triggered interrupt activation using 32-bit values
 * since the registers require 32-bit access */
static uint32_t _h_level_triggered = 0;
static uint32_t _l_level_triggered = 0;

void gpio_ll_irq_mask(gpio_port_t port, uint8_t pin)
{
    (void)port;
    EXTI->INTEN &= ~(1 << pin);
}

void gpio_ll_irq_unmask_and_clear(gpio_port_t port, uint8_t pin)
{
    (void)port;
    EXTI->PD = (1 << pin);
    EXTI->INTEN |= (1 << pin);
}

static inline unsigned _irq_num(unsigned pin)
{
    if (pin < 5) {
        return EXTI0_IRQn + pin;
    }
    if (pin < 10) {
        return EXTI5_9_IRQn;
    }
    return EXTI10_15_IRQn;
}

/* Forward declaration of ISR */
static void _gpio_isr(unsigned irqn);

int gpio_ll_irq(gpio_port_t port, uint8_t pin, gpio_irq_trig_t trig, gpio_ll_cb_t cb, void *arg)
{
    unsigned irq_state = irq_disable();
    int port_num = GPIO_PORT_NUM(port);

    /* set callback */
    _exti_ctx[pin].cb = cb;
    _exti_ctx[pin].arg = arg;

    /* enable alternate function clock for the GPIO module */
    periph_clk_en(APB2, RCU_APB2EN_AFEN_Msk);

    /* configure the EXTI channel */
    volatile uint32_t *afio_exti_ss = &AFIO->EXTISS0 + (pin >> 2);

    *afio_exti_ss &= ~(0xfUL << ((pin & 0x03) * 4));
    *afio_exti_ss |= (uint32_t)port_num << ((pin & 0x03) * 4);

    uint32_t pin_mask = 1UL << pin;

    _h_level_triggered &= ~pin_mask;
    _l_level_triggered &= ~pin_mask;
    EXTI->RTEN &= ~pin_mask;
    EXTI->FTEN &= ~pin_mask;

    /* configure the active flank, level interrupts are emulated */
    switch (trig) {
    case GPIO_TRIGGER_EDGE_RISING:
        EXTI->RTEN |= pin_mask;
        break;
    case GPIO_TRIGGER_EDGE_FALLING:
        EXTI->FTEN |= pin_mask;
        break;
    case GPIO_TRIGGER_EDGE_BOTH:
        EXTI->RTEN |= pin_mask;
        EXTI->FTEN |= pin_mask;
        break;
    case GPIO_TRIGGER_LEVEL_HIGH:
        EXTI->RTEN |= pin_mask;
        _h_level_triggered |= pin_mask;
        break;
    case GPIO_TRIGGER_LEVEL_LOW:
        EXTI->FTEN |= pin_mask;
        _l_level_triggered |= pin_mask;
        break;
    }

    /* clear pending interrupts */
    EXTI->PD = pin_mask;

    /* enable global pin interrupt */
    unsigned irqn = _irq_num(pin);

    clic_set_handler(irqn, _gpio_isr);
    clic_enable_interrupt(irqn, CPU_DEFAULT_IRQ_PRIO);

    /* unmask the pins interrupt channel */
    EXTI->INTEN |= pin_mask;

    /* emulate a level interrupt if the pin already has the level */
    uint32_t level = gpio_ll_read(port) & pin_mask;

    if ((trig == GPIO_TRIGGER_LEVEL_HIGH) && level) {
        EXTI->SWIEV |= pin_mask;
    }
    else if ((trig == GPIO_TRIGGER_LEVEL_LOW) && !level) {
        EXTI->SWIEV |= pin_mask;
    }

    irq_restore(irq_state);

    return 0;
}

static void _gpio_isr(unsigned irqn)
{
    (void)irqn;

    /* read pending interrupts */
    uint32_t pending = (EXTI->PD & GPIO_ISR_CHAN_MASK);

    /* clear pending interrupts */
    EXTI->PD = pending;

    /* generate soft interrupts for lines which have their interrupt enabled */
    pending &= EXTI->INTEN;

    /* iterate over all set bits */
    uint8_t pin = 0;

    while (pending) {
        /* get next pin with interrupt */
        pending = bitarithm_test_and_clear(pending, &pin);
        /* call registered callball function */
        _exti_ctx[pin].cb(_exti_ctx[pin].arg);

        /* emulate level triggered IRQs by asserting the IRQ again in software */
        uint32_t pin_mask = 1UL << pin;

        if ((_h_level_triggered & pin_mask) || (_l_level_triggered & pin_mask)) {
            /* determine the port of triggered interrupt */
            volatile uint32_t *afio_exti_ss = &AFIO->EXTISS0 + (pin >> 2);
            gpio_port_t port = GPIO_PORT(((*afio_exti_ss >> ((pin & 0x03) * 4)) & 0xfUL));

            /* trigger software interrupt if the pin has the according level */
            uint32_t level = gpio_ll_read(port) & pin_mask;
            if ((_h_level_triggered & pin_mask)  && level) {
                EXTI->SWIEV |= pin_mask;
            }
            else if ((_l_level_triggered & pin_mask)  && !level) {
                EXTI->SWIEV |= pin_mask;
            }
        }
    }
}
