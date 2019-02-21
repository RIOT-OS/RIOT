/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file        gpio.c
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief   Mask to get PINCFG reg value from mode value
 */
#define MODE_PINCFG_MASK            (0x06)

#ifdef MODULE_PERIPH_GPIO_IRQ

/**
 * @brief   Number of external interrupt lines
 */
#ifdef CPU_SAML1X
#define NUMOF_IRQS                  (8U)
#else
#define NUMOF_IRQS                  (16U)
#endif

/**
 * @brief   External Interrupts Controller selection macros
 */
#ifdef CPU_FAM_SAML11
#define _EIC EIC_SEC
#else
#define _EIC EIC
#endif

static gpio_isr_ctx_t gpio_config[NUMOF_IRQS];
#endif /* MODULE_PERIPH_GPIO_IRQ */

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

    /* configure the pin cfg */
    port->PINCFG[pin_pos].reg = (mode & MODE_PINCFG_MASK);

    /* and set pull-up/pull-down if applicable */
    if (mode == GPIO_IN_PU) {
        port->OUTSET.reg = pin_mask;
    }
    else if (mode == GPIO_IN_PD) {
        port->OUTCLR.reg = pin_mask;
    }

    return 0;
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

#ifdef MODULE_PERIPH_GPIO_IRQ
static int _exti(gpio_t pin)
{
    int port_num = ((pin >> 7) & 0x03);

    if (port_num > 1) {
        return -1;
    }
    return exti_config[port_num][_pin_pos(pin)];
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
#ifdef CPU_FAM_SAMD21
    /* enable clocks for the EIC module */
    PM->APBAMASK.reg |= PM_APBAMASK_EIC;
    /* SAMD21 used GCLK2 which is supplied by either the ultra low power
       internal or external 32 kHz */
    GCLK->CLKCTRL.reg = (EIC_GCLK_ID |
                         GCLK_CLKCTRL_CLKEN |
                         GCLK_CLKCTRL_GEN_GCLK2);
    while (GCLK->STATUS.bit.SYNCBUSY) {}
#else /* CPU_FAM_SAML21 */
    /* enable clocks for the EIC module */
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_EIC;
    GCLK->PCHCTRL[EIC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;
    /* disable the EIC module*/
    _EIC->CTRLA.reg = 0;
    while (_EIC->SYNCBUSY.reg & EIC_SYNCBUSY_ENABLE) {}
#endif
    /* configure the active flank */
    _EIC->CONFIG[exti >> 3].reg &= ~(0xf << ((exti & 0x7) * 4));
    _EIC->CONFIG[exti >> 3].reg |=  (flank << ((exti & 0x7) * 4));
    /* enable the global EIC interrupt */
#ifdef CPU_SAML1X
    /* EXTI[4..7] are binded to EIC_OTHER_IRQn */
    NVIC_EnableIRQ((exti > 3 )? EIC_OTHER_IRQn : (EIC_0_IRQn + exti));
#else
    NVIC_EnableIRQ(EIC_IRQn);
#endif
    /* clear interrupt flag and enable the interrupt line and line wakeup */
    _EIC->INTFLAG.reg = (1 << exti);
    _EIC->INTENSET.reg = (1 << exti);
#ifdef CPU_FAM_SAMD21
    _EIC->WAKEUP.reg |= (1 << exti);
    /* enable the EIC module*/
    _EIC->CTRL.reg = EIC_CTRL_ENABLE;
    while (_EIC->STATUS.reg & EIC_STATUS_SYNCBUSY) {}
#else /* CPU_FAM_SAML21 */
    /* enable the EIC module*/
    _EIC->CTRLA.reg = EIC_CTRLA_ENABLE;
    while (_EIC->SYNCBUSY.reg & EIC_SYNCBUSY_ENABLE) {}
#endif
    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    int exti = _exti(pin);
    if (exti == -1) {
        return;
    }
    _EIC->INTENSET.reg = (1 << exti);
}

void gpio_irq_disable(gpio_t pin)
{
    int exti = _exti(pin);
    if (exti == -1) {
        return;
    }
    _EIC->INTENCLR.reg = (1 << exti);
}

void isr_eic(void)
{
    for (unsigned i = 0; i < NUMOF_IRQS; i++) {
        if (_EIC->INTFLAG.reg & (1 << i)) {
            _EIC->INTFLAG.reg = (1 << i);
            gpio_config[i].cb(gpio_config[i].arg);
        }
    }
    cortexm_isr_end();
}

#ifdef CPU_SAML1X
void isr_eic0(void)
{
    isr_eic();
}

void isr_eic1(void)
{
    isr_eic();
}

void isr_eic2(void)
{
    isr_eic();
}

void isr_eic3(void)
{
    isr_eic();
}

void isr_eic_other(void)
{
    isr_eic();
}
#endif /* CPU_SAML1X */

#endif /* MODULE_PERIPH_GPIO_IRQ */
