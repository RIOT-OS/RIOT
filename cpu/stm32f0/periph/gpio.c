/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f0
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
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

typedef struct {
    gpio_cb_t cb;
    void *arg;
} exti_ctx_t;

/**
 * @brief   Allocate memory for one callback and argument per EXTI channel
 */
static exti_ctx_t exti_ctx[GPIO_ISR_CHAN_NUMOF];

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

/* static port mappings */
static GPIO_TypeDef *const gpio_port_map[GPIO_NUMOF] = {
#if GPIO_0_EN
    [GPIO_0] = GPIO_0_PORT,
#endif
#if GPIO_1_EN
    [GPIO_1] = GPIO_1_PORT,
#endif
#if GPIO_2_EN
    [GPIO_2] = GPIO_2_PORT,
#endif
#if GPIO_3_EN
    [GPIO_3] = GPIO_3_PORT,
#endif
#if GPIO_4_EN
    [GPIO_4] = GPIO_4_PORT,
#endif
#if GPIO_5_EN
    [GPIO_5] = GPIO_5_PORT,
#endif
#if GPIO_6_EN
    [GPIO_6] = GPIO_6_PORT,
#endif
#if GPIO_7_EN
    [GPIO_7] = GPIO_7_PORT,
#endif
#if GPIO_8_EN
    [GPIO_8] = GPIO_8_PORT,
#endif
#if GPIO_9_EN
    [GPIO_9] = GPIO_9_PORT,
#endif
#if GPIO_10_EN
    [GPIO_10] = GPIO_10_PORT,
#endif
#if GPIO_11_EN
    [GPIO_11] = GPIO_11_PORT,
#endif
};

/* static pin mappings */
static const uint8_t gpio_pin_map[GPIO_NUMOF] = {
#if GPIO_0_EN
    [GPIO_0] = GPIO_0_PIN,
#endif
#if GPIO_1_EN
    [GPIO_1] = GPIO_1_PIN,
#endif
#if GPIO_2_EN
    [GPIO_2] = GPIO_2_PIN,
#endif
#if GPIO_3_EN
    [GPIO_3] = GPIO_3_PIN,
#endif
#if GPIO_4_EN
    [GPIO_4] = GPIO_4_PIN,
#endif
#if GPIO_5_EN
    [GPIO_5] = GPIO_5_PIN,
#endif
#if GPIO_6_EN
    [GPIO_6] = GPIO_6_PIN,
#endif
#if GPIO_7_EN
    [GPIO_7] = GPIO_7_PIN,
#endif
#if GPIO_8_EN
    [GPIO_8] = GPIO_8_PIN,
#endif
#if GPIO_9_EN
    [GPIO_9] = GPIO_9_PIN,
#endif
#if GPIO_10_EN
    [GPIO_10] = GPIO_10_PIN,
#endif
#if GPIO_11_EN
    [GPIO_11] = GPIO_11_PIN,
#endif
};

/* static irq mappings */
static const IRQn_Type gpio_irq_map[GPIO_NUMOF] = {
#if GPIO_0_EN
    [GPIO_0] = GPIO_0_IRQ,
#endif
#if GPIO_1_EN
    [GPIO_1] = GPIO_1_IRQ,
#endif
#if GPIO_2_EN
    [GPIO_2] = GPIO_2_IRQ,
#endif
#if GPIO_3_EN
    [GPIO_3] = GPIO_3_IRQ,
#endif
#if GPIO_4_EN
    [GPIO_4] = GPIO_4_IRQ,
#endif
#if GPIO_5_EN
    [GPIO_5] = GPIO_5_IRQ,
#endif
#if GPIO_6_EN
    [GPIO_6] = GPIO_6_IRQ,
#endif
#if GPIO_7_EN
    [GPIO_7] = GPIO_7_IRQ,
#endif
#if GPIO_8_EN
    [GPIO_8] = GPIO_8_IRQ,
#endif
#if GPIO_9_EN
    [GPIO_9] = GPIO_9_IRQ,
#endif
#if GPIO_10_EN
    [GPIO_10] = GPIO_10_IRQ,
#endif
#if GPIO_11_EN
    [GPIO_11] = GPIO_11_IRQ,
#endif
};

/* static clock mapping */
static const uint8_t gpio_clock_map[GPIO_NUMOF] = {
#if GPIO_0_EN
    [GPIO_0] = GPIO_0_CLK,
#endif
#if GPIO_1_EN
    [GPIO_1] = GPIO_1_CLK,
#endif
#if GPIO_2_EN
    [GPIO_2] = GPIO_2_CLK,
#endif
#if GPIO_3_EN
    [GPIO_3] = GPIO_3_CLK,
#endif
#if GPIO_4_EN
    [GPIO_4] = GPIO_4_CLK,
#endif
#if GPIO_5_EN
    [GPIO_5] = GPIO_5_CLK,
#endif
#if GPIO_6_EN
    [GPIO_6] = GPIO_6_CLK,
#endif
#if GPIO_7_EN
    [GPIO_7] = GPIO_7_CLK,
#endif
#if GPIO_8_EN
    [GPIO_8] = GPIO_8_CLK,
#endif
#if GPIO_9_EN
    [GPIO_9] = GPIO_9_CLK,
#endif
#if GPIO_10_EN
    [GPIO_10] = GPIO_10_CLK,
#endif
#if GPIO_11_EN
    [GPIO_11] = GPIO_11_CLK,
#endif
};

