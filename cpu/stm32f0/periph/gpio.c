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

/* guard file in case no GPIO devices are defined */
#if GPIO_NUMOF

typedef struct {
    gpio_cb_t cb;
    void *arg;
} gpio_state_t;

static gpio_state_t gpio_config[GPIO_NUMOF];

int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    GPIO_TypeDef *port = 0;
    uint32_t pin = 0;

    switch (dev) {
#ifdef GPIO_0_EN
        case GPIO_0:
            GPIO_0_CLKEN();
            port = GPIO_0_PORT;
            pin = GPIO_0_PIN;
            break;
#endif
#ifdef GPIO_1_EN
        case GPIO_1:
            GPIO_1_CLKEN();
            port = GPIO_1_PORT;
            pin = GPIO_1_PIN;
            break;
#endif
#ifdef GPIO_2_EN
        case GPIO_2:
            GPIO_2_CLKEN();
            port = GPIO_2_PORT;
            pin = GPIO_2_PIN;
            break;
#endif
#ifdef GPIO_3_EN
        case GPIO_3:
            GPIO_3_CLKEN();
            port = GPIO_3_PORT;
            pin = GPIO_3_PIN;
            break;
#endif
#ifdef GPIO_4_EN
        case GPIO_4:
            GPIO_4_CLKEN();
            port = GPIO_4_PORT;
            pin = GPIO_4_PIN;
            break;
#endif
#ifdef GPIO_5_EN
        case GPIO_5:
            GPIO_5_CLKEN();
            port = GPIO_5_PORT;
            pin = GPIO_5_PIN;
            break;
#endif
#ifdef GPIO_6_EN
        case GPIO_6:
            GPIO_6_CLKEN();
            port = GPIO_6_PORT;
            pin = GPIO_6_PIN;
            break;
#endif
#ifdef GPIO_7_EN
        case GPIO_7:
            GPIO_7_CLKEN();
            port = GPIO_7_PORT;
            pin = GPIO_7_PIN;
            break;
#endif
#ifdef GPIO_8_EN
        case GPIO_8:
            GPIO_8_CLKEN();
            port = GPIO_8_PORT;
            pin = GPIO_8_PIN;
            break;
#endif
#ifdef GPIO_9_EN
        case GPIO_9:
            GPIO_9_CLKEN();
            port = GPIO_9_PORT;
            pin = GPIO_9_PIN;
            break;
#endif
#ifdef GPIO_10_EN
        case GPIO_10:
            GPIO_10_CLKEN();
            port = GPIO_10_PORT;
            pin = GPIO_10_PIN;
            break;
#endif
#ifdef GPIO_11_EN
        case GPIO_11:
            GPIO_11_CLKEN();
            port = GPIO_11_PORT;
            pin = GPIO_11_PIN;
            break;
#endif
    }

    port->MODER &= ~(2 << (2 * pin));           /* set pin to output mode */
    port->MODER |= (1 << (2 * pin));
    port->OTYPER &= ~(1 << pin);                /* set to push-pull configuration */
    port->OSPEEDR |= (3 << (2 * pin));          /* set to high speed */
    port->PUPDR &= ~(3 << (2 * pin));           /* configure push-pull resistors */
    port->PUPDR |= (pullup << (2 * pin));
    port->ODR &= ~(1 << pin);                   /* set pin to low signal */

    return 0; /* all OK */
}

