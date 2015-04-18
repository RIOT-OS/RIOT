/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        gpio.c
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#include "sched.h"
#include "thread.h"
#define ENABLE_DEBUG (0)
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
    PortGroup* port = NULL;
    uint32_t pin = 0;
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            port = &GPIO_0_DEV;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            port = &GPIO_1_DEV;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            port = &GPIO_2_DEV;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            port = &GPIO_3_DEV;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            port = &GPIO_4_DEV;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            port = &GPIO_5_DEV;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            port = &GPIO_6_DEV;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            port = &GPIO_7_DEV;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            port = &GPIO_8_DEV;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            port = &GPIO_9_DEV;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            port = &GPIO_10_DEV;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            port = &GPIO_11_DEV;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            port = &GPIO_12_DEV;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            port = &GPIO_13_DEV;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            port = &GPIO_14_DEV;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            port = &GPIO_15_DEV;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return -1;
    }

    if (port == NULL) {
        return -1;
    }

    /* configure as output */
    port->DIRSET.reg = 1 << pin;

    /* configure the pin's pull resistor state */
    switch (pushpull) {
        case GPIO_PULLDOWN:
            return -1;
        case GPIO_PULLUP:
            port->PINCFG[pin].bit.PULLEN = true;
            break;
        case GPIO_NOPULL:
            port->PINCFG[pin].bit.PULLEN = false;
            break;
    }

    return 0;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pushpull)
{
    PortGroup* port = NULL;
    uint32_t pin = 0;
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            port = &GPIO_0_DEV;
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            port = &GPIO_1_DEV;
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            port = &GPIO_2_DEV;
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            port = &GPIO_3_DEV;
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            port = &GPIO_4_DEV;
            pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            port = &GPIO_5_DEV;
            pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            port = &GPIO_6_DEV;
            pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            port = &GPIO_7_DEV;
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            port = &GPIO_8_DEV;
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            port = &GPIO_9_DEV;
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            port = &GPIO_10_DEV;
            pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            port = &GPIO_11_DEV;
            pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            port = &GPIO_12_DEV;
            pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            port = &GPIO_13_DEV;
            pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            port = &GPIO_14_DEV;
            pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            port = &GPIO_15_DEV;
            pin = GPIO_15_PIN;
            break;
#endif
        default:
            return -1;
    }

    if (port == NULL) {
        return -1;
    }

    /* configure as input */
    port->DIRCLR.reg = 1 << pin;
    /* buffer input value */
    port->PINCFG[pin].bit.INEN = true;

    /* configure the pin's pull resistor state */
    switch (pushpull) {
        case GPIO_PULLDOWN:
            port->OUTCLR.reg = 1 << pin;
            port->PINCFG[pin].bit.PULLEN = true;
            break;
        case GPIO_PULLUP:
            port->OUTSET.reg = 1 << pin;
            port->PINCFG[pin].bit.PULLEN = true;
            break;
        case GPIO_NOPULL:
            port->PINCFG[pin].bit.PULLEN = false;
            break;
    }

    return 0;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    PortGroup* port = NULL;
    uint32_t pin = 0;
    uint32_t extint = 0;
    int res = 0;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            port = &GPIO_0_DEV;
            pin = GPIO_0_PIN;
            extint = GPIO_0_EXTINT;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            port = &GPIO_1_DEV;
            pin = GPIO_1_PIN;
            extint = GPIO_1_EXTINT;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            port = &GPIO_2_DEV;
            pin = GPIO_2_PIN;
            extint = GPIO_2_EXTINT;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            port = &GPIO_3_DEV;
            pin = GPIO_3_PIN;
            extint = GPIO_3_EXTINT;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            port = &GPIO_4_DEV;
            pin = GPIO_4_PIN;
            extint = GPIO_4_EXTINT;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            port = &GPIO_5_DEV;
            pin = GPIO_5_PIN;
            extint = GPIO_5_EXTINT;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            port = &GPIO_6_DEV;
            pin = GPIO_6_PIN;
            extint = GPIO_6_EXTINT;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            port = &GPIO_7_DEV;
            pin = GPIO_7_PIN;
            extint = GPIO_7_EXTINT;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            port = &GPIO_8_DEV;
            pin = GPIO_8_PIN;
            extint = GPIO_8_EXTINT;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            port = &GPIO_9_DEV;
            pin = GPIO_9_PIN;
            extint = GPIO_9_EXTINT;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            port = &GPIO_10_DEV;
            pin = GPIO_10_PIN;
            extint = GPIO_10_EXTINT;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            port = &GPIO_11_DEV;
            pin = GPIO_11_PIN;
            extint = GPIO_11_EXTINT;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            port = &GPIO_12_DEV;
            pin = GPIO_12_PIN;
            extint = GPIO_12_EXTINT;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            port = &GPIO_13_DEV;
            pin = GPIO_13_PIN;
            extint = GPIO_13_EXTINT;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            port = &GPIO_14_DEV;
            pin = GPIO_14_PIN;
            extint = GPIO_14_EXTINT;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            port = &GPIO_15_DEV;
            pin = GPIO_15_PIN;
            extint = GPIO_15_EXTINT;
            break;
