/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
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
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"

#include "periph_conf.h"
#include "saml21_periph.h"

#include "sched.h"
#include "thread.h"
#include "panic.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

/* guard file in case no GPIO devices are defined */
#if GPIO_NUMOF

typedef struct {
    gpio_cb_t cb;       /**< callback called from GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
} gpio_state_t;

/**
 * Shitloads of macros just to make sure we don't have to allocate
 * more than the actually enabled amount of exti states.
 * The result is an array with the amount of enabled EXIT lines times gpio_state_t,
 * and an array mapping any EXTI nr to either a unique slot or -1.
 *
 * Example: only GPIO_EXTI3_EN and GPIO_EXTI7_EN are set.
 * GPIO_EXTI_TOTAL will be "2"
 * gpio_config will be 2*sizeof(gpio_state_t)
 * gpio_exti_map will have the following content:
 *
 * { -1, -1, -1, 0, -1, -1, -1, 1, -1 , ...}
 *
 * This maps EXTI3 to gpio_config[0], EXTI7 to gpio_config[1].
 *
 * */
#define GPIO_EXTI0_POS 0
#define GPIO_EXTI1_POS GPIO_EXTI0_EN
#define GPIO_EXTI2_POS (GPIO_EXTI1_POS + GPIO_EXTI1_EN)
#define GPIO_EXTI3_POS (GPIO_EXTI2_POS + GPIO_EXTI2_EN)
#define GPIO_EXTI4_POS (GPIO_EXTI3_POS + GPIO_EXTI3_EN)
#define GPIO_EXTI5_POS (GPIO_EXTI4_POS + GPIO_EXTI4_EN)
#define GPIO_EXTI6_POS (GPIO_EXTI5_POS + GPIO_EXTI5_EN)
#define GPIO_EXTI7_POS (GPIO_EXTI6_POS + GPIO_EXTI6_EN)
#define GPIO_EXTI8_POS (GPIO_EXTI7_POS + GPIO_EXTI7_EN)
#define GPIO_EXTI9_POS (GPIO_EXTI8_POS + GPIO_EXTI8_EN)
#define GPIO_EXTI10_POS (GPIO_EXTI9_POS + GPIO_EXTI9_EN)
#define GPIO_EXTI11_POS (GPIO_EXTI10_POS + GPIO_EXTI10_EN)
#define GPIO_EXTI12_POS (GPIO_EXTI11_POS + GPIO_EXTI11_EN)
#define GPIO_EXTI13_POS (GPIO_EXTI12_POS + GPIO_EXTI12_EN)
#define GPIO_EXTI14_POS (GPIO_EXTI13_POS + GPIO_EXTI13_EN)
#define GPIO_EXTI15_POS (GPIO_EXTI14_POS + GPIO_EXTI14_EN)

const static int8_t gpio_exti_map[] = {
    GPIO_EXTI0_EN ? GPIO_EXTI0_POS : -1,
    GPIO_EXTI1_EN ? GPIO_EXTI1_POS : -1,
    GPIO_EXTI2_EN ? GPIO_EXTI2_POS : -1,
    GPIO_EXTI3_EN ? GPIO_EXTI3_POS : -1,
    GPIO_EXTI4_EN ? GPIO_EXTI4_POS : -1,
    GPIO_EXTI5_EN ? GPIO_EXTI5_POS : -1,
    GPIO_EXTI6_EN ? GPIO_EXTI6_POS : -1,
    GPIO_EXTI7_EN ? GPIO_EXTI7_POS : -1,
    GPIO_EXTI8_EN ? GPIO_EXTI8_POS : -1,
    GPIO_EXTI9_EN ? GPIO_EXTI9_POS : -1,
    GPIO_EXTI10_EN ? GPIO_EXTI10_POS : -1,
    GPIO_EXTI11_EN ? GPIO_EXTI11_POS : -1,
    GPIO_EXTI12_EN ? GPIO_EXTI12_POS : -1,
    GPIO_EXTI13_EN ? GPIO_EXTI13_POS : -1,
    GPIO_EXTI14_EN ? GPIO_EXTI14_POS : -1,
    GPIO_EXTI15_EN ? GPIO_EXTI15_POS : -1,
};

#define GPIO_EXTI_TOTAL (\
    GPIO_EXTI0_EN + \
    GPIO_EXTI1_EN + \
    GPIO_EXTI2_EN + \
    GPIO_EXTI3_EN + \
    GPIO_EXTI4_EN + \
    GPIO_EXTI5_EN + \
    GPIO_EXTI6_EN + \
    GPIO_EXTI7_EN + \
    GPIO_EXTI8_EN + \
    GPIO_EXTI9_EN + \
    GPIO_EXTI10_EN + \
    GPIO_EXTI11_EN + \
    GPIO_EXTI12_EN + \
    GPIO_EXTI13_EN + \
    GPIO_EXTI14_EN + \
    GPIO_EXTI15_EN)

static gpio_state_t gpio_config[GPIO_EXTI_TOTAL];

/**
 * @brief Convenience enum
 */
enum {
    PA,
    PB,
    PC,
    PD,
    PE,
    PF
};