int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
{
    GPIO_TypeDef *port = 0;
    uint32_t pin = 0;

    switch (dev) {
#ifdef GPIO_0_EN
        case GPIO_0:
            GPIO_0_CLKEN();
            port = GPIO_0_PORT;
            pin = GPIO_0_PIN;
            break;
#endif
#ifdef GPIO_1_EN
        case GPIO_1:
            GPIO_1_CLKEN();
            port = GPIO_1_PORT;
            pin = GPIO_1_PIN;
            break;
#endif
#ifdef GPIO_2_EN
        case GPIO_2:
            GPIO_2_CLKEN();
            port = GPIO_2_PORT;
            pin = GPIO_2_PIN;
            break;
#endif
#ifdef GPIO_3_EN
        case GPIO_3:
            GPIO_3_CLKEN();
            port = GPIO_3_PORT;
            pin = GPIO_3_PIN;
            break;
#endif
#ifdef GPIO_4_EN
        case GPIO_4:
            GPIO_4_CLKEN();
            port = GPIO_4_PORT;
            pin = GPIO_4_PIN;
            break;
#endif
#ifdef GPIO_5_EN
        case GPIO_5:
            GPIO_5_CLKEN();
            port = GPIO_5_PORT;
            pin = GPIO_5_PIN;
            break;
#endif
#ifdef GPIO_6_EN
        case GPIO_6:
            GPIO_6_CLKEN();
            port = GPIO_6_PORT;
            pin = GPIO_6_PIN;
            break;
#endif
#ifdef GPIO_7_EN
        case GPIO_7:
            GPIO_7_CLKEN();
            port = GPIO_7_PORT;
            pin = GPIO_7_PIN;
            break;
#endif
#ifdef GPIO_8_EN
        case GPIO_8:
            GPIO_8_CLKEN();
            port = GPIO_8_PORT;
            pin = GPIO_8_PIN;
            break;
#endif
#ifdef GPIO_9_EN
        case GPIO_9:
            GPIO_9_CLKEN();
            port = GPIO_9_PORT;
            pin = GPIO_9_PIN;
            break;
#endif
#ifdef GPIO_10_EN
        case GPIO_10:
            GPIO_10_CLKEN();
            port = GPIO_10_PORT;
            pin = GPIO_10_PIN;
            break;
#endif
#ifdef GPIO_11_EN
        case GPIO_11:
            GPIO_11_CLKEN();
            port = GPIO_11_PORT;
            pin = GPIO_11_PIN;
            break;
#endif
    }

    port->MODER &= ~(3 << (2 * pin));           /* configure pin as input */
    port->PUPDR &= ~(3 << (2 * pin));           /* configure push-pull resistors */
    port->PUPDR |= (pullup << (2 * pin));

    return 0; /* everything alright here */
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int res;
    uint32_t pin = 0;

    /* configure pin as input */
    res = gpio_init_in(dev, pullup);
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
            pin = GPIO_0_PIN;
            GPIO_0_EXTI_CFG();
            NVIC_SetPriority(GPIO_0_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_0_IRQ);
            break;
#endif
#ifdef GPIO_1_EN
        case GPIO_1:
            pin = GPIO_1_PIN;
            GPIO_1_EXTI_CFG();
            NVIC_SetPriority(GPIO_1_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_1_IRQ);
            break;
#endif
#ifdef GPIO_2_EN
        case GPIO_2:
            pin = GPIO_2_PIN;
            GPIO_2_EXTI_CFG();
            NVIC_SetPriority(GPIO_2_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_2_IRQ);
            break;
#endif
#ifdef GPIO_3_EN
        case GPIO_3:
            pin = GPIO_3_PIN;
            GPIO_3_EXTI_CFG();
            NVIC_SetPriority(GPIO_3_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_3_IRQ);
            break;
#endif
#ifdef GPIO_4_EN
        case GPIO_4:
            pin = GPIO_4_PIN;
            GPIO_4_EXTI_CFG();
            NVIC_SetPriority(GPIO_4_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_4_IRQ);
            break;
#endif
#ifdef GPIO_5_EN
        case GPIO_5:
            pin = GPIO_5_PIN;
            GPIO_5_EXTI_CFG();
            NVIC_SetPriority(GPIO_5_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_5_IRQ);
            break;
#endif
#ifdef GPIO_6_EN
        case GPIO_6:
            pin = GPIO_6_PIN;
            GPIO_6_EXTI_CFG();
            NVIC_SetPriority(GPIO_6_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_6_IRQ);
            break;
#endif
#ifdef GPIO_7_EN
        case GPIO_7:
            pin = GPIO_7_PIN;
            GPIO_7_EXTI_CFG();
            NVIC_SetPriority(GPIO_7_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_7_IRQ);
            break;
#endif
#ifdef GPIO_8_EN
        case GPIO_8:
            pin = GPIO_8_PIN;
            GPIO_8_EXTI_CFG();
            NVIC_SetPriority(GPIO_8_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_8_IRQ);
            break;
#endif
#ifdef GPIO_9_EN
        case GPIO_9:
            pin = GPIO_9_PIN;
            GPIO_9_EXTI_CFG();
            NVIC_SetPriority(GPIO_9_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_9_IRQ);
            break;
#endif
#ifdef GPIO_10_EN
        case GPIO_10:
            pin = GPIO_10_PIN;
            GPIO_10_EXTI_CFG();
            NVIC_SetPriority(GPIO_10_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_10_IRQ);
            break;
#endif
#ifdef GPIO_11_EN
        case GPIO_11:
            pin = GPIO_11_PIN;
            GPIO_11_EXTI_CFG();
            NVIC_SetPriority(GPIO_11_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_11_IRQ);
            break;
#endif
    }

    /* set callback */
    gpio_config[dev].cb = cb;
    gpio_config[dev].arg = arg;

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
    switch (dev) {
#ifdef GPIO_0_EN
        case GPIO_0:
            EXTI->IMR |= (1 << GPIO_0_PIN);
            break;
#endif
#ifdef GPIO_1_EN
        case GPIO_1:
            EXTI->IMR |= (1 << GPIO_1_PIN);
            break;
#endif
#ifdef GPIO_2_EN
        case GPIO_2:
            EXTI->IMR |= (1 << GPIO_2_PIN);
            break;
#endif
#ifdef GPIO_3_EN
        case GPIO_3:
            EXTI->IMR |= (1 << GPIO_3_PIN);
            break;
#endif
#ifdef GPIO_4_EN
        case GPIO_4:
            EXTI->IMR |= (1 << GPIO_4_PIN);
            break;
#endif
#ifdef GPIO_5_EN
        case GPIO_5:
            EXTI->IMR |= (1 << GPIO_5_PIN);
            break;
#endif
#ifdef GPIO_6_EN
        case GPIO_6:
            EXTI->IMR |= (1 << GPIO_6_PIN);
            break;
#endif
#ifdef GPIO_7_EN
        case GPIO_7:
            EXTI->IMR |= (1 << GPIO_7_PIN);
            break;
#endif
#ifdef GPIO_8_EN
        case GPIO_8:
            EXTI->IMR |= (1 << GPIO_8_PIN);
            break;
#endif
#ifdef GPIO_9_EN
        case GPIO_9:
            EXTI->IMR |= (1 << GPIO_9_PIN);
            break;
#endif
#ifdef GPIO_10_EN
        case GPIO_10:
            EXTI->IMR |= (1 << GPIO_10_PIN);
            break;
#endif
#ifdef GPIO_11_EN
        case GPIO_11:
            EXTI->IMR |= (1 << GPIO_11_PIN);
            break;
#endif
    }
}