#endif
        default:
            return -1;
    }

    if (port == NULL) {
        return -1;
    }



    if (pin < 16) {
        port->WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG \
                        | PORT_WRCONFIG_WRPMUX \
                        | PORT_WRCONFIG_PMUX(0x0) \
                        | PORT_WRCONFIG_PMUXEN \
                        | (1 << pin);
    }
    else {
        port->WRCONFIG.reg = PORT_WRCONFIG_HWSEL \
                        | PORT_WRCONFIG_WRPINCFG \
                        | PORT_WRCONFIG_WRPMUX \
                        | PORT_WRCONFIG_PMUX(0x0) \
                        | PORT_WRCONFIG_PMUXEN \
                        | ((1 << pin) >> 16);
    }

    /* configure pin as input */
    res = gpio_init_in(dev, pullup);
    if (res < 0) {
        return res;
    }

    /* Turn on APB clock */
    PM->APBAMASK.reg |= PM_APBAMASK_EIC;

    GCLK->CLKCTRL = (GCLK_CLKCTRL_Type){
        .bit.ID = EIC_GCLK_ID,
        .bit.GEN = 0,
        .bit.CLKEN = 1,
        .bit.WRTLOCK = 0
    };

    /* Setup interrupt */
    NVIC_SetPriority(EIC_IRQn, 10);
    NVIC_EnableIRQ(EIC_IRQn);

    /* save callback */
    gpio_config[extint].cb = cb;
    gpio_config[extint].arg = arg;

    /*Enable pin interrupt */
    EIC->INTFLAG.reg |= (1 << extint);
    EIC->INTENSET.reg = 1 << extint;
    EIC->WAKEUP.reg |= 1 << extint;

    /*Set config */
    uint8_t config_pos = (4 * (extint % 8));
    uint8_t config_reg    = extint / 8;
    /*Set flank detection */
    switch (flank) {
        case GPIO_FALLING:
            EIC->CONFIG[config_reg].reg
                        |= (EIC_CONFIG_SENSE0_FALL_Val << config_pos);
            break;
        case GPIO_RISING:
            EIC->CONFIG[config_reg].reg
                        |= (EIC_CONFIG_SENSE0_RISE_Val << config_pos);
            break;
        case GPIO_BOTH:
            EIC->CONFIG[config_reg].reg
                        |= (EIC_CONFIG_SENSE0_BOTH_Val << config_pos);
            break;
    }

    /*Enable external interrupts*/
    EIC->CTRL.bit.ENABLE = true;
    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            EIC->INTENSET.reg = 1 << GPIO_0_EXTINT;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            EIC->INTENSET.reg = 1 << GPIO_1_EXTINT;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            EIC->INTENSET.reg = 1 << GPIO_2_EXTINT;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            EIC->INTENSET.reg = 1 << GPIO_3_EXTINT;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            EIC->INTENSET.reg = 1 << GPIO_4_EXTINT;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            EIC->INTENSET.reg = 1 << GPIO_5_EXTINT;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            EIC->INTENSET.reg = 1 << GPIO_6_EXTINT;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            EIC->INTENSET.reg = 1 << GPIO_7_EXTINT;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            EIC->INTENSET.reg = 1 << GPIO_8_EXTINT;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            EIC->INTENSET.reg = 1 << GPIO_9_EXTINT;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            EIC->INTENSET.reg = 1 << GPIO_10_EXTINT;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            EIC->INTENSET.reg = 1 << GPIO_11_EXTINT;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            EIC->INTENSET.reg = 1 << GPIO_12_EXTINT;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            EIC->INTENSET.reg = 1 << GPIO_13_EXTINT;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            EIC->INTENSET.reg = 1 << GPIO_14_EXTINT;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            EIC->INTENSET.reg = 1 << GPIO_15_EXTINT;
            break;
#endif
    }
}

