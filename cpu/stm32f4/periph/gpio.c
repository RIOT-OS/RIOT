/*
 * Copyright (C) 2014 Freie Universität Berlin
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

/* guard file in case no GPIO devices are defined */
#if GPIO_NUMOF

typedef struct {
    gpio_cb_t cb;       /**< callback to call on GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
} gpio_state_t;

/**
 * @brief Unified IRQ handler shared by all interrupt routines
 *
 * @param[in] dev   the device that triggered the interrupt
 */
static inline void irq_handler(gpio_t dev);

/* static port mapping */
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
#if GPIO_12_EN
    [GPIO_12] = GPIO_12_PORT,
#endif
#if GPIO_13_EN
    [GPIO_13] = GPIO_13_PORT,
#endif
#if GPIO_14_EN
    [GPIO_14] = GPIO_14_PORT,
#endif
#if GPIO_15_EN
    [GPIO_15] = GPIO_15_PORT,
#endif
};

/* static pin mapping */
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
#if GPIO_12_EN
    [GPIO_12] = GPIO_12_PIN,
#endif
#if GPIO_13_EN
    [GPIO_13] = GPIO_13_PIN,
#endif
#if GPIO_14_EN
    [GPIO_14] = GPIO_14_PIN,
#endif
#if GPIO_15_EN
    [GPIO_15] = GPIO_15_PIN,
#endif
};

/* static irq id mapping */
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
#if GPIO_12_EN
    [GPIO_12] = GPIO_12_IRQ,
#endif
#if GPIO_13_EN
    [GPIO_13] = GPIO_13_IRQ,
#endif
#if GPIO_14_EN
    [GPIO_14] = GPIO_14_IRQ,
#endif
#if GPIO_15_EN
    [GPIO_15] = GPIO_15_IRQ,
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
#if GPIO_12_EN
    [GPIO_12] = GPIO_12_CLK,
#endif
#if GPIO_13_EN
    [GPIO_13] = GPIO_13_CLK,
#endif
#if GPIO_14_EN
    [GPIO_14] = GPIO_14_CLK,
#endif
#if GPIO_15_EN
    [GPIO_15] = GPIO_15_CLK,
#endif
};

/**
 * @brief Hold one callback function pointer for each gpio device
 */
static gpio_state_t gpio_config[GPIO_NUMOF];

int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    GPIO_TypeDef *port;
    uint32_t pin;

    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    RCC->AHB1ENR |= (1 << gpio_clock_map[dev]);

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
    GPIO_TypeDef *port;
    uint32_t pin;

    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    RCC->AHB1ENR |= (1 << gpio_clock_map[dev]);

    port->MODER &= ~(3 << (2 * pin));           /* configure pin as input */
    port->PUPDR &= ~(3 << (2 * pin));           /* configure push-pull resistors */
    port->PUPDR |= (pullup << (2 * pin));

    return 0; /* everything alright here */
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int res;
    uint32_t pin;

    res = gpio_init_in(dev, pullup);
    if (res < 0) {
        return res;
    }

    pin = gpio_pin_map[dev];

    /* enable the SYSCFG clock */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /* enable IRQ */
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_0_EXTI_CFG();
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_1_EXTI_CFG();
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_2_EXTI_CFG();
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_3_EXTI_CFG();
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_4_EXTI_CFG();
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_5_EXTI_CFG();
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_6_EXTI_CFG();
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_7_EXTI_CFG();
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_8_EXTI_CFG();
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_9_EXTI_CFG();
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_10_EXTI_CFG();
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_11_EXTI_CFG();
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_12_EXTI_CFG();
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_13_EXTI_CFG();
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_14_EXTI_CFG();
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_15_EXTI_CFG();
            break;
#endif
    }
    NVIC_SetPriority(gpio_irq_map[dev], GPIO_IRQ_PRIO);
    NVIC_EnableIRQ(gpio_irq_map[dev]);

    /* set callback */
    gpio_config[dev].cb = cb;
    gpio_config[dev].arg = arg;

    /* configure the active edges */
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
    /* enable interrupt for EXTI line */
    EXTI->IMR |= (1 << pin);

    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    uint32_t pin;

    if (dev >= GPIO_NUMOF) {
        return;
    }

    pin = gpio_pin_map[dev];
    EXTI->IMR |= (1 << pin);
}

