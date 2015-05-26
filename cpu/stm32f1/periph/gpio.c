/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "stm32f10x.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "board.h"

#include "thread.h"

#include "debug.h"

/* guard file in case no GPIO device is defined */
#if GPIO_NUMOF

typedef struct {
    gpio_cb_t cb;       /**< callback called from GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
} gpio_state_t;

static gpio_state_t config[GPIO_NUMOF];

int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
{
    GPIO_TypeDef *port;
    uint32_t pin;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_0_CLKEN();
            port = GPIO_0_PORT;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_1_CLKEN();
            port = GPIO_1_PORT;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_2_CLKEN();
            port = GPIO_2_PORT;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_3_CLKEN();
            port = GPIO_3_PORT;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_4_CLKEN();
            port = GPIO_4_PORT;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_5_CLKEN();
            port = GPIO_5_PORT;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_6_CLKEN();
            port = GPIO_6_PORT;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_7_CLKEN();
            port = GPIO_7_PORT;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_8_CLKEN();
            port = GPIO_8_PORT;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_9_CLKEN();
            port = GPIO_9_PORT;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_10_CLKEN();
            port = GPIO_10_PORT;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_11_CLKEN();
            port = GPIO_11_PORT;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_12_CLKEN();
            port = GPIO_12_PORT;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_13_CLKEN();
            port = GPIO_13_PORT;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_14_CLKEN();
            port = GPIO_14_PORT;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_15_CLKEN();
            port = GPIO_15_PORT;
            pin = GPIO_15_PIN;
            break;
#endif

        default:
            return -1;
    }

    if (pin < 8) {
        port->CRL &= ~(0xf << (4 * pin));
        port->CRL |= (0x3 << (4* pin)); /* Output mode, 50 MHz */
                                        /* general purpose push-pull set implicitly */
    }
    else {
        port->CRH &= ~(0xf << (4 * (pin-8)));
        port->CRH |= (0x3 << (4* (pin-8))); /* Output mode, 50 MHz */
                                            /* general purpose push-pull set implicitly */
    }

    return 0; /* all OK */
}