void gpio_irq_disable(gpio_t dev)
{
    switch (dev) {
#ifdef GPIO_0_EN
        case GPIO_0:
            EXTI->IMR &= ~(1 << GPIO_0_PIN);
            break;
#endif
#ifdef GPIO_1_EN
        case GPIO_1:
            EXTI->IMR &= ~(1 << GPIO_1_PIN);
            break;
#endif
#ifdef GPIO_2_EN
        case GPIO_2:
            EXTI->IMR &= ~(1 << GPIO_2_PIN);
            break;
#endif
#ifdef GPIO_3_EN
        case GPIO_3:
            EXTI->IMR &= ~(1 << GPIO_3_PIN);
            break;
#endif
#ifdef GPIO_4_EN
        case GPIO_4:
            EXTI->IMR &= ~(1 << GPIO_4_PIN);
            break;
#endif
#ifdef GPIO_5_EN
        case GPIO_5:
            EXTI->IMR &= ~(1 << GPIO_5_PIN);
            break;
#endif
#ifdef GPIO_6_EN
        case GPIO_6:
            EXTI->IMR &= ~(1 << GPIO_6_PIN);
            break;
#endif
#ifdef GPIO_7_EN
        case GPIO_7:
            EXTI->IMR &= ~(1 << GPIO_7_PIN);
            break;
#endif
#ifdef GPIO_8_EN
        case GPIO_8:
            EXTI->IMR &= ~(1 << GPIO_8_PIN);
            break;
#endif
#ifdef GPIO_9_EN
        case GPIO_9:
            EXTI->IMR &= ~(1 << GPIO_9_PIN);
            break;
#endif
#ifdef GPIO_10_EN
        case GPIO_10:
            EXTI->IMR &= ~(1 << GPIO_10_PIN);
            break;
#endif
#ifdef GPIO_11_EN
        case GPIO_11:
            EXTI->IMR &= ~(1 << GPIO_11_PIN);
            break;
#endif
    }
}

