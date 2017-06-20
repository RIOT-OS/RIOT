/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
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
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Number of external interrupt lines
 */
#define NUMOF_IRQS                  (16U)

/**
 * @brief   Mask to get PINCFG reg value from mode value
 */
#define MODE_PINCFG_MASK            (0x06)

/**
 * @brief   Mapping of pins to EXTI lines, -1 means not EXTI possible
 */
static const int8_t exti_config[2][32] = {
#ifdef CPU_MODEL_SAMD21J18A
    { 0,  1,  2,  3,  4,  5,  6,  7, -1,  9, 10, 11, 12, 13, 14, 15,
      0,  1,  2,  3,  4,  5,  6,  7, 12, 13, -1, 15,  8, -1, 10, 11},
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
      0,  1, -1, -1, -1, -1,  6,  7, -1, -1, -1, -1, -1, -1, 14, 15},
#elif CPU_MODEL_SAMR21G18A
    {-1,  1, -1, -1,  4,  5,  6,  7, -1,  9, 10, 11, 12, 13, 14, 15,
     -1,  1,  2,  3, -1, -1,  6,  7, 12, 13, -1, 15,  8, -1, 10, 11},
    { 0, -1,  2,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      0,  1, -1, -1, -1, -1,  6,  7, -1, -1, -1, -1,  8, -1, -1, -1},
#endif
};

/**
 * @brief   Hold one interrupt context per interrupt line
 */
static gpio_isr_ctx_t gpio_config[NUMOF_IRQS];

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

void gpio_init_mux(gpio_t pin, gpio_mux_t mux)
{
    PortGroup* port = _port(pin);
    int pin_pos = _pin_pos(pin);

    port->PINCFG[pin_pos].reg |= PORT_PINCFG_PMUXEN;
    port->PMUX[pin_pos >> 1].reg &= ~(0xf << (4 * (pin_pos & 0x1)));
    port->PMUX[pin_pos >> 1].reg |=  (mux << (4 * (pin_pos & 0x1)));
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    PortGroup* port = _port(pin);
    int pin_pos = _pin_pos(pin);
    int pin_mask = _pin_mask(pin);

    /* make sure pin mode is applicable */
    if (mode > 0x7) {
        return -1;
    }

    /* set pin direction */
    if (mode & 0x2) {
        port->DIRCLR.reg = pin_mask;
    }
    else {
        port->DIRSET.reg = pin_mask;
    }

    /* configure the pin cfg and clear output register */
    port->PINCFG[pin_pos].reg = (mode & MODE_PINCFG_MASK);
    port->OUTCLR.reg = pin_mask;

    /* and set pull-up/pull-down if applicable */
    if (mode == 0x7) {
        port->OUTSET.reg = pin_mask;
    }

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
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
    /* configure pin as input and set MUX to peripheral function A */
    gpio_init(pin, mode);
    gpio_init_mux(pin, GPIO_MUX_A);
    /* enable clocks for the EIC module */
    PM->APBAMASK.reg |= PM_APBAMASK_EIC;
    GCLK->CLKCTRL.reg = (EIC_GCLK_ID |
                         GCLK_CLKCTRL_CLKEN |
                         GCLK_CLKCTRL_GEN_GCLK2);
    while (GCLK->STATUS.bit.SYNCBUSY) {}
    /* configure the active flank */
    EIC->CONFIG[exti >> 3].reg &= ~(0xf << ((exti & 0x7) * 4));
    EIC->CONFIG[exti >> 3].reg |=  (flank << ((exti & 0x7) * 4));
    /* enable the global EIC interrupt */
    NVIC_EnableIRQ(EIC_IRQn);
    /* clear interrupt flag and enable the interrupt line and line wakeup */
    EIC->INTFLAG.reg = (1 << exti);
    EIC->WAKEUP.reg |= (1 << exti);
    EIC->INTENSET.reg = (1 << exti);
    /* enable the EIC module*/
    EIC->CTRL.reg = EIC_CTRL_ENABLE;
    while (EIC->STATUS.reg & EIC_STATUS_SYNCBUSY) {}
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
        _port(pin)->OUTSET.reg = _pin_mask(pin);
    } else {
        _port(pin)->OUTCLR.reg = _pin_mask(pin);
    }
}

void isr_eic(void)
{
    for (unsigned i = 0; i < NUMOF_IRQS; i++) {
        if (EIC->INTFLAG.reg & (1 << i)) {
            EIC->INTFLAG.reg = (1 << i);
            if(EIC->INTENSET.reg & (1 << i)) {
                gpio_config[i].cb(gpio_config[i].arg);
            }
        }
    }
    cortexm_isr_end();
}
