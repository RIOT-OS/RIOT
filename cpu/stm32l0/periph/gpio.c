/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l0
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"

/**
 * @brief   The STM32L0 family has 16 external interrupt lines
 */
#define EXTI_NUMOF          (16U)

/**
 * @brief   Allocate memory for one callback and argument per EXTI channel
 */
static gpio_isr_ctx_t isr_ctx[EXTI_NUMOF];

/**
 * @brief   Extract the port base address from the given pin identifier
 */
static inline GPIO_TypeDef *_port(gpio_t pin)
{
    return (GPIO_TypeDef *)(pin & ~(0x0f));
}

/**
 * @brief   Extract the port number form the given identifier
 *
 * The port number is extracted by looking at bits 10, 11, 12, 13 of the base
 * register addresses.
 */
static inline int _port_num(gpio_t pin)
{
    return ((pin >> 10) & 0x0f);
}

/**
 * @brief   Extract the pin number from the last 4 bit of the pin identifier
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    GPIO_TypeDef *port = _port(pin);
    int pin_num = _pin_num(pin);

    /* enable clock */
    periph_clk_en(IOP, (RCC_IOPENR_GPIOAEN << _port_num(pin)));

    /* set mode */
    port->MODER &= ~(0x3 << (2 * pin_num));
    port->MODER |=  ((mode & 0x3) << (2 * pin_num));
    /* set pull resistor configuration */
    port->PUPDR &= ~(0x3 << (2 * pin_num));
    port->PUPDR |=  (((mode >> 2) & 0x3) << (2 * pin_num));
    /* set output mode */
    port->OTYPER &= ~(1 << pin_num);
    port->OTYPER |=  (((mode >> 4) & 0x1) << pin_num);
    /* finally set pin speed to maximum and reset output */
    port->OSPEEDR |= (3 << (2 * pin_num));
    port->BRR = (1 << pin_num);

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    int pin_num = _pin_num(pin);
    int port_num = _port_num(pin);

    /* set callback */
    isr_ctx[pin_num].cb = cb;
    isr_ctx[pin_num].arg = arg;

    /* enable clock of the SYSCFG module for EXTI configuration */
    periph_clk_en(APB2, RCC_APB2ENR_SYSCFGEN);

    /* initialize pin as input */
    gpio_init(pin, mode);

    /* enable global pin interrupt */
    if (pin_num < 2) {
        NVIC_EnableIRQ(EXTI0_1_IRQn);
    }
    else if (pin_num < 4) {
        NVIC_EnableIRQ(EXTI2_3_IRQn);
    }
    else {
        NVIC_EnableIRQ(EXTI4_15_IRQn);
    }
    /* configure the active edge(s) */
    switch (flank) {
        case GPIO_RISING:
            EXTI->RTSR |= (1 << pin_num);
            EXTI->FTSR &= ~(1 << pin_num);
            break;
        case GPIO_FALLING:
            EXTI->RTSR &= ~(1 << pin_num);
            EXTI->FTSR |= (1 << pin_num);
            break;
        case GPIO_BOTH:
            EXTI->RTSR |= (1 << pin_num);
            EXTI->FTSR |= (1 << pin_num);
            break;
    }
    /* enable specific pin as exti sources */
    SYSCFG->EXTICR[pin_num >> 2] &= ~(0xf << ((pin_num & 0x03) * 4));
    SYSCFG->EXTICR[pin_num >> 2] |= (port_num << ((pin_num & 0x03) * 4));
    /* clear any pending requests */
    EXTI->PR = (1 << pin_num);
    /* enable interrupt for EXTI line */
    EXTI->IMR |= (1 << pin_num);
    return 0;
}

void gpio_init_af(gpio_t pin, gpio_af_t af)
{
    GPIO_TypeDef *port = _port(pin);
    uint32_t pin_num = _pin_num(pin);

    /* set pin to AF mode */
    port->MODER &= ~(3 << (2 * pin_num));
    port->MODER |= (2 << (2 * pin_num));
    /* set selected function */
    port->AFR[(pin_num > 7) ? 1 : 0] &= ~(0xf << ((pin_num & 0x07) * 4));
    port->AFR[(pin_num > 7) ? 1 : 0] |= (af << ((pin_num & 0x07) * 4));
}

void gpio_init_analog(gpio_t pin)
{
    /* enable clock, needed as this function can be used without calling
     * gpio_init first */
    periph_clk_en(IOP, (RCC_IOPENR_GPIOAEN << _port_num(pin)));
    /* set to analog mode */
    _port(pin)->MODER |= (0x3 << (2 * _pin_num(pin)));
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
    if (_port(pin)->MODER & (0x3 << (_pin_num(pin) * 2))) {
        return _port(pin)->ODR & (1 << _pin_num(pin));
    }
    else {
        return _port(pin)->IDR & (1 << _pin_num(pin));
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
        _port(pin)->BRR = (1 << _pin_num(pin));
    } else {
        _port(pin)->BSRR = (1 << _pin_num(pin));
    }
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        _port(pin)->BSRR = (1 << _pin_num(pin));
    } else {
        _port(pin)->BRR = (1 << _pin_num(pin));
    }
}

void isr_exti(void)
{
    /* only generate interrupts against lines which have their IMR set */
    uint32_t pending_isr = (EXTI->PR & EXTI->IMR);
    for (size_t i = 0; i < EXTI_NUMOF; i++) {
        if (pending_isr & (1 << i)) {
            EXTI->PR = (1 << i);        /* clear by writing a 1 */
            isr_ctx[i].cb(isr_ctx[i].arg);
        }
    }
    cortexm_isr_end();
}
