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
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Number of external interrupt lines
 */
#define NUMOF_IRQS                  (16U)

/**
 * @brief   Mapping of pins to EXTI lines, -1 means not EXTI possible
 */
static const int8_t exti_config[2][32] = {
    {-1,  1, -1, -1,  4,  5,  6,  7, -1,  9, 10, 11, 12, 13, 14, 15,
     -1,  1,  2,  3, -1, -1,  6,  7, 12, 13, -1, -1, -1, -1, -1, -1},
    { 0, -1,  2,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      0,  1, -1, -1, -1, -1,  6,  7, -1, -1, -1, 15,  8, -1, 10, 11},
};

/**
 * @brief   Datatype to use for saving the interrupt contexts
 */
typedef struct {
    gpio_cb_t cb;       /**< callback to call on GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
} gpio_exti_t;

/**
 * @brief   Hold one interrupt context per interrupt line
 */
static gpio_exti_t gpio_config[NUMOF_IRQS];

static inline PortGroup *_port(gpio_t pin)
{
    return (PortGroup *)(pin & ~(0x1f));
}

static inline int _pin_pos(gpio_t pin)
{
    return (pin & 0x1f);
}

static inline int _pin_mask(gpio_t pin)
{
    return (1 << _pin_pos(pin));
}

static int _exti(gpio_t pin)
{
    int port_num = ((pin >> 7) & 0x03);

    if (port_num > 1) {
        return -1;
    }
    return exti_config[port_num][_pin_pos(pin)];
}

int gpio_init(gpio_t pin, gpio_dir_t dir, gpio_pp_t pushpull)
{
    PortGroup* port = _port(pin);
    int pin_pos = _pin_pos(pin);
    int pin_mask = _pin_mask(pin);

    /* configure the pin's pull resistor and reset all other configuration */
    switch (pushpull) {
        case GPIO_PULLDOWN:
            port->OUTCLR.reg = pin_mask;
            port->PINCFG[pin_pos].reg = PORT_PINCFG_PULLEN;
            break;
        case GPIO_PULLUP:
            port->OUTSET.reg = pin_mask;
            port->PINCFG[pin_pos].reg = PORT_PINCFG_PULLEN;
            break;
        case GPIO_NOPULL:
            port->PINCFG[pin_pos].reg = 0;
            break;
    }
    /* set pin_pos direction */
    if (dir == GPIO_DIR_OUT) {
        if (pushpull == GPIO_PULLDOWN) {
            return -1;
        }
        port->DIRSET.reg = pin_mask;            /* configure as output */
    }
    else {
        port->DIRCLR.reg = pin_mask;            /* configure as input */
        port->PINCFG[pin_pos].reg |= PORT_PINCFG_INEN;
    }
    return 0;
}

int gpio_init_exti(gpio_t pin, gpio_pp_t pullup, gpio_flank_t flank,
                    gpio_cb_t cb, void *arg)
{
    int exti = _exti(pin);

    /* make sure EIC channel is valid */
    if (exti == -1) {
        return -1;
    }

    /* save callback */
    gpio_config[exti].cb = cb;
    gpio_config[exti].arg = arg;
    /* configure ping as input and set MUX to peripheral function A */
    gpio_init(pin, GPIO_DIR_IN, pullup);
    gpio_init_mux(pin, GPIO_MUX_A);
    /* enable clocks for the EIC module */
    PM->APBAMASK.reg |= PM_APBAMASK_EIC;
    GCLK->CLKCTRL.reg = (EIC_GCLK_ID | GCLK_CLKCTRL_CLKEN);
    /* enable the EIC module*/
    EIC->CTRL.reg = EIC_CTRL_ENABLE;
    /* configure the active flank */
    EIC->CONFIG[exti >> 3].reg &= ~(0xf << (exti & 0x7));
    switch (flank) {
        case GPIO_FALLING:
            EIC->CONFIG[exti >> 3].reg |= (EIC_CONFIG_SENSE0_FALL_Val <<
                                          (exti & 0x7));
            break;
        case GPIO_RISING:
            EIC->CONFIG[exti >> 3].reg |= (EIC_CONFIG_SENSE1_RISE_Val <<
                                          (exti & 0x7));
        case GPIO_BOTH:
        default:
            EIC->CONFIG[exti >> 3].reg |= (EIC_CONFIG_SENSE1_BOTH_Val <<
                                          (exti & 0x7));
            break;
    }

    /* enable the global EIC interrupt */
    NVIC_EnableIRQ(EIC_IRQn);
    /* clear interrupt flag and enable the interrupt line and line wakeup */
    EIC->INTFLAG.reg = (1 << exti);
    EIC->WAKEUP.reg |= (1 << exti);
    EIC->INTENSET.reg = (1 << exti);
    return 0;
}

int gpio_init_mux(gpio_t pin, gpio_mux_t mux)
{
    PortGroup* port = _port(pin);
    int pin_pos = _pin_pos(pin);

    port->PINCFG[pin_pos].reg |= PORT_PINCFG_PMUXEN;
    port->PMUX[pin_pos >> 1].reg &= ~(0xf << (4 * (pin_pos & 0x1)));
    port->PMUX[pin_pos >> 1].reg |=  (mux << (4 * (pin_pos & 0x1)));

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    int exti = _exti(pin);
    if (exti == -1) {
        return;
    }
    EIC->INTENSET.reg = (1 << exti);
}

void gpio_irq_disable(gpio_t pin)
{
    int exti = _exti(pin);
    if (exti == -1) {
        return;
    }
    EIC->INTENCLR.reg = (1 << exti);
}

int gpio_read(gpio_t pin)
{
    PortGroup *port = _port(pin);
    int mask = _pin_mask(pin);

    if (port->DIR.reg & mask) {
        return (port->OUT.reg & mask) ? 1 : 0;
    }
    else {
        return (port->IN.reg & mask) ? 1 : 0;
    }
}

void gpio_set(gpio_t pin)
{
    _port(pin)->OUTSET.reg = _pin_mask(pin);
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->OUTCLR.reg = _pin_mask(pin);
}

void gpio_toggle(gpio_t pin)
{
    _port(pin)->OUTTGL.reg = _pin_mask(pin);
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        gpio_set(pin);
    } else {
        gpio_clear(pin);
    }
}

void isr_eic(void)
{
    uint16_t status = EIC->INTFLAG.reg;
    for (int i = 0; i < NUMOF_IRQS; i++) {
        if (status & (1 << i)) {
            gpio_config[i].cb(gpio_config[i].arg);
            EIC->INTFLAG.reg = (1 << i);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