int gpio_init(gpio_t dev, gpio_dir_t dir, gpio_pp_t pullup)
{
    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    RCC->AHBENR |= (1 << gpio_clock_map[dev]);

    port->PUPDR &= ~(3 << (2 * pin));           /* configure push-pull resistors */
    port->PUPDR |= (pullup << (2 * pin));

    if (dir == GPIO_DIR_OUT) {
        port->MODER &= ~(2 << (2 * pin));           /* set pin to output mode */
        port->MODER |= (1 << (2 * pin));
        port->OTYPER &= ~(1 << pin);                /* set to push-pull configuBuild and test: 1 failures in total
ration */
        port->OSPEEDR |= (3 << (2 * pin));          /* set to high speed */
        port->ODR &= ~(1 << pin);                   /* set pin to low signal */
    }
    else {
        port->MODER &= ~(3 << (2 * pin));           /* configure pin as input */

    }

    return 0; /* all OK */
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int res;

    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    pin = gpio_pin_map[dev];

    /* configure pin as input */
    res = gpio_init(dev, GPIO_DIR_IN, pullup);
    if (res < 0) {
        return res;
    }

    /* set interrupt priority (its the same for all EXTI interrupts) */
    NVIC_SetPriority(EXTI0_1_IRQn, GPIO_IRQ_PRIO);
    NVIC_SetPriority(EXTI2_3_IRQn, GPIO_IRQ_PRIO);
    NVIC_SetPriority(EXTI4_15_IRQn, GPIO_IRQ_PRIO);

    /* enable clock of the SYSCFG module for EXTI configuration */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;

    /* read pin number, set EXIT channel and enable global interrupt for EXTI channel */
    switch (dev) {
#ifdef GPIO_0_EN
        case GPIO_0:
            GPIO_0_EXTI_CFG();
            break;
#endif
#ifdef GPIO_1_EN
        case GPIO_1:
            GPIO_1_EXTI_CFG();
            break;
#endif
#ifdef GPIO_2_EN
        case GPIO_2:
            GPIO_2_EXTI_CFG();
            break;
#endif
#ifdef GPIO_3_EN
        case GPIO_3:
            GPIO_3_EXTI_CFG();
            break;
#endif
#ifdef GPIO_4_EN
        case GPIO_4:
            GPIO_4_EXTI_CFG();
            break;
#endif
#ifdef GPIO_5_EN
        case GPIO_5:
            GPIO_5_EXTI_CFG();
            break;
#endif
#ifdef GPIO_6_EN
        case GPIO_6:
            GPIO_6_EXTI_CFG();
            break;
#endif
#ifdef GPIO_7_EN
        case GPIO_7:
            GPIO_7_EXTI_CFG();
            break;
#endif
#ifdef GPIO_8_EN
        case GPIO_8:
            GPIO_8_EXTI_CFG();
            break;
#endif
#ifdef GPIO_9_EN
        case GPIO_9:
            GPIO_9_EXTI_CFG();
            break;
#endif
#ifdef GPIO_10_EN
        case GPIO_10:
            GPIO_10_EXTI_CFG();
            break;
#endif
#ifdef GPIO_11_EN
        case GPIO_11:
            GPIO_11_EXTI_CFG();
            break;
#endif
    }
    NVIC_EnableIRQ(gpio_irq_map[dev]);

    /* set callback */
    exti_ctx[pin_num].cb = cb;
    exti_ctx[pin_num].arg = arg;
    /* configure the event that triggers an interrupt */
    switch (flank) {
        case GPIO_RISING:
            EXTI->RTSR |= (1 << pin);
            EXTI->FTSR &= ~(1 << pin);
            break;
        case GPIO_FALLING:
            EXTI->RTSR &= ~(1 << pin);
            EXTI->FTSR |= (1 << pin);
            break;
        case GPIO_BOTH:
            EXTI->RTSR |= (1 << pin);
            EXTI->FTSR |= (1 << pin);
            break;
    }

    /* clear any pending requests */
    EXTI->PR = (1 << pin);
    /* unmask the pins interrupt channel */
    EXTI->IMR |= (1 << pin);

    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    uint8_t pin;

    if (dev >= GPIO_NUMOF) {
        return;
    }

    pin = gpio_pin_map[dev];
    EXTI->IMR |= (1 << pin);
}

void gpio_irq_disable(gpio_t dev)
{
    uint8_t pin;

    if (dev >= GPIO_NUMOF) {
        return;
    }

    pin = gpio_pin_map[dev];
    EXTI->IMR &= ~(1 << pin);
}

int gpio_read(gpio_t dev)
{
    GPIO_TypeDef *port;
    uint8_t pin;

    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    if (port->MODER & (1 << (pin * 2))) {       /* if configured as output */
        return port->ODR & (1 << pin);          /* read output data register */
    } else {
        return port->IDR & (1 << pin);          /* else read input data register */
    }
}

void gpio_set(gpio_t dev)
{
    GPIO_TypeDef *port;
    uint8_t pin;

    if (dev >= GPIO_NUMOF) {
        return;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    port->ODR |= (1 << pin);
}

void gpio_clear(gpio_t dev)
{
    GPIO_TypeDef *port;
    uint8_t pin;

    if (dev >= GPIO_NUMOF) {
        return;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    port->ODR &= ~(1 << pin);
}

void gpio_toggle(gpio_t dev)
{
    if (gpio_read(dev)) {
        gpio_clear(dev);
    } else {
        gpio_set(dev);
    }
}

void gpio_write(gpio_t dev, int value)
{
    if (value) {
        gpio_set(dev);
    } else {
        gpio_clear(dev);
    }
}

void isr_exti(void)
{
    for (int i = 0; i < GPIO_ISR_CHAN_NUMOF; i++) {
        if (EXTI->PR & (1 << i)) {
            EXTI->PR = (1 << i);        /* clear by writing a 1 */
            exti_ctx[i].cb(exti_ctx[i].arg);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