int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
{
    GPIO_TypeDef *port;
    uint32_t pin;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_0_CLKEN();
            port = GPIO_0_PORT;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_1_CLKEN();
            port = GPIO_1_PORT;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_2_CLKEN();
            port = GPIO_2_PORT;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_3_CLKEN();
            port = GPIO_3_PORT;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_4_CLKEN();
            port = GPIO_4_PORT;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_5_CLKEN();
            port = GPIO_5_PORT;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_6_CLKEN();
            port = GPIO_6_PORT;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_7_CLKEN();
            port = GPIO_7_PORT;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_8_CLKEN();
            port = GPIO_8_PORT;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_9_CLKEN();
            port = GPIO_9_PORT;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_10_CLKEN();
            port = GPIO_10_PORT;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_11_CLKEN();
            port = GPIO_11_PORT;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_12_CLKEN();
            port = GPIO_12_PORT;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_13_CLKEN();
            port = GPIO_13_PORT;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_14_CLKEN();
            port = GPIO_14_PORT;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_15_CLKEN();
            port = GPIO_15_PORT;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return -1;
    }

    if (pin < 8) {
        port->CRL &= ~(0xf << (4 * pin));
        port->CRL |= (0x4 << (4 * pin));
    }
    else {
        port->CRL &= ~(0xf << (4 * pin));
        port->CRH |= (0x4 << (4 * (pin-8)));
    }

    return 0; /* everything alright here */
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int res;
    uint8_t exti_line;
    uint8_t gpio_irq;

    /* configure pin as input */
    res = gpio_init_in(dev, pullup);
    if (res < 0) {
        return res;
    }

    /* set interrupt priority (its the same for all EXTI interrupts) */
    NVIC_SetPriority(EXTI0_IRQn, GPIO_IRQ_PRIO);
    NVIC_SetPriority(EXTI1_IRQn, GPIO_IRQ_PRIO);
    NVIC_SetPriority(EXTI2_IRQn, GPIO_IRQ_PRIO);
    NVIC_SetPriority(EXTI4_IRQn, GPIO_IRQ_PRIO);

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    /* read pin number, set EXTI channel and enable global interrupt for EXTI channel */
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            exti_line = GPIO_0_PIN;
            gpio_irq = GPIO_0;
            GPIO_0_EXTI_CFG();
            NVIC_SetPriority(GPIO_0_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_0_IRQ);
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            exti_line = GPIO_1_PIN;
            gpio_irq = GPIO_1;
            GPIO_1_EXTI_CFG();
            NVIC_SetPriority(GPIO_1_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_1_IRQ);
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            exti_line = GPIO_2_PIN;
            gpio_irq = GPIO_2;
            GPIO_2_EXTI_CFG();
            NVIC_SetPriority(GPIO_2_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_2_IRQ);
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            exti_line = GPIO_3_PIN;
            gpio_irq = GPIO_3;
            GPIO_3_EXTI_CFG();
            NVIC_SetPriority(GPIO_3_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_3_IRQ);
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            exti_line = GPIO_4_PIN;
            gpio_irq = GPIO_4;
            GPIO_4_EXTI_CFG();
            NVIC_SetPriority(GPIO_4_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_4_IRQ);
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            exti_line = GPIO_5_PIN;
            gpio_irq = GPIO_5;
            GPIO_5_EXTI_CFG();
            NVIC_SetPriority(GPIO_5_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_5_IRQ);
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            exti_line = GPIO_6_PIN;
            gpio_irq = GPIO_6;
            GPIO_6_EXTI_CFG();
            NVIC_SetPriority(GPIO_6_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_6_IRQ);
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            exti_line = GPIO_7_PIN;
            gpio_irq = GPIO_7;
            GPIO_7_EXTI_CFG();
            NVIC_SetPriority(GPIO_7_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_7_IRQ);
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            exti_line = GPIO_8_PIN;
            gpio_irq = GPIO_8;
            GPIO_8_EXTI_CFG();
            NVIC_SetPriority(GPIO_8_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_8_IRQ);
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            exti_line = GPIO_9_PIN;
            gpio_irq = GPIO_9;
            GPIO_9_EXTI_CFG();
            NVIC_SetPriority(GPIO_9_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_9_IRQ);
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            exti_line = GPIO_10_PIN;
            gpio_irq = GPIO_10;
            GPIO_10_EXTI_CFG();
            NVIC_SetPriority(GPIO_10_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_10_IRQ);
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            exti_line = GPIO_11_PIN;
            gpio_irq = GPIO_11;
            GPIO_11_EXTI_CFG();
            NVIC_SetPriority(GPIO_11_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_11_IRQ);
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            exti_line = GPIO_12_PIN;
            gpio_irq = GPIO_12;
            GPIO_12_EXTI_CFG();
            NVIC_SetPriority(GPIO_12_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_12_IRQ);
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            exti_line = GPIO_13_PIN;
            gpio_irq = GPIO_13;
            GPIO_13_EXTI_CFG();
            NVIC_SetPriority(GPIO_13_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_13_IRQ);
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            exti_line = GPIO_14_PIN;
            gpio_irq = GPIO_14;
            GPIO_14_EXTI_CFG();
            NVIC_SetPriority(GPIO_14_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_14_IRQ);
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            exti_line = GPIO_15_PIN;
            gpio_irq = GPIO_15;
            GPIO_15_EXTI_CFG();
            NVIC_SetPriority(GPIO_15_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_15_IRQ);
            break;
#endif
        default:
            return -1;
    }

    /* set callback */
    config[gpio_irq].cb = cb;
    config[gpio_irq].arg = arg;

    /* configure the event that triggers an interrupt */
    switch (flank) {
        case GPIO_RISING:
            EXTI->RTSR |= (1 << exti_line);
            EXTI->FTSR &= ~(1 << exti_line);
            break;
        case GPIO_FALLING:
            EXTI->RTSR &= ~(1 << exti_line);
            EXTI->FTSR |= (1 << exti_line);
            break;
        case GPIO_BOTH:
            EXTI->RTSR |= (1 << exti_line);
            EXTI->FTSR |= (1 << exti_line);
            break;
    }

    /* clear event mask */
    EXTI->EMR &= ~(1 << exti_line);
    /* unmask the pins interrupt channel */
    EXTI->IMR |= (1 << exti_line);

    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    uint8_t exti_line;

    switch(dev) {
#if GPIO_0_EN
        case GPIO_0:
            exti_line = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            exti_line = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            exti_line = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            exti_line = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            exti_line = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            exti_line = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            exti_line = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            exti_line = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            exti_line = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            exti_line = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            exti_line = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            exti_line = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            exti_line = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            exti_line = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            exti_line = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            exti_line = GPIO_15_PIN;
            break;
#endif
        default:
            return;
    }

    /* unmask the pins interrupt channel */
    EXTI->IMR |= (1 << exti_line);

    return;
}

