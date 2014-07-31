/*
 * Copyright (C) 2014 Hauke Petersen <mail@haukepetersen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        gpio.c
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

#define ENABLE_DEBUG    (1)
#include "debug.h"

/* guard file in case no GPIO devices are defined */
#if GPIO_NUMOF

typedef struct {
    gpio_cb_t cb;       /**< callback called from GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
} gpio_state_t;

static gpio_state_t gpio_config[GPIO_NUMOF];

int gpio_init_out(gpio_t dev, gpio_pp_t pushpull)
{
    Pio *port = 0;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            port = GPIO_0_DEV;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            port = GPIO_1_DEV;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            port = GPIO_2_DEV;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            port = GPIO_3_DEV;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            port = GPIO_4_DEV;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            port = GPIO_5_DEV;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            port = GPIO_6_DEV;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            port = GPIO_7_DEV;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            port = GPIO_8_DEV;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            port = GPIO_9_DEV;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            port = GPIO_10_DEV;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            port = GPIO_11_DEV;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            port = GPIO_12_DEV;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            port = GPIO_13_DEV;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            port = GPIO_14_DEV;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            port = GPIO_15_DEV;
            pin = GPIO_15_PIN;
            break;
#endif
    }

    /* configure pin as output */
    port->PIO_PER = pin;
    port->PIO_OER = pin;
    port->PIO_CODR = pin;

    /* configure the pin's pull resistor state */
    switch (pushpull) {
        case GPIO_PULLDOWN:
            return -1;
        case GPIO_PULLUP:
            port->PIO_PUER = pin;
            break;
        case GPIO_NOPULL:
            port->PIO_PUDR = pin;
            break;
    }

    return 0;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pushpull)
{
    Pio *port = 0;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            port = GPIO_0_DEV;
            pin = GPIO_0_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_0_IRQ);
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            port = GPIO_1_DEV;
            pin = GPIO_1_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_1_IRQ);
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            port = GPIO_2_DEV;
            pin = GPIO_2_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_2_IRQ);
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            port = GPIO_3_DEV;
            pin = GPIO_3_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_3_IRQ);
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            port = GPIO_4_DEV;
            pin = GPIO_4_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_4_IRQ);
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            port = GPIO_5_DEV;
            pin = GPIO_5_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_5_IRQ);
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            port = GPIO_6_DEV;
            pin = GPIO_6_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_6_IRQ);
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            port = GPIO_7_DEV;
            pin = GPIO_7_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_7_IRQ);
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            port = GPIO_8_DEV;
            pin = GPIO_8_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_8_IRQ);
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            port = GPIO_9_DEV;
            pin = GPIO_9_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_9_IRQ);
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            port = GPIO_10_DEV;
            pin = GPIO_10_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_10_IRQ);
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            port = GPIO_11_DEV;
            pin = GPIO_11_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_11_IRQ);
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            port = GPIO_12_DEV;
            pin = GPIO_12_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_12_IRQ);
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            port = GPIO_13_DEV;
            pin = GPIO_13_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_13_IRQ);
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            port = GPIO_14_DEV;
            pin = GPIO_14_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_14_IRQ);
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            port = GPIO_15_DEV;
            pin = GPIO_15_PIN;
            PMC->PMC_PCER0 = (1 << GPIO_15_IRQ);
            break;
