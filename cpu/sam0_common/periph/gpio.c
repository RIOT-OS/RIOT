/*
 * SPDX-FileCopyrightText: 2014-2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2015 FreshTemp, LLC.
 * SPDX-FileCopyrightText: 2022 SSV Software Systems GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file        gpio.c
 * @brief       Low-level GPIO driver implementation
 *
 * On processors that support Deep Sleep the External Interrupt Controller
 * will be off during Deep Sleep.
 * To wake the CPU up from Deep Sleep the RTC Tamper Detection will be
 * used instead.
 * Only a few pins (@ref rtc_tamper_pins) can be used for that purpose.
 *
 * Note that when configuring those pins as interrupt, the RTC/RTT will be
 * stopped briefly as the RTC configuration is enable protected.
 *
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * The GPIO implementation here support the PERIPH_GPIO_FAST_READ pseudomodule
 * on the cortex-m0/m23 based devices.  This trades an increase in power
 * consumption for a decrease in GPIO pin read latency. Enable this in your
 * makefile with:
 *
 * ```
 * FEATURES_OPTIONAL += periph_gpio_fast_read
 * ```
 *
 * This switches the GPIO reads to use the Cortex-M0+ single-cycle I/O port
 * instead of the regular APB access. The single-cycle I/O port is always used
 * for writes when it is available on the device.
 *
 * @}
 */

#include "cpu.h"
#include "bitarithm.h"
#include "pm_layered.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
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

/* The Cortex-m0 based ATSAM devices can use the Single-cycle I/O Port for GPIO.
 * When used, the gpio_t is mapped to the IOBUS area and must be mapped back to
 * the peripheral memory space for configuration access. When it is not
 * available, the _port_iobus() and _port() functions behave identical.
 */
static inline PortGroup *_port_iobus(gpio_t pin)
{
    return (PortGroup *)(pin & ~(0x1f));
}

