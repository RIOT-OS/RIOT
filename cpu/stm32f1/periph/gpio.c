/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f1
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "thread.h"
#include "sched.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Number of available external interrupt lines
 */
#define GPIO_ISR_CHAN_NUMOF             (16U)

/**
 * @brief   Extract information from mode parameter
 */
#define MODE_MASK                       (0x0f)
#define ODR_POS                         (4U)

/**
 * @brief   Allocate memory for one callback and argument per EXTI channel
 */
static gpio_isr_ctx_t exti_ctx[GPIO_ISR_CHAN_NUMOF];

/**
 * @brief   Extract the pin's port base address from the given pin identifier
 */
static inline GPIO_TypeDef *_port(gpio_t pin)
{
    return (GPIO_TypeDef *)(pin & ~(0x0f));
}

/**
 * @brief   Extract the port number from the given pin identifier
 *
 * Isolating bits 10 to 13 of the port base addresses leads to unique port
 * numbers.
 */
static inline int _port_num(gpio_t pin)
{
    return (((pin >> 10) & 0x0f) - 2);
}

/**
 * @brief   Get the pin number from the pin identifier, encoded in the LSB 4 bit
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}


int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    GPIO_TypeDef *port = _port(pin);
    int pin_num = _pin_num(pin);

    /* open-drain output with pull-up is not supported */
    if (mode == GPIO_OD_PU) {
        return -1;
    }

    /* enable the clock for the selected port */
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN << _port_num(pin));

    /* set pin mode */
    port->CR[pin_num >> 3] &= ~(0xf << ((pin_num & 0x7) * 4));
    port->CR[pin_num >> 3] |=  ((mode & MODE_MASK) << ((pin_num & 0x7) * 4));
    /* set initial state of output register */
    port->BRR = (1 << pin_num);
    port->BSRR = ((mode >> ODR_POS) << pin_num);

    return 0; /* all OK */
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    int pin_num = _pin_num(pin);

    /* disable interrupts on the channel we want to edit (just in case) */
    EXTI->IMR &= ~(1 << pin_num);
    /* configure pin as input */
    gpio_init(pin, mode);
    /* set callback */
    exti_ctx[pin_num].cb = cb;
    exti_ctx[pin_num].arg = arg;
    /* enable alternate function clock for the GPIO module */
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    /* configure the EXTI channel */
    AFIO->EXTICR[pin_num >> 2] &= ~(0xf << ((pin_num & 0x3) * 4));
    AFIO->EXTICR[pin_num >> 2] |=  (_port_num(pin) << ((pin_num & 0x3) * 4));
    /* configure the active flank */
    EXTI->RTSR &= ~(1 << pin_num);
    EXTI->RTSR |=  ((flank & 0x1) << pin_num);
    EXTI->FTSR &= ~(1 << pin_num);
    EXTI->FTSR |=  ((flank >> 1) << pin_num);
    /* active global interrupt for the selected port */
    if (pin_num < 5) {
        NVIC_EnableIRQ(EXTI0_IRQn + pin_num);
    }
    else if (pin_num < 10) {
        NVIC_EnableIRQ(EXTI9_5_IRQn);
    }
    else {
        NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
    /* clear event mask */
    EXTI->EMR &= ~(1 << pin_num);
    /* unmask the pins interrupt channel */
    EXTI->IMR |= (1 << pin_num);
    return 0;
}

void gpio_init_af(gpio_t pin, gpio_af_out_t af)
{
    int pin_num = _pin_num(pin);
    GPIO_TypeDef *port = _port(pin);

    /* enable the clock for the selected port */
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN << _port_num(pin));
    /* configure the pin */
    port->CR[pin_num >> 3] &= ~(0xf << ((pin_num & 0x7) * 4));
    port->CR[pin_num >> 3] |=  (af << ((pin_num & 0x7) * 4));
}

void gpio_init_analog(gpio_t pin)
{
    /* enable the GPIO port RCC */
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN << _port_num(pin));

    /* map the pin as analog input */
    int pin_num = _pin_num(pin);
    _port(pin)->CR[pin_num >= 8] &= ~(0xfl << (4 * (pin_num - ((pin_num >= 8) * 8))));
}

void gpio_irq_enable(gpio_t pin)
{
    EXTI->IMR |= (1 << _pin_num(pin));
}

void gpio_irq_disable(gpio_t pin)
{
    EXTI->IMR &= ~(1 << _pin_num(pin));
}

int gpio_read(gpio_t pin)
{
    GPIO_TypeDef *port = _port(pin);
    int pin_num = _pin_num(pin);

    if (port->CR[pin_num >> 3] & (0x3 << ((pin_num & 0x7) << 2))) {
        /* pin is output */
        return (port->ODR & (1 << pin_num));
    }
    else {
        /* or input */
        return (port->IDR & (1 << pin_num));
    }
}

void gpio_set(gpio_t pin)
{
    _port(pin)->BSRR = (1 << _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->BRR = (1 << _pin_num(pin));
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        gpio_clear(pin);
    }
    else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        _port(pin)->BSRR = (1 << _pin_num(pin));
    }
    else {
        _port(pin)->BRR = (1 << _pin_num(pin));
    }
}

void isr_exti(void)
{
    /* only generate interrupts against lines which have their IMR set */
    uint32_t pending_isr = (EXTI->PR & EXTI->IMR);
    for (unsigned i = 0; i < GPIO_ISR_CHAN_NUMOF; i++) {
        if (pending_isr & (1 << i)) {
            EXTI->PR = (1 << i);        /* clear by writing a 1 */
            exti_ctx[i].cb(exti_ctx[i].arg);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