/**
 * @brief Map from 0..GPIO_NUMOF-1 to actual GPIO pins
 *
 * This map/macro combo is used to map natural integers
 * starting by 0 upto GPIO_NUMOF-1 to the actual GPIO pin.
 *
 * GPIO pins are mapped into an implicit flat number space, e.g.,
 * Port C Pin 14 is mapped to (2*32 + 14)
 *
 * Pxn is supposed to be part of an enum set in "periph_conf.h".
 *
 * Example:
 *
 * enum {
 *     PA2,
 *     PB10,
 *     GPIO_UNUSED,
 *     PB5,
 *     PA3,
 *  };
 *
 * All pins > GPIO_UNUSED are mapped to the same spot so
 * they don't waste memory.
 */
#define DEFINE_GPIO_PIN(port, pin) [(port ## pin) > GPIO_UNUSED ? GPIO_UNUSED : (port ## pin)] = (port*32 + pin)
const uint8_t gpio_table[] = {
    DEFINE_GPIO_PIN(PA, 2),
    DEFINE_GPIO_PIN(PA, 4),
    DEFINE_GPIO_PIN(PA, 5),
    DEFINE_GPIO_PIN(PA, 6),
    DEFINE_GPIO_PIN(PA, 7),
    DEFINE_GPIO_PIN(PA, 3),
    DEFINE_GPIO_PIN(PA, 12),
    DEFINE_GPIO_PIN(PA, 13),
    DEFINE_GPIO_PIN(PB, 4),
    DEFINE_GPIO_PIN(PB, 5),
    DEFINE_GPIO_PIN(PB, 6),
    DEFINE_GPIO_PIN(PB, 7),
    DEFINE_GPIO_PIN(PB, 9),
    DEFINE_GPIO_PIN(PB, 10),
};

int gpio_init_mux(gpio_t dev, uint32_t mux)
{
    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    PortGroup* port = _port(dev);
    uint32_t pin = _pin(dev);

    /* WRCONFIG supports only half of the pin-space, selected by HWSEL
     * (half-word selection).
     * So here we either just use the lower half, or shift it by a
     * half-word and set the half-word select (HWSEL) flag.
     * */
    uint32_t hwmode = (pin < 16) ? (1 << pin) : (((1 << pin) >> 16) | PORT_WRCONFIG_HWSEL);

    port->WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG
                         | PORT_WRCONFIG_WRPMUX
                         | PORT_WRCONFIG_PMUX(mux)
                         | PORT_WRCONFIG_PMUXEN
                         /*| (mux ? PORT_WRCONFIG_PMUXEN : 0)*/
                         | hwmode;

    return 0;
}
int gpio_init_out(gpio_t dev, gpio_pp_t pushpull)
{
    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    PortGroup* port = _port(dev);
    uint32_t pin = _pin(dev);

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
    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    PortGroup* port = _port(dev);
    uint32_t pin = _pin(dev);

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
    if (dev >= GPIO_NUMOF) {
        return -1;
    }

    uint32_t pin = _pin(dev);
    uint32_t extint = pin % 16;

    int exti_pos = gpio_exti_map[extint];
    if (exti_pos == -1) {
        /* EXTI line not enabled. */
        return -1;
    }

    int res = 0;

    /* set pin mux */
    gpio_init_mux(dev, 0);

    /* configure pin as input */
    res = gpio_init_in(dev, pullup);
    if (res < 0) {
        return res;
    }

    /* Turn on APB clock */
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_EIC;

    /* setup GCLK for EIC */
    GCLK->PCHCTRL[EIC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;

    /* Setup interrupt */
    NVIC_SetPriority(EIC_IRQn, 10);
    NVIC_EnableIRQ(EIC_IRQn);

    /* save callback */
    gpio_config[exti_pos].cb = cb;
    gpio_config[exti_pos].arg = arg;

    /* must disable EIC before configuring. */
    EIC->CTRLA.bit.ENABLE = false;

    /*Enable pin interrupt */
    EIC->INTFLAG.reg |= (1 << extint);
    EIC->INTENSET.reg = 1 << extint;

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
    EIC->CTRLA.bit.ENABLE = true;
    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    uint32_t extint = _pin(dev) % 16;
    EIC->INTENSET.reg = 1 << extint;
}

void gpio_irq_disable(gpio_t dev)
{
    uint32_t extint = _pin(dev) % 16;
    EIC->INTENCLR.reg = 1 << extint;
}

int gpio_read(gpio_t dev)
{
    PortGroup* port = _port(dev);
    uint32_t pin = _pin(dev);
    int res = -1;

    res = port->IN.reg & (1 << pin);

    /* make sure we are not returning a negative value if bit 31 is set */
    if (res < -1) {
        res = 1;
    }

    return res;
}

void gpio_set(gpio_t dev)
{
    PortGroup* port = _port(dev);
    uint32_t pin = _pin(dev);

    port->OUTSET.reg = 1 << pin;
}

void gpio_clear(gpio_t dev)
{
    PortGroup* port = _port(dev);
    uint32_t pin = _pin(dev);
    port->OUTCLR.reg = 1 << pin;
}

void gpio_toggle(gpio_t dev)
{
    PortGroup* port = _port(dev);
    uint32_t pin = _pin(dev);
    port->OUTTGL.reg = 1 << pin;
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
    for (int i = 0; i < 16; i++) {
        if (status & (0x1<<i)) {
            int gpio_exti = gpio_exti_map[i];
            gpio_config[gpio_exti].cb(gpio_config[gpio_exti].arg);
            EIC->INTFLAG.reg = 0x1 << i;
            break;
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

#endif /* GPIO_NUMOF */