void gpio_irq_disable(gpio_t dev)
{
    uint8_t exti_line;

    switch(dev) {
#if GPIO_0_EN
        case GPIO_0:
            exti_line = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            exti_line = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            exti_line = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            exti_line = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            exti_line = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            exti_line = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            exti_line = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            exti_line = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            exti_line = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            exti_line = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            exti_line = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            exti_line = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            exti_line = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            exti_line = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            exti_line = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            exti_line = GPIO_15_PIN;
            break;
#endif
        default:
            return;
    }

    /* unmask the pins interrupt channel */
    EXTI->IMR &= ~(1 << exti_line);

    return;
}

int gpio_read(gpio_t dev)
{
    GPIO_TypeDef *port;
    uint32_t pin;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            port = GPIO_0_PORT;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            port = GPIO_1_PORT;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            port = GPIO_2_PORT;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            port = GPIO_3_PORT;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            port = GPIO_4_PORT;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            port = GPIO_5_PORT;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            port = GPIO_6_PORT;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            port = GPIO_7_PORT;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            port = GPIO_8_PORT;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            port = GPIO_9_PORT;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            port = GPIO_10_PORT;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            port = GPIO_11_PORT;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            port = GPIO_12_PORT;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            port = GPIO_13_PORT;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            port = GPIO_14_PORT;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            port = GPIO_15_PORT;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return -1;
    }

    if (pin < 8) {
        if (port->CRL & (0x3 << (pin * 4))) {  /* if configured as output */
            return port->ODR & (1 << pin);          /* read output data register */
        } else {
            return port->IDR & (1 << pin);          /* else read input data register */
        }
    }
    else {
        if (port->CRH & (0x3 << ((pin-8) * 4))) {  /* if configured as output */
            return port->ODR & (1 << pin);          /* read output data register */
        } else {
            return port->IDR & (1 << pin);          /* else read input data register */
        }
    }
}

void gpio_set(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_0_PORT->BSRR = (1 << GPIO_0_PIN);
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_1_PORT->BSRR = (1 << GPIO_1_PIN);
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_2_PORT->BSRR = (1 << GPIO_2_PIN);
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_3_PORT->BSRR = (1 << GPIO_3_PIN);
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_4_PORT->BSRR = (1 << GPIO_4_PIN);
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_5_PORT->BSRR = (1 << GPIO_5_PIN);
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_6_PORT->BSRR = (1 << GPIO_6_PIN);
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_7_PORT->BSRR = (1 << GPIO_7_PIN);
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_8_PORT->BSRR = (1 << GPIO_8_PIN);
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_9_PORT->BSRR = (1 << GPIO_9_PIN);
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_10_PORT->BSRR = (1 << GPIO_10_PIN);
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_11_PORT->BSRR = (1 << GPIO_11_PIN);
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_12_PORT->BSRR = (1 << GPIO_12_PIN);
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_13_PORT->BSRR = (1 << GPIO_13_PIN);
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_14_PORT->BSRR = (1 << GPIO_14_PIN);
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_15_PORT->BSRR = (1 << GPIO_15_PIN);
            break;
#endif
    }
}

void gpio_clear(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_0_PORT->BRR = (1 << GPIO_0_PIN);
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_1_PORT->BRR = (1 << GPIO_1_PIN);
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_2_PORT->BRR = (1 << GPIO_2_PIN);
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_3_PORT->BRR = (1 << GPIO_3_PIN);
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_4_PORT->BRR = (1 << GPIO_4_PIN);
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_5_PORT->BRR = (1 << GPIO_5_PIN);
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_6_PORT->BRR = (1 << GPIO_6_PIN);
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_7_PORT->BRR = (1 << GPIO_7_PIN);
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_8_PORT->BRR = (1 << GPIO_8_PIN);
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_9_PORT->BRR = (1 << GPIO_9_PIN);
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_10_PORT->BRR = (1 << GPIO_10_PIN);
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_11_PORT->BRR = (1 << GPIO_11_PIN);
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_12_PORT->BRR = (1 << GPIO_12_PIN);
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_13_PORT->BRR = (1 << GPIO_13_PIN);
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_14_PORT->BRR = (1 << GPIO_14_PIN);
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_15_PORT->BRR = (1 << GPIO_15_PIN);
            break;
#endif
    }
}


void gpio_toggle(gpio_t dev)
{
    if (gpio_read(dev)) {
        gpio_clear(dev);
    }
    else {
        gpio_set(dev);
    }
}

void gpio_write(gpio_t dev, int value)
{
    if (value) {
        gpio_set(dev);
    }
    else {
        gpio_clear(dev);
    }
}