#endif
    }

    /* give the PIO module the power over the corresponding pin */
    port->PIO_PER = pin;
    /* configure pin as input */
    port->PIO_ODR = pin;

    /* configure the pin's pull resistor state */
    switch (pushpull) {
        case GPIO_PULLDOWN:
            return -1;
        case GPIO_PULLUP:
            port->PIO_PUER = pin;
            break;
        case GPIO_NOPULL:
            port->PIO_PUDR = pin;
            break;
    }

    return 0;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int res;
    Pio *port = 0;
    uint32_t pin = 0;

    /* initialize port as input */
    res = gpio_init_in(dev, pullup);
    if (res < 0) {
        return res;
    }

    /* read port and enable port interrupts */
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            port = GPIO_0_DEV;
            pin = GPIO_0_PIN;
            NVIC_SetPriority(GPIO_0_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_0_IRQ);
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            port = GPIO_1_DEV;
            pin = GPIO_1_PIN;
            NVIC_SetPriority(GPIO_1_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_1_IRQ);
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            port = GPIO_2_DEV;
            pin = GPIO_2_PIN;
            NVIC_SetPriority(GPIO_2_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_2_IRQ);
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            port = GPIO_3_DEV;
            pin = GPIO_3_PIN;
            NVIC_SetPriority(GPIO_3_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_3_IRQ);
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            port = GPIO_4_DEV;
            pin = GPIO_4_PIN;
            NVIC_SetPriority(GPIO_4_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_4_IRQ);
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            port = GPIO_5_DEV;
            pin = GPIO_5_PIN;
            NVIC_SetPriority(GPIO_5_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_5_IRQ);
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            port = GPIO_6_DEV;
            pin = GPIO_6_PIN;
            NVIC_SetPriority(GPIO_6_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_6_IRQ);
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            port = GPIO_7_DEV;
            pin = GPIO_7_PIN;
            NVIC_SetPriority(GPIO_7_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_7_IRQ);
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            port = GPIO_8_DEV;
            pin = GPIO_8_PIN;
            NVIC_SetPriority(GPIO_8_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_8_IRQ);
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            port = GPIO_9_DEV;
            pin = GPIO_9_PIN;
            NVIC_SetPriority(GPIO_9_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_9_IRQ);
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            port = GPIO_10_DEV;
            pin = GPIO_10_PIN;
            NVIC_SetPriority(GPIO_10_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_10_IRQ);
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            port = GPIO_11_DEV;
            pin = GPIO_11_PIN;
            NVIC_SetPriority(GPIO_11_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_11_IRQ);
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            port = GPIO_12_DEV;
            pin = GPIO_12_PIN;
            NVIC_SetPriority(GPIO_12_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_12_IRQ);
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            port = GPIO_13_DEV;
            pin = GPIO_13_PIN;
            NVIC_SetPriority(GPIO_13_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_13_IRQ);
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            port = GPIO_14_DEV;
            pin = GPIO_14_PIN;
            NVIC_SetPriority(GPIO_14_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_14_IRQ);
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            port = GPIO_15_DEV;
            pin = GPIO_15_PIN;
            NVIC_SetPriority(GPIO_15_IRQ, GPIO_IRQ_PRIO);
            NVIC_EnableIRQ(GPIO_15_IRQ);
            break;
#endif
    }

    /* set callback function and parameter */
    gpio_config[dev].cb = cb;
    gpio_config[dev].arg = arg;

    /* set the active flank */
    switch (flank) {
        case GPIO_FALLING:
            port->PIO_AIMER = pin;
            port->PIO_ESR = pin;
            port->PIO_FELLSR = pin;
            break;
        case GPIO_RISING:
            port->PIO_AIMER = pin;
            port->PIO_ESR = pin;
            port->PIO_REHLSR = pin;
            break;
        case GPIO_BOTH:
            port->PIO_AIMDR = pin;
            break;
    }

    /* clean interrupt status register */
    port->PIO_ISR;

    /* enable the interrupt for the given channel */
    port->PIO_IER = pin;

    return 0;
}

int gpio_read(gpio_t dev)
{
    Pio *port = 0;
    uint32_t pin = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            port = GPIO_0_DEV;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            port = GPIO_1_DEV;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            port = GPIO_2_DEV;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            port = GPIO_3_DEV;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            port = GPIO_4_DEV;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            port = GPIO_5_DEV;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            port = GPIO_6_DEV;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            port = GPIO_7_DEV;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            port = GPIO_8_DEV;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            port = GPIO_9_DEV;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            port = GPIO_10_DEV;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            port = GPIO_11_DEV;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            port = GPIO_12_DEV;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            port = GPIO_13_DEV;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            port = GPIO_14_DEV;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            port = GPIO_15_DEV;
            pin = GPIO_15_PIN;
            break;
#endif
    }

    if (port->PIO_OSR & pin) {
        return (port->PIO_ODSR & pin);
    }
    else {
        return (port->PIO_PDSR & pin);
    }
}

void gpio_set(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_0_DEV->PIO_SODR = GPIO_0_PIN;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_1_DEV->PIO_SODR = GPIO_1_PIN;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_2_DEV->PIO_SODR = GPIO_2_PIN;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_3_DEV->PIO_SODR = GPIO_3_PIN;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_4_DEV->PIO_SODR = GPIO_4_PIN;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_5_DEV->PIO_SODR = GPIO_5_PIN;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_6_DEV->PIO_SODR = GPIO_6_PIN;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_7_DEV->PIO_SODR = GPIO_7_PIN;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_8_DEV->PIO_SODR = GPIO_8_PIN;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_9_DEV->PIO_SODR = GPIO_9_PIN;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_10_DEV->PIO_SODR = GPIO_10_PIN;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_11_DEV->PIO_SODR = GPIO_11_PIN;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_12_DEV->PIO_SODR = GPIO_12_PIN;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_13_DEV->PIO_SODR = GPIO_13_PIN;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_14_DEV->PIO_SODR = GPIO_14_PIN;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_15_DEV->PIO_SODR = GPIO_15_PIN;
#endif
    }
}