void gpio_irq_disable(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            EIC->INTENCLR.reg = 1 << GPIO_0_EXTINT;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            EIC->INTENCLR.reg = 1 << GPIO_1_EXTINT;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            EIC->INTENCLR.reg = 1 << GPIO_2_EXTINT;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            EIC->INTENCLR.reg = 1 << GPIO_3_EXTINT;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            EIC->INTENCLR.reg = 1 << GPIO_4_EXTINT;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            EIC->INTENCLR.reg = 1 << GPIO_5_EXTINT;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            EIC->INTENCLR.reg = 1 << GPIO_6_EXTINT;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            EIC->INTENCLR.reg = 1 << GPIO_7_EXTINT;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            EIC->INTENCLR.reg = 1 << GPIO_8_EXTINT;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            EIC->INTENCLR.reg = 1 << GPIO_9_EXTINT;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            EIC->INTENCLR.reg = 1 << GPIO_10_EXTINT;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            EIC->INTENCLR.reg = 1 << GPIO_11_EXTINT;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            EIC->INTENCLR.reg = 1 << GPIO_12_EXTINT;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            EIC->INTENCLR.reg = 1 << GPIO_13_EXTINT;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            EIC->INTENCLR.reg = 1 << GPIO_14_EXTINT;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            EIC->INTENCLR.reg = 1 << GPIO_15_EXTINT;
            break;
#endif
    }
}

int gpio_read(gpio_t dev)
{
    int res = -1;

    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            res = (&GPIO_0_DEV)->IN.reg & (1 << GPIO_0_PIN);
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            res = (&GPIO_1_DEV)->IN.reg & (1 << GPIO_1_PIN);
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            res = (&GPIO_2_DEV)->IN.reg & (1 << GPIO_2_PIN);
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            res = (&GPIO_3_DEV)->IN.reg & (1 << GPIO_3_PIN);
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            res = (&GPIO_4_DEV)->IN.reg & (1 << GPIO_4_PIN);
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            res = (&GPIO_5_DEV)->IN.reg & (1 << GPIO_5_PIN);
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            res = (&GPIO_6_DEV)->IN.reg & (1 << GPIO_6_PIN);
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            res = (&GPIO_7_DEV)->IN.reg & (1 << GPIO_7_PIN);
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            res = (&GPIO_8_DEV)->IN.reg & (1 << GPIO_8_PIN);
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            res = (&GPIO_9_DEV)->IN.reg & (1 << GPIO_9_PIN);
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            res = (&GPIO_10_DEV)->IN.reg & (1 << GPIO_10_PIN);
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            res = (&GPIO_11_DEV)->IN.reg & (1 << GPIO_11_PIN);
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            res = (&GPIO_12_DEV)->IN.reg & (1 << GPIO_12_PIN);
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            res = (&GPIO_13_DEV)->IN.reg & (1 << GPIO_13_PIN);
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            res = (&GPIO_14_DEV)->IN.reg & (1 << GPIO_14_PIN);
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            res = (&GPIO_15_DEV)->IN.reg & (1 << GPIO_15_PIN);
            break;
#endif
    }

    /* make sure we are not returning a negative value if bit 31 is set */
    if (res < -1) {
        res = 1;
    }

    return res;
}

void gpio_set(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            (&GPIO_0_DEV)->OUTSET.reg = 1 << GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            (&GPIO_1_DEV)->OUTSET.reg = 1 << GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            (&GPIO_2_DEV)->OUTSET.reg = 1 << GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            (&GPIO_3_DEV)->OUTSET.reg = 1 << GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            (&GPIO_4_DEV)->OUTSET.reg = 1 << GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            (&GPIO_5_DEV)->OUTSET.reg = 1 << GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            (&GPIO_6_DEV)->OUTSET.reg = 1 << GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            (&GPIO_7_DEV)->OUTSET.reg = 1 << GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            (&GPIO_8_DEV)->OUTSET.reg = 1 << GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            (&GPIO_9_DEV)->OUTSET.reg = 1 << GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            (&GPIO_10_DEV)->OUTSET.reg = 1 << GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            (&GPIO_11_DEV)->OUTSET.reg = 1 << GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            (&GPIO_12_DEV)->OUTSET.reg = 1 << GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            (&GPIO_13_DEV)->OUTSET.reg = 1 << GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            (&GPIO_14_DEV)->OUTSET.reg = 1 << GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            (&GPIO_15_DEV)->OUTSET.reg = 1 << GPIO_15_PIN;
            break;
#endif
    }
}

