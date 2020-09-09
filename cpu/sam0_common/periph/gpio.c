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
#include "bitarithm.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Mask to get PINCFG reg value from mode value
 */
#define MODE_PINCFG_MASK            (0x06)

/**
 * @brief   The GCLK used for clocking EXTI
 */
#ifndef CONFIG_SAM0_GCLK_GPIO
#define CONFIG_SAM0_GCLK_GPIO       (SAM0_GCLK_MAIN)
#endif

#ifdef MODULE_PERIPH_GPIO_IRQ

/**
 * @brief   Number of external interrupt lines
 */
#ifdef CPU_COMMON_SAML1X
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

/**
 * @brief   Clock source for the External Interrupt Controller
 */
typedef enum {
    _EIC_CLOCK_FAST,
    _EIC_CLOCK_SLOW
} gpio_eic_clock_t;

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

void gpio_disable_mux(gpio_t pin)
{
    PortGroup* port = _port(pin);
    int pin_pos = _pin_pos(pin);

    port->PINCFG[pin_pos].reg &= ~PORT_PINCFG_PMUXEN;
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

#ifdef CPU_COMMON_SAMD21
#define EIC_SYNC() while (_EIC->STATUS.bit.SYNCBUSY)
#else
#define EIC_SYNC() while (_EIC->SYNCBUSY.bit.ENABLE)
#endif

static int _exti(gpio_t pin)
{
    unsigned port_num = ((pin >> 7) & 0x03);

    if (port_num >= ARRAY_SIZE(exti_config)) {
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
#ifdef CPU_COMMON_SAMD21
    /* enable clocks for the EIC module */
    PM->APBAMASK.reg |= PM_APBAMASK_EIC;
    GCLK->CLKCTRL.reg = EIC_GCLK_ID
                      | GCLK_CLKCTRL_CLKEN
                      | GCLK_CLKCTRL_GEN(CONFIG_SAM0_GCLK_GPIO);
    while (GCLK->STATUS.bit.SYNCBUSY) {}
#else /* CPU_COMMON_SAML21 */
    /* enable clocks for the EIC module */
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_EIC;
    GCLK->PCHCTRL[EIC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(CONFIG_SAM0_GCLK_GPIO);
    /* disable the EIC module*/
    _EIC->CTRLA.reg = 0;
    EIC_SYNC();
#endif
    /* configure the active flank */
    _EIC->CONFIG[exti >> 3].reg &= ~(0xf << ((exti & 0x7) * 4));
    _EIC->CONFIG[exti >> 3].reg |=  (flank << ((exti & 0x7) * 4));
    /* enable the global EIC interrupt */
#ifdef CPU_COMMON_SAML1X
    /* EXTI[4..7] are binded to EIC_OTHER_IRQn */
    NVIC_EnableIRQ((exti > 3 )? EIC_OTHER_IRQn : (EIC_0_IRQn + exti));
#elif defined(CPU_COMMON_SAMD5X)
    NVIC_EnableIRQ(EIC_0_IRQn + exti);
#else
    NVIC_EnableIRQ(EIC_IRQn);
#endif
    /* clear interrupt flag and enable the interrupt line and line wakeup */
    _EIC->INTFLAG.reg = (1 << exti);
    _EIC->INTENSET.reg = (1 << exti);
#ifdef CPU_COMMON_SAMD21
    _EIC->WAKEUP.reg |= (1 << exti);
    /* enable the EIC module*/
    _EIC->CTRL.reg = EIC_CTRL_ENABLE;
    EIC_SYNC();
#else /* CPU_COMMON_SAML21 */
    /* enable the EIC module*/
    _EIC->CTRLA.reg = EIC_CTRLA_ENABLE;
    EIC_SYNC();
#endif
    return 0;
}

inline static void reenable_eic(gpio_eic_clock_t clock) {
#if defined(CPU_COMMON_SAMD21)
    if (clock == _EIC_CLOCK_SLOW) {
        GCLK->CLKCTRL.reg = EIC_GCLK_ID
                          | GCLK_CLKCTRL_CLKEN
                          | GCLK_CLKCTRL_GEN(SAM0_GCLK_32KHZ);
    } else {
        GCLK->CLKCTRL.reg = EIC_GCLK_ID
                          | GCLK_CLKCTRL_CLKEN
                          | GCLK_CLKCTRL_GEN(CONFIG_SAM0_GCLK_GPIO);
    }
    while (GCLK->STATUS.bit.SYNCBUSY) {}
#else
    uint32_t ctrla_reg = EIC_CTRLA_ENABLE;

    EIC->CTRLA.reg = 0;
    EIC_SYNC();

    if (clock == _EIC_CLOCK_SLOW) {
        ctrla_reg |= EIC_CTRLA_CKSEL;
    }

    EIC->CTRLA.reg = ctrla_reg;
    EIC_SYNC();
#endif
}

void gpio_pm_cb_enter(int deep)
{
#if defined(PM_SLEEPCFG_SLEEPMODE_STANDBY)
    (void) deep;

    if (PM->SLEEPCFG.bit.SLEEPMODE == PM_SLEEPCFG_SLEEPMODE_STANDBY) {
        DEBUG_PUTS("gpio: switching EIC to slow clock");
        reenable_eic(_EIC_CLOCK_SLOW);
    }
#else
    if (deep) {
        DEBUG_PUTS("gpio: switching EIC to slow clock");
        reenable_eic(_EIC_CLOCK_SLOW);
    }
#endif
}

void gpio_pm_cb_leave(int deep)
{
#if defined(PM_SLEEPCFG_SLEEPMODE_STANDBY)
    (void) deep;

    if (PM->SLEEPCFG.bit.SLEEPMODE == PM_SLEEPCFG_SLEEPMODE_STANDBY) {
        DEBUG_PUTS("gpio: switching EIC to fast clock");
        reenable_eic(_EIC_CLOCK_FAST);
    }
#else
    if (deep) {
        DEBUG_PUTS("gpio: switching EIC to fast clock");
        reenable_eic(_EIC_CLOCK_FAST);
    }
#endif
}

void gpio_irq_enable(gpio_t pin)
{
    int exti = _exti(pin);
    if (exti == -1) {
        return;
    }

    /* clear stale interrupt */
    _EIC->INTFLAG.reg = (1 << exti);

    /* enable interrupt */
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

#if defined(CPU_COMMON_SAML1X)
void isr_eic_other(void)
#else
void isr_eic(void)
#endif
{
    /* read & clear interrupt flags */
    uint32_t state = _EIC->INTFLAG.reg;
    state &= (1 << NUMOF_IRQS) - 1;
    _EIC->INTFLAG.reg = state;

    /* execute interrupt callbacks */
    uint8_t pin = 0;
    while (state) {
        state = bitarithm_test_and_clear(state, &pin);
        gpio_config[pin].cb(gpio_config[pin].arg);
    }

    cortexm_isr_end();
}

#if defined(CPU_COMMON_SAML1X) || defined(CPU_COMMON_SAMD5X)

#define ISR_EICn(n)             \
void isr_eic ## n (void)        \
{                               \
    _EIC->INTFLAG.reg = 1 << n; \
    gpio_config[n].cb(gpio_config[n].arg); \
    cortexm_isr_end();          \
}

ISR_EICn(0)
ISR_EICn(1)
ISR_EICn(2)
ISR_EICn(3)
#if defined(CPU_COMMON_SAMD5X)
ISR_EICn(4)
ISR_EICn(5)
ISR_EICn(6)
ISR_EICn(7)
#if (NUMOF_IRQS > 8)
ISR_EICn(8)
ISR_EICn(9)
ISR_EICn(10)
ISR_EICn(11)
ISR_EICn(12)
ISR_EICn(13)
ISR_EICn(14)
ISR_EICn(15)
#endif /* NUMOF_IRQS > 8 */
#endif /* CPU_COMMON_SAMD5X */
#endif /* CPU_COMMON_SAML1X || CPU_COMMON_SAMD5X */

#else /* MODULE_PERIPH_GPIO_IRQ */

void gpio_pm_cb_enter(int deep)
{
    (void) deep;
}

void gpio_pm_cb_leave(int deep)
{
    (void) deep;
}

#endif /* MODULE_PERIPH_GPIO_IRQ */
