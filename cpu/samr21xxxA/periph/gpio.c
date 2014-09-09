/*
 * Copyright (C) 2014 Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
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
 * @brief       Low-level GPIO driver implementation for SAMR21
 *
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"
/* guard file in case no GPIO devices are defined */
#if GPIO_NUMOF

typedef struct {
    gpio_cb_t cb;       /**< callback called from GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
} gpio_state_t;

/* static gpio_state_t gpio_config[GPIO_NUMOF]; */

//simplifing getting the gpio number from gpio_t
static uint32_t port_get_gpio_number(gpio_t dev)
{
    uint32_t pin = 0;
    switch (dev) {
#if GPIO_0_EN
        case GPIO_0:
            pin = GPIO_0_PIN;
            break;
#endif
#if GPIO_1_EN
        case GPIO_1:
            pin = GPIO_1_PIN;
            break;
#endif
#if GPIO_2_EN
        case GPIO_2:
            pin = GPIO_2_PIN;
            break;
#endif
#if GPIO_3_EN
        case GPIO_3:
            pin = GPIO_3_PIN;
            break;
#endif
#if GPIO_4_EN
        case GPIO_4:
               pin = GPIO_4_PIN;
            break;
#endif
#if GPIO_5_EN
        case GPIO_5:
               pin = GPIO_5_PIN;
            break;
#endif
#if GPIO_6_EN
        case GPIO_6:
               pin = GPIO_6_PIN;
            break;
#endif
#if GPIO_7_EN
        case GPIO_7:
            pin = GPIO_7_PIN;
            break;
#endif
#if GPIO_8_EN
        case GPIO_8:
            pin = GPIO_8_PIN;
            break;
#endif
#if GPIO_9_EN
        case GPIO_9:
            pin = GPIO_9_PIN;
            break;
#endif
#if GPIO_10_EN
        case GPIO_10:
             pin = GPIO_10_PIN;
            break;
#endif
#if GPIO_11_EN
        case GPIO_11:
             pin = GPIO_11_PIN;
            break;
#endif
#if GPIO_12_EN
        case GPIO_12:
             pin = GPIO_12_PIN;
            break;
#endif
#if GPIO_13_EN
        case GPIO_13:
             pin = GPIO_13_PIN;
            break;
#endif
#if GPIO_14_EN
        case GPIO_14:
             pin = GPIO_14_PIN;
            break;
#endif
#if GPIO_15_EN
        case GPIO_15:
             pin = GPIO_15_PIN;
            break;
#endif
    return pin;
}
}

//TODO: Atmel ASF
static inline PortGroup* system_pinmux_get_group_from_gpio_pin(
        const uint8_t gpio_pin)
{
    uint8_t port_index  = (gpio_pin / 128);
    uint8_t group_index = (gpio_pin / 32);

    /* Array of available ports. */
    Port *const ports[PORT_INST_NUM] = PORT_INSTS;
    if (port_index < PORT_INST_NUM) {
        return &(ports[port_index]->Group[group_index]);
    } else {
        return 0;
    }
}

int gpio_init_out(gpio_t dev, gpio_pp_t pushpull)
{
    uint32_t pin = port_get_gpio_number(dev);

    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    group->DIRSET.reg = (1 << (pin % 32));

    /* configure the pin's pull resistor state */
    //TODO: Does this matter in output mode?
    switch (pushpull) {
        case GPIO_PULLDOWN:
            return -1;
        case GPIO_PULLUP:
            group->PINCFG[pin % 32].bit.PULLEN = true;
            break;
        case GPIO_NOPULL:
            group->PINCFG[pin % 32].bit.PULLEN = false;
            break;
    }

    return 0;
}

int gpio_init_in(gpio_t dev, gpio_pp_t pushpull)
{
    uint32_t pin = port_get_gpio_number(dev);
    /* configure pin as input */
    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    group->DIRCLR.reg = (1 << (pin % 32)); //write a 1 to clear the bit in dir
    group->PINCFG[pin % 32].bit.INEN = true;
    /* configure the pin's pull resistor state */
    switch (pushpull) {
        //To enable PullDown, set out = 1, and pullen = 1
        case GPIO_PULLDOWN:
            group->OUTSET.reg = (1 << (pin % 32));
            group->PINCFG[pin % 32].bit.PULLEN = true;
            break;
        case GPIO_PULLUP:
            group->OUTCLR.reg = (1 << (pin % 32));
            group->PINCFG[pin % 32].bit.PULLEN = true;
            break;
        case GPIO_NOPULL:
            group->PINCFG[pin % 32].bit.PULLEN = false;
            break;
    }

    return 0;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    /* TODO: implement */
    return -1;
}

void gpio_irq_enable(gpio_t dev)
{
    /* TODO: implement */
}

void gpio_irq_disable(gpio_t dev)
{
    /* TODO: implement */
}

int gpio_read(gpio_t dev)
{
    uint32_t pin = port_get_gpio_number(dev);
    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    int result = group->IN.reg;
    return result;
}

void gpio_set(gpio_t dev)
{
    uint32_t pin = port_get_gpio_number(dev);
    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    group->OUTSET.reg = (1 << (pin %32)); //To set bit, set in set register
}

void gpio_clear(gpio_t dev)
{
    uint32_t pin = port_get_gpio_number(dev);
    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    group->OUTCLR.reg = (1 << (pin %32)); //To clear, set the bit in the clear register
}

void gpio_toggle(gpio_t dev)
{
    uint32_t pin = port_get_gpio_number(dev);
    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    group->OUTTGL.reg = (1 << (pin %32)); //To toggle, set bit in toggle register
}

void gpio_write(gpio_t dev, int value)
{
    if (value) {
        gpio_set(dev);
    } else {
        gpio_clear(dev);
    }
}

#endif /* GPIO_NUMOF */