void gpio_clear(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            (&GPIO_0_DEV)->OUTCLR.reg = 1 << GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            (&GPIO_1_DEV)->OUTCLR.reg = 1 << GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            (&GPIO_2_DEV)->OUTCLR.reg = 1 << GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            (&GPIO_3_DEV)->OUTCLR.reg = 1 << GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            (&GPIO_4_DEV)->OUTCLR.reg = 1 << GPIO_4_PIN;
            break;

#endif
#if GPIO_5_EN
        case GPIO_5:
            (&GPIO_5_DEV)->OUTCLR.reg = 1 << GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            (&GPIO_6_DEV)->OUTCLR.reg = 1 << GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            (&GPIO_7_DEV)->OUTCLR.reg = 1 << GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            (&GPIO_8_DEV)->OUTCLR.reg = 1 << GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            (&GPIO_9_DEV)->OUTCLR.reg = 1 << GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            (&GPIO_10_DEV)->OUTCLR.reg = 1 << GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            (&GPIO_11_DEV)->OUTCLR.reg = 1 << GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            (&GPIO_12_DEV)->OUTCLR.reg = 1 << GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            (&GPIO_13_DEV)->OUTCLR.reg = 1 << GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            (&GPIO_14_DEV)->OUTCLR.reg = 1 << GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            (&GPIO_15_DEV)->OUTCLR.reg = 1 << GPIO_15_PIN;
            break;
#endif
    }
}

void gpio_toggle(gpio_t dev)
{
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            (&GPIO_0_DEV)->OUTTGL.reg = 1 << GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            (&GPIO_1_DEV)->OUTTGL.reg = 1 << GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            (&GPIO_2_DEV)->OUTTGL.reg = 1 << GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            (&GPIO_3_DEV)->OUTTGL.reg = 1 << GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
            (&GPIO_4_DEV)->OUTTGL.reg = 1 << GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
            (&GPIO_5_DEV)->OUTTGL.reg = 1 << GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
            (&GPIO_6_DEV)->OUTTGL.reg = 1 << GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            (&GPIO_7_DEV)->OUTTGL.reg = 1 << GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            (&GPIO_8_DEV)->OUTTGL.reg = 1 << GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            (&GPIO_9_DEV)->OUTTGL.reg = 1 << GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
            (&GPIO_10_DEV)->OUTTGL.reg = 1 << GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
            (&GPIO_11_DEV)->OUTTGL.reg = 1 << GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
            (&GPIO_12_DEV)->OUTTGL.reg = 1 << GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
            (&GPIO_13_DEV)->OUTTGL.reg = 1 << GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
            (&GPIO_14_DEV)->OUTTGL.reg = 1 << GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
            (&GPIO_15_DEV)->OUTTGL.reg = 1 << GPIO_15_PIN;
            break;
#endif
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

void isr_eic(void)
{
    uint16_t status = EIC->INTFLAG.reg;
    switch (status) {
        case EIC_INTFLAG_EXTINT0:
            gpio_config[0].cb(gpio_config[0].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT0;
            break;
        case EIC_INTFLAG_EXTINT1:
            gpio_config[1].cb(gpio_config[1].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT1;
            break;
        case EIC_INTFLAG_EXTINT2:
            gpio_config[2].cb(gpio_config[2].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT2;
            break;
        case EIC_INTFLAG_EXTINT3:
            gpio_config[3].cb(gpio_config[3].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT3;
            break;
        case EIC_INTFLAG_EXTINT4:
            gpio_config[4].cb(gpio_config[4].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT4;
            break;
        case EIC_INTFLAG_EXTINT5:
            gpio_config[5].cb(gpio_config[5].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT5;
            break;
        case EIC_INTFLAG_EXTINT6:
            gpio_config[6].cb(gpio_config[6].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT6;
            break;
        case EIC_INTFLAG_EXTINT7:
            gpio_config[7].cb(gpio_config[7].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT7;
            break;
        case EIC_INTFLAG_EXTINT8:
            gpio_config[8].cb(gpio_config[8].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT8;
            break;
        case EIC_INTFLAG_EXTINT9:
            gpio_config[9].cb(gpio_config[9].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT9;
            break;
        case EIC_INTFLAG_EXTINT10:
            gpio_config[10].cb(gpio_config[10].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT10;
            break;
        case EIC_INTFLAG_EXTINT11:
            gpio_config[11].cb(gpio_config[11].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT11;
            break;
        case EIC_INTFLAG_EXTINT12:
            gpio_config[12].cb(gpio_config[12].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT12;
            break;
        case EIC_INTFLAG_EXTINT13:
            gpio_config[13].cb(gpio_config[13].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT13;
            break;
        case EIC_INTFLAG_EXTINT14:
            gpio_config[14].cb(gpio_config[14].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT14;
            break;
        case EIC_INTFLAG_EXTINT15:
            gpio_config[15].cb(gpio_config[15].arg);
            EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT15;
            break;
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#endif /* GPIO_NUMOF */