static inline PortGroup *_port(gpio_t pin)
{
#ifdef MODULE_PERIPH_GPIO_FAST_READ
    /* Shift the PortGroup address back from the IOBUS region to the peripheral
     * region
     */
#ifdef PORT_IOBUS_SEC
    return (PortGroup *)((uintptr_t)_port_iobus(pin) -
                         (uintptr_t)PORT_IOBUS_SEC + (uintptr_t)PORT_SEC);
#else
    return (PortGroup *)((uintptr_t)_port_iobus(pin) -
                         (uintptr_t)PORT_IOBUS + (uintptr_t)PORT);
#endif /* PORT_IOBUS_SEC */
#else
    return _port_iobus(pin);
#endif
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
        if (IS_ACTIVE(MODULE_PERIPH_GPIO_FAST_READ)) {
            port->CTRL.reg |= pin_mask;
        }
        port->DIRCLR.reg = pin_mask;
    }
    else {
        if (IS_ACTIVE(MODULE_PERIPH_GPIO_FAST_READ)) {
            port->CTRL.reg &= ~pin_mask;
        }
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

bool gpio_read(gpio_t pin)
{
    PortGroup *port;
    int mask = _pin_mask(pin);

    if (IS_ACTIVE(MODULE_PERIPH_GPIO_FAST_READ)) {
        port = _port_iobus(pin);
    }
    else {
        port = _port(pin);
    }

    if (port->DIR.reg & mask) {
        return (port->OUT.reg & mask) ? 1 : 0;
    }
    else {
        return (port->IN.reg & mask) ? 1 : 0;
    }
}

void gpio_set(gpio_t pin)
{
    _port_iobus(pin)->OUTSET.reg = _pin_mask(pin);
}

void gpio_clear(gpio_t pin)
{
    _port_iobus(pin)->OUTCLR.reg = _pin_mask(pin);
}

void gpio_toggle(gpio_t pin)
{
    _port_iobus(pin)->OUTTGL.reg = _pin_mask(pin);
}

void gpio_write(gpio_t pin, bool value)
{
    if (value) {
        _port_iobus(pin)->OUTSET.reg = _pin_mask(pin);
    } else {
        _port_iobus(pin)->OUTCLR.reg = _pin_mask(pin);
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ

#ifdef CPU_COMMON_SAMD21
#define EIC_SYNC() while (_EIC->STATUS.reg & EIC_STATUS_SYNCBUSY)
#else
#define EIC_SYNC() while (_EIC->SYNCBUSY.reg & EIC_SYNCBUSY_ENABLE)
#endif

static int _exti(gpio_t pin)
{
    unsigned port_num = ((pin >> 7) & 0x03);

    if (port_num >= ARRAY_SIZE(exti_config)) {
        return -1;
    }
    return exti_config[port_num][_pin_pos(pin)];
}

/* check if an RTC tamper pin was configured as interrupt */
__attribute__ ((unused))
static bool _rtc_irq_enabled(void)
{
#if MODULE_PERIPH_GPIO_TAMPER_WAKE
    for (unsigned i = 0; i < ARRAY_SIZE(rtc_tamper_pins); ++i) {
        int exti = _exti(rtc_tamper_pins[i]);

        if (exti == -1) {
            continue;
        }

        if (_EIC->INTENSET.reg & (1 << exti)) {
            return true;
        }
    }
#endif
    return false;
}

static int _set_extwake(gpio_t pin, bool on)
{
#ifdef REG_RSTC_WKEN
    uint8_t idx = _pin_pos(pin);

    /* PA0 - PA7 are EXTWAKE pins */
    if ((idx > 7) || (_port(pin) != _port(GPIO_PIN(PA, 0)))) {
        return -1;
    }

    if (on) {
        RSTC->WKEN.reg |= 1 << idx;
    } else {
        RSTC->WKEN.reg &= ~(1 << idx);
    }

    return idx;
#else
    (void)pin;
    (void)on;
    return -1;
#endif
}

static void _init_rtc_pin(gpio_t pin, gpio_flank_t flank)
{
    if (IS_ACTIVE(MODULE_PERIPH_GPIO_TAMPER_WAKE)) {
        rtc_tamper_register(pin, flank);
    }
#ifdef REG_RSTC_WKEN
    int idx = _set_extwake(pin, true);
    if (idx < 0) {
        return;
    }

    if (flank == GPIO_RISING) {
        RSTC->WKPOL.reg |= 1 << idx;
    } else {
        RSTC->WKPOL.reg &= ~(1 << idx);
    }
#endif
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    int exti = _exti(pin);

    /* if it's a tamper pin configure wake from Deep Sleep */
    _init_rtc_pin(pin, flank);

    /* make sure EIC channel and callback are valid */
    if (exti == -1 || cb == NULL) {
        return -1;
    }

    if (gpio_config[exti].cb) {
        DEBUG("gpio: Warning - interrupt line %u already in use\n", exti);
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
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
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
#if IS_ACTIVE(MODULE_PM_LAYERED) && defined(SAM0_GPIO_PM_BLOCK)
    /* block pm mode if it hasn't been blocked before */
    if (!(_EIC->INTENSET.reg & (1 << exti))) {
        pm_block(SAM0_GPIO_PM_BLOCK);
    }
#endif
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
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
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
    unsigned mode = PM->SLEEPCFG.reg & PM_SLEEPCFG_SLEEPMODE_Msk;

    if (mode == PM_SLEEPCFG_SLEEPMODE_STANDBY) {
        DEBUG_PUTS("gpio: switching EIC to slow clock");
        reenable_eic(_EIC_CLOCK_SLOW);
    }
    else if (IS_ACTIVE(MODULE_PERIPH_GPIO_TAMPER_WAKE)
          && mode > PM_SLEEPCFG_SLEEPMODE_STANDBY
          && _rtc_irq_enabled()) {
        rtc_tamper_enable();
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

    if ((PM->SLEEPCFG.reg & PM_SLEEPCFG_SLEEPMODE_Msk) == PM_SLEEPCFG_SLEEPMODE_STANDBY) {
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
    _EIC->INTFLAG.reg = 1 << exti;

#if IS_ACTIVE(MODULE_PM_LAYERED) && defined(SAM0_GPIO_PM_BLOCK)
    /* unblock power mode */
    if (!(_EIC->INTENSET.reg & (1 << exti))) {
        pm_block(SAM0_GPIO_PM_BLOCK);
    }
#endif

    /* enable interrupt */
    _EIC->INTENSET.reg = 1 << exti;

    /* enable wake from deep sleep */
    _set_extwake(pin, true);
}

void gpio_irq_disable(gpio_t pin)
{
    int exti = _exti(pin);
    if (exti == -1) {
        return;
    }

#if IS_ACTIVE(MODULE_PM_LAYERED) && defined(SAM0_GPIO_PM_BLOCK)
    /* unblock power mode */
    if (_EIC->INTENSET.reg & (1 << exti)) {
        pm_unblock(SAM0_GPIO_PM_BLOCK);
    }
#endif

    /* disable interrupt */
    _EIC->INTENCLR.reg = 1 << exti;

    /* disable wake from deep sleep */
    _set_extwake(pin, false);
}

#if defined(CPU_COMMON_SAML1X)
void isr_eic_other(void)
#else
void isr_eic(void)
#endif
{
    /* read & clear interrupt flags */
    uint32_t state = _EIC->INTFLAG.reg & _EIC->INTENSET.reg;
    state &= EIC_INTFLAG_EXTINT_Msk;
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