void gpio_clear(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            GPIO_0_DEV->PIO_CODR = GPIO_0_PIN;
#endif
#if GPIO_1_EN
        case GPIO_1:
            GPIO_1_DEV->PIO_CODR = GPIO_1_PIN;
#endif
#if GPIO_2_EN
        case GPIO_2:
            GPIO_2_DEV->PIO_CODR = GPIO_2_PIN;
#endif
#if GPIO_3_EN
        case GPIO_3:
            GPIO_3_DEV->PIO_CODR = GPIO_3_PIN;
#endif
#if GPIO_4_EN
        case GPIO_4:
            GPIO_4_DEV->PIO_CODR = GPIO_4_PIN;
#endif
#if GPIO_5_EN
        case GPIO_5:
            GPIO_5_DEV->PIO_CODR = GPIO_5_PIN;
#endif
#if GPIO_6_EN
        case GPIO_6:
            GPIO_6_DEV->PIO_CODR = GPIO_6_PIN;
#endif
#if GPIO_7_EN
        case GPIO_7:
            GPIO_7_DEV->PIO_CODR = GPIO_7_PIN;
#endif
#if GPIO_8_EN
        case GPIO_8:
            GPIO_8_DEV->PIO_CODR = GPIO_8_PIN;
#endif
#if GPIO_9_EN
        case GPIO_9:
            GPIO_9_DEV->PIO_CODR = GPIO_9_PIN;
#endif
#if GPIO_10_EN
        case GPIO_10:
            GPIO_10_DEV->PIO_CODR = GPIO_10_PIN;
#endif
#if GPIO_11_EN
        case GPIO_11:
            GPIO_11_DEV->PIO_CODR = GPIO_11_PIN;
#endif
#if GPIO_12_EN
        case GPIO_12:
            GPIO_12_DEV->PIO_CODR = GPIO_12_PIN;
#endif
#if GPIO_13_EN
        case GPIO_13:
            GPIO_13_DEV->PIO_CODR = GPIO_13_PIN;
#endif
#if GPIO_14_EN
        case GPIO_14:
            GPIO_14_DEV->PIO_CODR = GPIO_14_PIN;
#endif
#if GPIO_15_EN
        case GPIO_15:
            GPIO_15_DEV->PIO_CODR = GPIO_15_PIN;
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

__attribute__((naked)) void isr_pioa(void)
{
    ISR_ENTER();
    uint32_t status = PIOA->PIO_ISR;
#ifdef GPIO_A0_MAP
    if (status & (1 << 0)) {
        gpio_config[GPIO_A0_MAP].cb(gpio_config[GPIO_A0_MAP].arg);
    }
#endif
#ifdef GPIO_A1_MAP
    if (status & (1 << 1)) {
        gpio_config[GPIO_A1_MAP].cb(gpio_config[GPIO_A1_MAP].arg);
    }
#endif
#ifdef GPIO_A2_MAP
    if (status & (1 << 2)) {
        gpio_config[GPIO_A2_MAP].cb(gpio_config[GPIO_A2_MAP].arg);
    }
#endif
#ifdef GPIO_A3_MAP
    if (status & (1 << 3)) {
        gpio_config[GPIO_A3_MAP].cb(gpio_config[GPIO_A3_MAP].arg);
    }
#endif
#ifdef GPIO_A4_MAP
    if (status & (1 << 4)) {
        gpio_config[GPIO_A4_MAP].cb(gpio_config[GPIO_A4_MAP].arg);
    }
#endif
#ifdef GPIO_A5_MAP
    if (status & (1 << 5)) {
        gpio_config[GPIO_A5_MAP].cb(gpio_config[GPIO_A5_MAP].arg);
    }
#endif
#ifdef GPIO_A6_MAP
    if (status & (1 << 6)) {
        gpio_config[GPIO_A6_MAP].cb(gpio_config[GPIO_A6_MAP].arg);
    }
#endif
#ifdef GPIO_A7_MAP
    if (status & (1 << 7)) {
        gpio_config[GPIO_A7_MAP].cb(gpio_config[GPIO_A7_MAP].arg);
    }
#endif
#ifdef GPIO_A8_MAP
    if (status & (1 << 8)) {
        gpio_config[GPIO_A8_MAP].cb(gpio_config[GPIO_A8_MAP].arg);
    }
#endif
#ifdef GPIO_A9_MAP
    if (status & (1 << 9)) {
        gpio_config[GPIO_A9_MAP].cb(gpio_config[GPIO_A9_MAP].arg);
    }
#endif
#ifdef GPIO_A10_MAP
    if (status & (1 << 10)) {
        gpio_config[GPIO_A10_MAP].cb(gpio_config[GPIO_A10_MAP].arg);
    }
#endif
#ifdef GPIO_A11_MAP
    if (status & (1 << 11)) {
        gpio_config[GPIO_A11_MAP].cb(gpio_config[GPIO_A11_MAP].arg);
    }
#endif
#ifdef GPIO_A12_MAP
    if (status & (1 << 12)) {
        gpio_config[GPIO_A12_MAP].cb(gpio_config[GPIO_A12_MAP].arg);
    }
#endif
#ifdef GPIO_A13_MAP
    if (status & (1 << 13)) {
        gpio_config[GPIO_A13_MAP].cb(gpio_config[GPIO_A13_MAP].arg);
    }
#endif
#ifdef GPIO_A14_MAP
    if (status & (1 << 14)) {
        gpio_config[GPIO_A14_MAP].cb(gpio_config[GPIO_A14_MAP].arg);
    }
#endif
#ifdef GPIO_A15_MAP
    if (status & (1 << 15)) {
        gpio_config[GPIO_A15_MAP].cb(gpio_config[GPIO_A15_MAP].arg);
    }
#endif
#ifdef GPIO_A16_MAP
    if (status & (1 << 16)) {
        gpio_config[GPIO_A16_MAP].cb(gpio_config[GPIO_A16_MAP].arg);
    }
#endif
#ifdef GPIO_A17_MAP
    if (status & (1 << 17)) {
        gpio_config[GPIO_A17_MAP].cb(gpio_config[GPIO_A17_MAP].arg);
    }
#endif
#ifdef GPIO_A18_MAP
    if (status & (1 << 18)) {
        gpio_config[GPIO_A18_MAP].cb(gpio_config[GPIO_A18_MAP].arg);
    }
#endif
#ifdef GPIO_A19_MAP
    if (status & (1 << 19)) {
        gpio_config[GPIO_A19_MAP].cb(gpio_config[GPIO_A19_MAP].arg);
    }
#endif
#ifdef GPIO_A20_MAP
    if (status & (1 << 20)) {
        gpio_config[GPIO_A20_MAP].cb(gpio_config[GPIO_A20_MAP].arg);
    }
#endif
#ifdef GPIO_A21_MAP
    if (status & (1 << 21)) {
        gpio_config[GPIO_A21_MAP].cb(gpio_config[GPIO_A21_MAP].arg);
    }
#endif
#ifdef GPIO_A22_MAP
    if (status & (1 << 22)) {
        gpio_config[GPIO_A22_MAP].cb(gpio_config[GPIO_A22_MAP].arg);
    }
#endif
#ifdef GPIO_A23_MAP
    if (status & (1 << 23)) {
        gpio_config[GPIO_A23_MAP].cb(gpio_config[GPIO_A23_MAP].arg);
    }
#endif
#ifdef GPIO_A24_MAP
    if (status & (1 << 24)) {
        gpio_config[GPIO_A24_MAP].cb(gpio_config[GPIO_A24_MAP].arg);
    }
#endif
#ifdef GPIO_A25_MAP
    if (status & (1 << 25)) {
        gpio_config[GPIO_A25_MAP].cb(gpio_config[GPIO_A25_MAP].arg);
    }
#endif
#ifdef GPIO_A26_MAP
    if (status & (1 << 26)) {
        gpio_config[GPIO_A26_MAP].cb(gpio_config[GPIO_A26_MAP].arg);
    }
#endif
#ifdef GPIO_A27_MAP
    if (status & (1 << 27)) {
        gpio_config[GPIO_A27_MAP].cb(gpio_config[GPIO_A27_MAP].arg);
    }
#endif
#ifdef GPIO_A28_MAP
    if (status & (1 << 28)) {
        gpio_config[GPIO_A28_MAP].cb(gpio_config[GPIO_A28_MAP].arg);
    }
#endif
#ifdef GPIO_A29_MAP
    if (status & (1 << 29)) {
        gpio_config[GPIO_A29_MAP].cb(gpio_config[GPIO_A29_MAP].arg);
    }
#endif
#ifdef GPIO_A30_MAP
    if (status & (1 << 30)) {
        gpio_config[GPIO_A30_MAP].cb(gpio_config[GPIO_A30_MAP].arg);
    }
#endif
#ifdef GPIO_A31_MAP
    if (status & (1 << 31)) {
        gpio_config[GPIO_A31_MAP].cb(gpio_config[GPIO_A31_MAP].arg);
    }
#endif
    if (sched_context_switch_request) {
        thread_yield();
    }
    ISR_EXIT();
}

__attribute__((naked)) void isr_piob(void)
{
    ISR_ENTER();
    uint32_t status = PIOB->PIO_ISR;
#ifdef GPIO_B0_MAP
    if (status & (1 << 0)) {
        gpio_config[GPIO_B0_MAP].cb(gpio_config[GPIO_B0_MAP].arg);
    }
#endif
#ifdef GPIO_B1_MAP
    if (status & (1 << 1)) {
        gpio_config[GPIO_B1_MAP].cb(gpio_config[GPIO_B1_MAP].arg);
    }
#endif
#ifdef GPIO_B2_MAP
    if (status & (1 << 2)) {
        gpio_config[GPIO_B2_MAP].cb(gpio_config[GPIO_B2_MAP].arg);
    }
#endif
#ifdef GPIO_B3_MAP
    if (status & (1 << 3)) {
        gpio_config[GPIO_B3_MAP].cb(gpio_config[GPIO_B3_MAP].arg);
    }
#endif
#ifdef GPIO_B4_MAP
    if (status & (1 << 4)) {
        gpio_config[GPIO_B4_MAP].cb(gpio_config[GPIO_B4_MAP].arg);
    }
#endif
#ifdef GPIO_B5_MAP
    if (status & (1 << 5)) {
        gpio_config[GPIO_B5_MAP].cb(gpio_config[GPIO_B5_MAP].arg);
    }
#endif
#ifdef GPIO_B6_MAP
    if (status & (1 << 6)) {
        gpio_config[GPIO_B6_MAP].cb(gpio_config[GPIO_B6_MAP].arg);
    }
#endif
#ifdef GPIO_B7_MAP
    if (status & (1 << 7)) {
        gpio_config[GPIO_B7_MAP].cb(gpio_config[GPIO_B7_MAP].arg);
    }
#endif
#ifdef GPIO_B8_MAP
    if (status & (1 << 8)) {
        gpio_config[GPIO_B8_MAP].cb(gpio_config[GPIO_B8_MAP].arg);
    }
#endif
#ifdef GPIO_B9_MAP
    if (status & (1 << 9)) {
        gpio_config[GPIO_B9_MAP].cb(gpio_config[GPIO_B9_MAP].arg);
    }
#endif
#ifdef GPIO_B10_MAP
    if (status & (1 << 10)) {
        gpio_config[GPIO_B10_MAP].cb(gpio_config[GPIO_B10_MAP].arg);
    }
#endif
#ifdef GPIO_B11_MAP
    if (status & (1 << 11)) {
        gpio_config[GPIO_B11_MAP].cb(gpio_config[GPIO_B11_MAP].arg);
    }
#endif
#ifdef GPIO_B12_MAP
    if (status & (1 << 12)) {
        gpio_config[GPIO_B12_MAP].cb(gpio_config[GPIO_B12_MAP].arg);
    }
#endif
#ifdef GPIO_B13_MAP
    if (status & (1 << 13)) {
        gpio_config[GPIO_B13_MAP].cb(gpio_config[GPIO_B13_MAP].arg);
    }
#endif
#ifdef GPIO_B14_MAP
    if (status & (1 << 14)) {
        gpio_config[GPIO_B14_MAP].cb(gpio_config[GPIO_B14_MAP].arg);
    }
#endif
#ifdef GPIO_B15_MAP
    if (status & (1 << 15)) {
        gpio_config[GPIO_B15_MAP].cb(gpio_config[GPIO_B15_MAP].arg);
    }
#endif
#ifdef GPIO_B16_MAP
    if (status & (1 << 16)) {
        gpio_config[GPIO_B16_MAP].cb(gpio_config[GPIO_B16_MAP].arg);
    }
#endif
#ifdef GPIO_B17_MAP
    if (status & (1 << 17)) {
        gpio_config[GPIO_B17_MAP].cb(gpio_config[GPIO_B17_MAP].arg);
    }
#endif
#ifdef GPIO_B18_MAP
    if (status & (1 << 18)) {
        gpio_config[GPIO_B18_MAP].cb(gpio_config[GPIO_B18_MAP].arg);
    }
#endif
#ifdef GPIO_B19_MAP
    if (status & (1 << 19)) {
        gpio_config[GPIO_B19_MAP].cb(gpio_config[GPIO_B19_MAP].arg);
    }
#endif
#ifdef GPIO_B20_MAP
    if (status & (1 << 20)) {
        gpio_config[GPIO_B20_MAP].cb(gpio_config[GPIO_B20_MAP].arg);
    }
#endif
#ifdef GPIO_B21_MAP
    if (status & (1 << 21)) {
        gpio_config[GPIO_B21_MAP].cb(gpio_config[GPIO_B21_MAP].arg);
    }
#endif
#ifdef GPIO_B22_MAP
    if (status & (1 << 22)) {
        gpio_config[GPIO_B22_MAP].cb(gpio_config[GPIO_B22_MAP].arg);
    }
#endif
#ifdef GPIO_B23_MAP
    if (status & (1 << 23)) {
        gpio_config[GPIO_B23_MAP].cb(gpio_config[GPIO_B23_MAP].arg);
    }
#endif
#ifdef GPIO_B24_MAP
    if (status & (1 << 24)) {
        gpio_config[GPIO_B24_MAP].cb(gpio_config[GPIO_B24_MAP].arg);
    }
#endif
#ifdef GPIO_B25_MAP
    if (status & (1 << 25)) {
        gpio_config[GPIO_B25_MAP].cb(gpio_config[GPIO_B25_MAP].arg);
    }
#endif
#ifdef GPIO_B26_MAP
    if (status & (1 << 26)) {
        gpio_config[GPIO_B26_MAP].cb(gpio_config[GPIO_B26_MAP].arg);
    }
#endif
#ifdef GPIO_B27_MAP
    if (status & (1 << 27)) {
        gpio_config[GPIO_B27_MAP].cb(gpio_config[GPIO_B27_MAP].arg);
    }
#endif
#ifdef GPIO_B28_MAP
    if (status & (1 << 28)) {
        gpio_config[GPIO_B28_MAP].cb(gpio_config[GPIO_B28_MAP].arg);
    }
#endif
#ifdef GPIO_B29_MAP
    if (status & (1 << 29)) {
        gpio_config[GPIO_B29_MAP].cb(gpio_config[GPIO_B29_MAP].arg);
    }
#endif
#ifdef GPIO_B30_MAP
    if (status & (1 << 30)) {
        gpio_config[GPIO_B30_MAP].cb(gpio_config[GPIO_B30_MAP].arg);
    }
#endif
#ifdef GPIO_B31_MAP
    if (status & (1 << 31)) {
        gpio_config[GPIO_B31_MAP].cb(gpio_config[GPIO_B31_MAP].arg);
    }
#endif
    if (sched_context_switch_request) {
        thread_yield();
    }
    ISR_EXIT();
}

__attribute__((naked)) void isr_pioc(void)
{
    ISR_ENTER();
    uint32_t status = PIOC->PIO_ISR;
#ifdef GPIO_C0_MAP
    if (status & (1 << 0)) {
        gpio_config[GPIO_C0_MAP].cb(gpio_config[GPIO_C0_MAP].arg);
    }
#endif
#ifdef GPIO_C1_MAP
    if (status & (1 << 1)) {
        gpio_config[GPIO_C1_MAP].cb(gpio_config[GPIO_C1_MAP].arg);
    }
#endif
#ifdef GPIO_C2_MAP
    if (status & (1 << 2)) {
        gpio_config[GPIO_C2_MAP].cb(gpio_config[GPIO_C2_MAP].arg);
    }
#endif
#ifdef GPIO_C3_MAP
    if (status & (1 << 3)) {
        gpio_config[GPIO_C3_MAP].cb(gpio_config[GPIO_C3_MAP].arg);
    }
#endif
#ifdef GPIO_C4_MAP
    if (status & (1 << 4)) {
        gpio_config[GPIO_C4_MAP].cb(gpio_config[GPIO_C4_MAP].arg);
    }
#endif
#ifdef GPIO_C5_MAP
    if (status & (1 << 5)) {
        gpio_config[GPIO_C5_MAP].cb(gpio_config[GPIO_C5_MAP].arg);
    }
#endif
#ifdef GPIO_C6_MAP
    if (status & (1 << 6)) {
        gpio_config[GPIO_C6_MAP].cb(gpio_config[GPIO_C6_MAP].arg);
    }
#endif
#ifdef GPIO_C7_MAP
    if (status & (1 << 7)) {
        gpio_config[GPIO_C7_MAP].cb(gpio_config[GPIO_C7_MAP].arg);
    }
#endif
#ifdef GPIO_C8_MAP
    if (status & (1 << 8)) {
        gpio_config[GPIO_C8_MAP].cb(gpio_config[GPIO_C8_MAP].arg);
    }
#endif
#ifdef GPIO_C9_MAP
    if (status & (1 << 9)) {
        gpio_config[GPIO_C9_MAP].cb(gpio_config[GPIO_C9_MAP].arg);
    }
#endif
#ifdef GPIO_C10_MAP
    if (status & (1 << 10)) {
        gpio_config[GPIO_C10_MAP].cb(gpio_config[GPIO_C10_MAP].arg);
    }
#endif
#ifdef GPIO_C11_MAP
    if (status & (1 << 11)) {
        gpio_config[GPIO_C11_MAP].cb(gpio_config[GPIO_C11_MAP].arg);
    }
#endif
#ifdef GPIO_C12_MAP
    if (status & (1 << 12)) {
        gpio_config[GPIO_C12_MAP].cb(gpio_config[GPIO_C12_MAP].arg);
    }
#endif
#ifdef GPIO_C13_MAP
    if (status & (1 << 13)) {
        gpio_config[GPIO_C13_MAP].cb(gpio_config[GPIO_C13_MAP].arg);
    }
#endif
#ifdef GPIO_C14_MAP
    if (status & (1 << 14)) {
        gpio_config[GPIO_C14_MAP].cb(gpio_config[GPIO_C14_MAP].arg);
    }
#endif
#ifdef GPIO_C15_MAP
    if (status & (1 << 15)) {
        gpio_config[GPIO_C15_MAP].cb(gpio_config[GPIO_C15_MAP].arg);
    }
#endif
#ifdef GPIO_C16_MAP
    if (status & (1 << 16)) {
        gpio_config[GPIO_C16_MAP].cb(gpio_config[GPIO_C16_MAP].arg);
    }
#endif
#ifdef GPIO_C17_MAP
    if (status & (1 << 17)) {
        gpio_config[GPIO_C17_MAP].cb(gpio_config[GPIO_C17_MAP].arg);
    }
#endif
#ifdef GPIO_C18_MAP
    if (status & (1 << 18)) {
        gpio_config[GPIO_C18_MAP].cb(gpio_config[GPIO_C18_MAP].arg);
    }
#endif
#ifdef GPIO_C19_MAP
    if (status & (1 << 19)) {
        gpio_config[GPIO_C19_MAP].cb(gpio_config[GPIO_C19_MAP].arg);
    }
#endif
#ifdef GPIO_C20_MAP
    if (status & (1 << 20)) {
        gpio_config[GPIO_C20_MAP].cb(gpio_config[GPIO_C20_MAP].arg);
    }
#endif
#ifdef GPIO_C21_MAP
    if (status & (1 << 21)) {
        gpio_config[GPIO_C21_MAP].cb(gpio_config[GPIO_C21_MAP].arg);
    }
#endif
#ifdef GPIO_C22_MAP
    if (status & (1 << 22)) {
        gpio_config[GPIO_C22_MAP].cb(gpio_config[GPIO_C22_MAP].arg);
    }
#endif
#ifdef GPIO_C23_MAP
    if (status & (1 << 23)) {
        gpio_config[GPIO_C23_MAP].cb(gpio_config[GPIO_C23_MAP].arg);
    }
#endif
#ifdef GPIO_C24_MAP
    if (status & (1 << 24)) {
        gpio_config[GPIO_C24_MAP].cb(gpio_config[GPIO_C24_MAP].arg);
    }
#endif
#ifdef GPIO_C25_MAP
    if (status & (1 << 25)) {
        gpio_config[GPIO_C25_MAP].cb(gpio_config[GPIO_C25_MAP].arg);
    }
#endif
#ifdef GPIO_C26_MAP
    if (status & (1 << 26)) {
        gpio_config[GPIO_C26_MAP].cb(gpio_config[GPIO_C26_MAP].arg);
    }
#endif
#ifdef GPIO_C27_MAP
    if (status & (1 << 27)) {
        gpio_config[GPIO_C27_MAP].cb(gpio_config[GPIO_C27_MAP].arg);
    }
#endif
#ifdef GPIO_C28_MAP
    if (status & (1 << 28)) {
        gpio_config[GPIO_C28_MAP].cb(gpio_config[GPIO_C28_MAP].arg);
    }
#endif
#ifdef GPIO_C29_MAP
    if (status & (1 << 29)) {
        gpio_config[GPIO_C29_MAP].cb(gpio_config[GPIO_C29_MAP].arg);
    }
#endif
#ifdef GPIO_C30_MAP
    if (status & (1 << 30)) {
        gpio_config[GPIO_C30_MAP].cb(gpio_config[GPIO_C30_MAP].arg);
    }
#endif
#ifdef GPIO_C31_MAP
    if (status & (1 << 31)) {
        gpio_config[GPIO_C31_MAP].cb(gpio_config[GPIO_C31_MAP].arg);
    }
#endif
    if (sched_context_switch_request) {
        thread_yield();
    }

    ISR_EXIT();
}

__attribute__((naked)) void isr_piod(void)
{
    ISR_ENTER();
    uint32_t status = PIOD->PIO_ISR;
#ifdef GPIO_D0_MAP
    if (status & (1 << 0)) {
        gpio_config[GPIO_D0_MAP].cb(gpio_config[GPIO_D0_MAP].arg);
    }
#endif
#ifdef GPIO_D1_MAP
    if (status & (1 << 1)) {
        gpio_config[GPIO_D1_MAP].cb(gpio_config[GPIO_D1_MAP].arg);
    }
#endif
#ifdef GPIO_D2_MAP
    if (status & (1 << 2)) {
        gpio_config[GPIO_D2_MAP].cb(gpio_config[GPIO_D2_MAP].arg);
    }
#endif
#ifdef GPIO_D3_MAP
    if (status & (1 << 3)) {
        gpio_config[GPIO_D3_MAP].cb(gpio_config[GPIO_D3_MAP].arg);
    }
#endif
#ifdef GPIO_D4_MAP
    if (status & (1 << 4)) {
        gpio_config[GPIO_D4_MAP].cb(gpio_config[GPIO_D4_MAP].arg);
    }
#endif
#ifdef GPIO_D5_MAP
    if (status & (1 << 5)) {
        gpio_config[GPIO_D5_MAP].cb(gpio_config[GPIO_D5_MAP].arg);
    }
#endif
#ifdef GPIO_D6_MAP
    if (status & (1 << 6)) {
        gpio_config[GPIO_D6_MAP].cb(gpio_config[GPIO_D6_MAP].arg);
    }
#endif
#ifdef GPIO_D7_MAP
    if (status & (1 << 7)) {
        gpio_config[GPIO_D7_MAP].cb(gpio_config[GPIO_D7_MAP].arg);
    }
#endif
#ifdef GPIO_D8_MAP
    if (status & (1 << 8)) {
        gpio_config[GPIO_D8_MAP].cb(gpio_config[GPIO_D8_MAP].arg);
    }
#endif
#ifdef GPIO_D9_MAP
    if (status & (1 << 9)) {
        gpio_config[GPIO_D9_MAP].cb(gpio_config[GPIO_D9_MAP].arg);
    }
#endif
#ifdef GPIO_D10_MAP
    if (status & (1 << 10)) {
        gpio_config[GPIO_D10_MAP].cb(gpio_config[GPIO_D10_MAP].arg);
    }
#endif
#ifdef GPIO_D11_MAP
    if (status & (1 << 11)) {
        gpio_config[GPIO_D11_MAP].cb(gpio_config[GPIO_D11_MAP].arg);
    }
#endif
#ifdef GPIO_D12_MAP
    if (status & (1 << 12)) {
        gpio_config[GPIO_D12_MAP].cb(gpio_config[GPIO_D12_MAP].arg);
    }
#endif
#ifdef GPIO_D13_MAP
    if (status & (1 << 13)) {
        gpio_config[GPIO_D13_MAP].cb(gpio_config[GPIO_D13_MAP].arg);
    }
#endif
#ifdef GPIO_D14_MAP
    if (status & (1 << 14)) {
        gpio_config[GPIO_D14_MAP].cb(gpio_config[GPIO_D14_MAP].arg);
    }
#endif
#ifdef GPIO_D15_MAP
    if (status & (1 << 15)) {
        gpio_config[GPIO_D15_MAP].cb(gpio_config[GPIO_D15_MAP].arg);
    }
#endif
#ifdef GPIO_D16_MAP
    if (status & (1 << 16)) {
        gpio_config[GPIO_D16_MAP].cb(gpio_config[GPIO_D16_MAP].arg);
    }
#endif
#ifdef GPIO_D17_MAP
    if (status & (1 << 17)) {
        gpio_config[GPIO_D17_MAP].cb(gpio_config[GPIO_D17_MAP].arg);
    }
#endif
#ifdef GPIO_D18_MAP
    if (status & (1 << 18)) {
        gpio_config[GPIO_D18_MAP].cb(gpio_config[GPIO_D18_MAP].arg);
    }
#endif
#ifdef GPIO_D19_MAP
    if (status & (1 << 19)) {
        gpio_config[GPIO_D19_MAP].cb(gpio_config[GPIO_D19_MAP].arg);
    }
#endif
#ifdef GPIO_D20_MAP
    if (status & (1 << 20)) {
        gpio_config[GPIO_D20_MAP].cb(gpio_config[GPIO_D20_MAP].arg);
    }
#endif
#ifdef GPIO_D21_MAP
    if (status & (1 << 21)) {
        gpio_config[GPIO_D21_MAP].cb(gpio_config[GPIO_D21_MAP].arg);
    }
#endif
#ifdef GPIO_D22_MAP
    if (status & (1 << 22)) {
        gpio_config[GPIO_D22_MAP].cb(gpio_config[GPIO_D22_MAP].arg);
    }
#endif
#ifdef GPIO_D23_MAP
    if (status & (1 << 23)) {
        gpio_config[GPIO_D23_MAP].cb(gpio_config[GPIO_D23_MAP].arg);
    }
#endif
#ifdef GPIO_D24_MAP
    if (status & (1 << 24)) {
        gpio_config[GPIO_D24_MAP].cb(gpio_config[GPIO_D24_MAP].arg);
    }
#endif
#ifdef GPIO_D25_MAP
    if (status & (1 << 25)) {
        gpio_config[GPIO_D25_MAP].cb(gpio_config[GPIO_D25_MAP].arg);
    }
#endif
#ifdef GPIO_D26_MAP
    if (status & (1 << 26)) {
        gpio_config[GPIO_D26_MAP].cb(gpio_config[GPIO_D26_MAP].arg);
    }
#endif
#ifdef GPIO_D27_MAP
    if (status & (1 << 27)) {
        gpio_config[GPIO_D27_MAP].cb(gpio_config[GPIO_D27_MAP].arg);
    }
#endif
#ifdef GPIO_D28_MAP
    if (status & (1 << 28)) {
        gpio_config[GPIO_D28_MAP].cb(gpio_config[GPIO_D28_MAP].arg);
    }
#endif
#ifdef GPIO_D29_MAP
    if (status & (1 << 29)) {
        gpio_config[GPIO_D29_MAP].cb(gpio_config[GPIO_D29_MAP].arg);
    }
#endif
#ifdef GPIO_D30_MAP
    if (status & (1 << 30)) {
        gpio_config[GPIO_D30_MAP].cb(gpio_config[GPIO_D30_MAP].arg);
    }
#endif
#ifdef GPIO_D31_MAP
    if (status & (1 << 31)) {
        gpio_config[GPIO_D31_MAP].cb(gpio_config[GPIO_D31_MAP].arg);
    }
#endif
    if (sched_context_switch_request) {
        thread_yield();
    }

    ISR_EXIT();
}

#endif /* GPIO_NUMOF */