void gpio_irq_disable(gpio_t dev)
{
    uint32_t pin;

    if (dev >= GPIO_NUMOF) {
        return;
    }

    pin = gpio_pin_map[dev];
    EXTI->IMR &= ~(1 << pin);
}

int gpio_read(gpio_t dev)
{
    GPIO_TypeDef *port;
    uint32_t pin;

    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    if (port->MODER & (3 << (pin * 2))) {       /* if configured as output */
        return port->ODR & (1 << pin);          /* read output data register */
    } else {
        return port->IDR & (1 << pin);          /* else read input data register */
    }
}

void gpio_set(gpio_t dev)
{
    GPIO_TypeDef *port;
    uint32_t pin;

    if (dev >= GPIO_NUMOF) {
        return;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    port->BSRRL = (1 << pin);
}

void gpio_clear(gpio_t dev)
{
    GPIO_TypeDef *port;
    uint32_t pin;

    if (dev >= GPIO_NUMOF) {
        return;
    }

    port = gpio_port_map[dev];
    pin = gpio_pin_map[dev];

    port->BSRRH = (1 << pin);
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

static inline void irq_handler(gpio_t dev)
{
    gpio_config[dev].cb(gpio_config[dev].arg);
    if (sched_context_switch_request) {
        thread_yield();
    }
}

void isr_exti0(void)
{
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR |= EXTI_PR_PR0;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_0);
    }
}

void isr_exti1(void)
{
    if (EXTI->PR & EXTI_PR_PR1) {
        EXTI->PR |= EXTI_PR_PR1;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_1);
    }
}

void isr_exti2(void)
{
    if (EXTI->PR & EXTI_PR_PR2) {
        EXTI->PR |= EXTI_PR_PR2;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_2);
    }
}

void isr_exti3(void)
{
    if (EXTI->PR & EXTI_PR_PR3) {
        EXTI->PR |= EXTI_PR_PR3;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_3);
    }
}

void isr_exti4(void)
{
    if (EXTI->PR & EXTI_PR_PR4) {
        EXTI->PR |= EXTI_PR_PR4;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_4);
    }
}

void isr_exti9_5(void)
{
    if (EXTI->PR & EXTI_PR_PR5) {
        EXTI->PR |= EXTI_PR_PR5;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_5);
    }
    if (EXTI->PR & EXTI_PR_PR6) {
        EXTI->PR |= EXTI_PR_PR6;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_6);
    }
    if (EXTI->PR & EXTI_PR_PR7) {
        EXTI->PR |= EXTI_PR_PR7;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_7);
    }
    if (EXTI->PR & EXTI_PR_PR8) {
        EXTI->PR |= EXTI_PR_PR8;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_8);
    }
    if (EXTI->PR & EXTI_PR_PR9) {
        EXTI->PR |= EXTI_PR_PR9;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_9);
    }
}

void isr_exti15_10(void)
{
    if (EXTI->PR & EXTI_PR_PR10) {
        EXTI->PR |= EXTI_PR_PR10;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_10);
    }
    if (EXTI->PR & EXTI_PR_PR11) {
        EXTI->PR |= EXTI_PR_PR11;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_11);
    }
    if (EXTI->PR & EXTI_PR_PR12) {
        EXTI->PR |= EXTI_PR_PR12;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_12);
    }
    if (EXTI->PR & EXTI_PR_PR13) {
        EXTI->PR |= EXTI_PR_PR13;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_13);
    }
    if (EXTI->PR & EXTI_PR_PR14) {
        EXTI->PR |= EXTI_PR_PR14;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_14);
    }
    if (EXTI->PR & EXTI_PR_PR15) {
        EXTI->PR |= EXTI_PR_PR15;        /* clear status bit by writing a 1 to it */
        irq_handler(GPIO_IRQ_15);
    }
}

#endif /* GPIO_NUMOF */
