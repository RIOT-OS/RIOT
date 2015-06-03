/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f4
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"

/**
 * @brief   Number of available external interrupt lines
 */
#define GPIO_ISR_CHAN_NUMOF             (16U)

/**
 * @brief   Datastructure to hold an interrupt context
 */
typedef struct {
    void (*cb)(void *arg);      /**< interrupt callback routine */
    void *arg;                  /**< optional argument */
} exti_ctx_t;

/**
 * @brief   Hold one callback function pointer for each interrupt line
 */
static exti_ctx_t exti_chan[GPIO_ISR_CHAN_NUMOF];

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

int gpio_init(gpio_t pin, gpio_dir_t dir, gpio_pp_t pullup)
{
    GPIO_TypeDef *port = _port(pin);
    int pin_num = _pin_num(pin);

    /* enable clock */
    RCC->AHB1ENR |= (1 << _port_num(pin));
    /* configure pull register */
    port->PUPDR &= ~(3 << (2 * pin_num));
    port->PUPDR |= (pullup << (2 * pin_num));
    /* set direction */
    if (dir == GPIO_DIR_OUT) {
        port->MODER &= ~(2 << (2 * pin_num));   /* set pin to output mode */
        port->MODER |= (1 << (2 * pin_num));
        port->OTYPER &= ~(1 << pin_num);        /* set to push-pull */
        port->OSPEEDR |= (3 << (2 * pin_num));  /* set to high speed */
        port->ODR &= ~(1 << pin_num);           /* set pin to low signal */
    }
    else {
        port->MODER &= ~(3 << (2 * pin_num));   /* configure pin as input */
    }
    return 0;
}

int gpio_init_exti(gpio_t pin,
                   gpio_pp_t pullup, gpio_flank_t flank,
                   gpio_cb_t cb, void *arg)
{
    int pin_num = _pin_num(pin);
    int port_num = _port_num(pin);

    /* configure and save exti configuration struct */
    exti_chan[pin_num].cb = cb;
    exti_chan[pin_num].arg = arg;
    /* enable the SYSCFG clock */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    /* initialize pin as input */
    gpio_init(pin, GPIO_DIR_IN, pullup);
    /* enable global pin interrupt */
    if (pin_num < 5) {
        NVIC_EnableIRQ(EXTI0_IRQn + pin_num);
    }
    else if (pin_num < 10) {
        NVIC_EnableIRQ(EXTI9_5_IRQn);
    }
    else {
        NVIC_EnableIRQ(EXTI15_10_IRQn);
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
    SYSCFG->EXTICR[pin_num >> 2] &= ~(0xf << (pin_num & 0x03));
    SYSCFG->EXTICR[pin_num >> 2] |= (port_num << (pin_num & 0x03));
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
    port->AFR[pin_num & 0x10] &= ~(0xf << ((pin_num & 0x0f) * 4));
    port->AFR[pin_num & 0x10] |= (af << ((pin_num & 0x0f) * 4));
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
    uint32_t pin_num = _pin_num(pin);

    if (port->MODER & (3 << (pin_num * 2))) {   /* if configured as output */
        return port->ODR & (1 << pin_num);      /* read output data reg */
    } else {
        return port->IDR & (1 << pin_num);      /* else read input data reg */
    }
}

void gpio_set(gpio_t pin)
{
    _port(pin)->BSRRL = (1 << _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->BSRRH = (1 << _pin_num(pin));
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        gpio_clear(pin);
    } else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        gpio_set(pin);
    } else {
        gpio_clear(pin);
    }
}

void isr_exti(void)
{
    for (int i = 0; i << GPIO_ISR_CHAN_NUMOF; i++) {
        if (EXTI->PR & (1 << i)) {
            EXTI->PR |= (1 << i);               /* clear by writing a 1 */
            exti_chan[i].cb(exti_chan[i].arg);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