#ifdef GPIO_IRQ_0
void isr_exti0(void)
{
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR |= EXTI_PR_PR0;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_0].cb(config[GPIO_IRQ_0].arg);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#ifdef GPIO_IRQ_1
void isr_exti1(void)
{
    if (EXTI->PR & EXTI_PR_PR1) {
        EXTI->PR |= EXTI_PR_PR1;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_1].cb(config[GPIO_IRQ_1].arg);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#ifdef GPIO_IRQ_2
void isr_exti2(void)
{
    if (EXTI->PR & EXTI_PR_PR2) {
        EXTI->PR |= EXTI_PR_PR2;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_2].cb(config[GPIO_IRQ_2].arg);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#ifdef GPIO_IRQ_3
void isr_exti3(void)
{
    if (EXTI->PR & EXTI_PR_PR3) {
        EXTI->PR |= EXTI_PR_PR3;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_3].cb(config[GPIO_IRQ_3].arg);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#ifdef GPIO_IRQ_4
void isr_exti4(void)
{
    if (EXTI->PR & EXTI_PR_PR4) {
        EXTI->PR |= EXTI_PR_PR4;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_4].cb(config[GPIO_IRQ_4].arg);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#if defined(GPIO_IRQ_5) || defined(GPIO_IRQ_6) || defined(GPIO_IRQ_7) || defined(GPIO_IRQ_8) || defined(GPIO_IRQ_9)
void isr_exti9_5(void)
{
#ifdef GPIO_IRQ_5
    if (EXTI->PR & EXTI_PR_PR5) {
        EXTI->PR |= EXTI_PR_PR5;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_5].cb(config[GPIO_IRQ_5].arg);
    }
#endif
#ifdef GPIO_IRQ_6
    if (EXTI->PR & EXTI_PR_PR6) {
        EXTI->PR |= EXTI_PR_PR6;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_6].cb(config[GPIO_IRQ_6].arg);
    }
#endif
#ifdef GPIO_IRQ_7
    if (EXTI->PR & EXTI_PR_PR7) {
        EXTI->PR |= EXTI_PR_PR7;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_7].cb(config[GPIO_IRQ_7].arg);
    }
#endif
#ifdef GPIO_IRQ_8
    if (EXTI->PR & EXTI_PR_PR8) {
        EXTI->PR |= EXTI_PR_PR8;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_8].cb(config[GPIO_IRQ_8].arg);
    }
#endif
#ifdef GPIO_IRQ_9
    if (EXTI->PR & EXTI_PR_PR9) {
        EXTI->PR |= EXTI_PR_PR9;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_9].cb(config[GPIO_IRQ_9].arg);
    }
#endif
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#if defined(GPIO_IRQ_10) || defined(GPIO_IRQ_11) || defined(GPIO_IRQ_12) || defined(GPIO_IRQ_13) || defined(GPIO_IRQ_14) || defined(GPIO_IRQ_15)
void isr_exti15_10(void)
{
#ifdef GPIO_IRQ_10
    if (EXTI->PR & EXTI_PR_PR10) {
        EXTI->PR |= EXTI_PR_PR10;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_10].cb(config[GPIO_IRQ_10].arg);
    }
#endif
#ifdef GPIO_IRQ_11
    if (EXTI->PR & EXTI_PR_PR11) {
        EXTI->PR |= EXTI_PR_PR11;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_11].cb(config[GPIO_IRQ_11].arg);
    }
#endif
#ifdef GPIO_IRQ_12
    if (EXTI->PR & EXTI_PR_PR12) {
        EXTI->PR |= EXTI_PR_PR12;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_12].cb(config[GPIO_IRQ_12].arg);
    }
#endif
#ifdef GPIO_IRQ_13
    if (EXTI->PR & EXTI_PR_PR13) {
        EXTI->PR |= EXTI_PR_PR13;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_13].cb(config[GPIO_IRQ_13].arg);
    }
#endif
#ifdef GPIO_IRQ_14
    if (EXTI->PR & EXTI_PR_PR14) {
        EXTI->PR |= EXTI_PR_PR14;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_14].cb(config[GPIO_IRQ_14].arg);
    }
#endif
#ifdef GPIO_IRQ_15
    if (EXTI->PR & EXTI_PR_PR15) {
        EXTI->PR |= EXTI_PR_PR15;        /* clear status bit by writing a 1 to it */
        config[GPIO_IRQ_15].cb(config[GPIO_IRQ_15].arg);
    }
#endif
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#endif