int gpio_read(gpio_t dev)
{
    GPIO_TypeDef *port = 0;
    uint32_t pin = 0;

    switch (dev) {
#ifdef GPIO_0_EN
        case GPIO_0:
            port = GPIO_0_PORT;
            pin = GPIO_0_PIN;
            break;
#endif
#ifdef GPIO_1_EN
        case GPIO_1:
            port = GPIO_1_PORT;
            pin = GPIO_1_PIN;
            break;
#endif
#ifdef GPIO_2_EN
        case GPIO_2:
            port = GPIO_2_PORT;
            pin = GPIO_2_PIN;
            break;
#endif
#ifdef GPIO_3_EN
        case GPIO_3:
            port = GPIO_3_PORT;
            pin = GPIO_3_PIN;
            break;
#endif
#ifdef GPIO_4_EN
        case GPIO_4:
            port = GPIO_4_PORT;
            pin = GPIO_4_PIN;
            break;
#endif
#ifdef GPIO_5_EN
        case GPIO_5:
            port = GPIO_5_PORT;
            pin = GPIO_5_PIN;
            break;
#endif
#ifdef GPIO_6_EN
        case GPIO_6:
            port = GPIO_6_PORT;
            pin = GPIO_6_PIN;
            break;
#endif
#ifdef GPIO_7_EN
        case GPIO_7:
            port = GPIO_7_PORT;
            pin = GPIO_7_PIN;
            break;
#endif
#ifdef GPIO_8_EN
        case GPIO_8:
            port = GPIO_8_PORT;
            pin = GPIO_8_PIN;
            break;
#endif
#ifdef GPIO_9_EN
        case GPIO_9:
            port = GPIO_9_PORT;
            pin = GPIO_9_PIN;
            break;
#endif
#ifdef GPIO_10_EN
        case GPIO_10:
            port = GPIO_10_PORT;
            pin = GPIO_10_PIN;
            break;
#endif
#ifdef GPIO_11_EN
        case GPIO_11:
            port = GPIO_11_PORT;
            pin = GPIO_11_PIN;
            break;
#endif
    }

    if (port->MODER & (1 << (pin * 2))) {       /* if configured as output */
        return port->ODR & (1 << pin);          /* read output data register */
    } else {
        return port->IDR & (1 << pin);          /* else read input data register */
    }
}

void gpio_set(gpio_t dev)
{
    switch (dev) {
#ifdef GPIO_0_EN
        case GPIO_0:
            GPIO_0_PORT->ODR |= (1 << GPIO_0_PIN);
            break;
#endif
#ifdef GPIO_1_EN
        case GPIO_1:
            GPIO_1_PORT->ODR |= (1 << GPIO_1_PIN);
            break;
#endif
#ifdef GPIO_2_EN
        case GPIO_2:
            GPIO_2_PORT->ODR |= (1 << GPIO_2_PIN);
            break;
#endif
#ifdef GPIO_3_EN
        case GPIO_3:
            GPIO_3_PORT->ODR |= (1 << GPIO_3_PIN);
            break;
#endif
#ifdef GPIO_4_EN
        case GPIO_4:
            GPIO_4_PORT->ODR |= (1 << GPIO_4_PIN);
            break;
#endif
#ifdef GPIO_5_EN
        case GPIO_5:
            GPIO_5_PORT->ODR |= (1 << GPIO_5_PIN);
            break;
#endif
#ifdef GPIO_6_EN
        case GPIO_6:
            GPIO_6_PORT->ODR |= (1 << GPIO_6_PIN);
            break;
#endif
#ifdef GPIO_7_EN
        case GPIO_7:
            GPIO_7_PORT->ODR |= (1 << GPIO_7_PIN);
            break;
#endif
#ifdef GPIO_8_EN
        case GPIO_8:
            GPIO_8_PORT->ODR |= (1 << GPIO_8_PIN);
            break;
#endif
#ifdef GPIO_9_EN
        case GPIO_9:
            GPIO_9_PORT->ODR |= (1 << GPIO_9_PIN);
            break;
#endif
#ifdef GPIO_10_EN
        case GPIO_10:
            GPIO_10_PORT->ODR |= (1 << GPIO_10_PIN);
            break;
#endif
#ifdef GPIO_11_EN
        case GPIO_11:
            GPIO_11_PORT->ODR |= (1 << GPIO_11_PIN);
            break;
#endif
    }
}

void gpio_clear(gpio_t dev)
{
    switch (dev) {
#ifdef GPIO_0_EN
        case GPIO_0:
            GPIO_0_PORT->ODR &= ~(1 << GPIO_0_PIN);
            break;
#endif
#ifdef GPIO_1_EN
        case GPIO_1:
            GPIO_1_PORT->ODR &= ~(1 << GPIO_1_PIN);
            break;
#endif
#ifdef GPIO_2_EN
        case GPIO_2:
            GPIO_2_PORT->ODR &= ~(1 << GPIO_2_PIN);
            break;
#endif
#ifdef GPIO_3_EN
        case GPIO_3:
            GPIO_3_PORT->ODR &= ~(1 << GPIO_3_PIN);
            break;
#endif
#ifdef GPIO_4_EN
        case GPIO_4:
            GPIO_4_PORT->ODR &= ~(1 << GPIO_4_PIN);
            break;
#endif
#ifdef GPIO_5_EN
        case GPIO_5:
            GPIO_5_PORT->ODR &= ~(1 << GPIO_5_PIN);
            break;
#endif
#ifdef GPIO_6_EN
        case GPIO_6:
            GPIO_6_PORT->ODR &= ~(1 << GPIO_6_PIN);
            break;
#endif
#ifdef GPIO_7_EN
        case GPIO_7:
            GPIO_7_PORT->ODR &= ~(1 << GPIO_7_PIN);
            break;
#endif
#ifdef GPIO_8_EN
        case GPIO_8:
            GPIO_8_PORT->ODR &= ~(1 << GPIO_8_PIN);
            break;
#endif
#ifdef GPIO_9_EN
        case GPIO_9:
            GPIO_9_PORT->ODR &= ~(1 << GPIO_9_PIN);
            break;
#endif
#ifdef GPIO_10_EN
        case GPIO_10:
            GPIO_10_PORT->ODR &= ~(1 << GPIO_10_PIN);
            break;
#endif
#ifdef GPIO_11_EN
        case GPIO_11:
            GPIO_11_PORT->ODR &= ~(1 << GPIO_11_PIN);
            break;
#endif
    }
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

__attribute__((naked)) void isr_exti0_1(void)
{
    ISR_ENTER();
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR |= EXTI_PR_PR0;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_0].cb(gpio_config[GPIO_IRQ_0].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR1) {
        EXTI->PR |= EXTI_PR_PR1;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_1].cb(gpio_config[GPIO_IRQ_1].arg);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
    ISR_EXIT();
}

__attribute__((naked)) void isr_exti2_3(void)
{
    ISR_ENTER();
    if (EXTI->PR & EXTI_PR_PR2) {
        EXTI->PR |= EXTI_PR_PR2;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_2].cb(gpio_config[GPIO_IRQ_2].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR3) {
        EXTI->PR |= EXTI_PR_PR3;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_3].cb(gpio_config[GPIO_IRQ_3].arg);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
    ISR_EXIT();
}

__attribute__((naked)) void isr_exti4_15(void)
{
    ISR_ENTER();
    if (EXTI->PR & EXTI_PR_PR4) {
        EXTI->PR |= EXTI_PR_PR4;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_4].cb(gpio_config[GPIO_IRQ_4].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR5) {
        EXTI->PR |= EXTI_PR_PR5;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_5].cb(gpio_config[GPIO_IRQ_5].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR6) {
        EXTI->PR |= EXTI_PR_PR6;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_6].cb(gpio_config[GPIO_IRQ_6].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR7) {
        EXTI->PR |= EXTI_PR_PR7;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_7].cb(gpio_config[GPIO_IRQ_7].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR8) {
        EXTI->PR |= EXTI_PR_PR8;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_8].cb(gpio_config[GPIO_IRQ_8].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR9) {
        EXTI->PR |= EXTI_PR_PR9;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_9].cb(gpio_config[GPIO_IRQ_9].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR10) {
        EXTI->PR |= EXTI_PR_PR10;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_10].cb(gpio_config[GPIO_IRQ_10].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR11) {
        EXTI->PR |= EXTI_PR_PR11;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_11].cb(gpio_config[GPIO_IRQ_11].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR12) {
        EXTI->PR |= EXTI_PR_PR12;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_12].cb(gpio_config[GPIO_IRQ_12].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR13) {
        EXTI->PR |= EXTI_PR_PR13;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_13].cb(gpio_config[GPIO_IRQ_13].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR14) {
        EXTI->PR |= EXTI_PR_PR14;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_14].cb(gpio_config[GPIO_IRQ_14].arg);
    }
    else if (EXTI->PR & EXTI_PR_PR15) {
        EXTI->PR |= EXTI_PR_PR15;        /* clear status bit by writing a 1 to it */
        gpio_config[GPIO_IRQ_15].cb(gpio_config[GPIO_IRQ_15].arg);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
    ISR_EXIT();
}

#endif /* GPIO_NUMOF */
