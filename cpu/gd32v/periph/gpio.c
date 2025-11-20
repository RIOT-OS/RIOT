/*
 * SPDX-FileCopyrightText: 2014-2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2020 Koen Zandberg <koen@bergzand.net>
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht <gunar@schorcht.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_gd32v
 * @{
 *
 * @file
 * @brief           Low-level GPIO driver implementation for GD32V
 *
 * @author          Koen Zandberg <koen@bergzand.net>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author          Gunar Schorcht
 */

#include "assert.h"
#include "bitarithm.h"
#include "cpu.h"
#include "clic.h"
#include "log.h"
#include "periph_cpu.h"
#include "periph/gpio.h"

/**
 * @brief   Extract information from mode parameter
 */
#define MODE_MASK                       (0x0f)
#define ODR_POS                         (4U)

#ifdef MODULE_PERIPH_GPIO_IRQ
/**
 * @brief   Number of available external interrupt lines
 */
#define GPIO_ISR_CHAN_NUMOF             (16U)
#define GPIO_ISR_CHAN_MASK              (0xFFFF)

/**
 * @brief   Allocate memory for one callback and argument per EXTI channel
 */
static gpio_isr_ctx_t exti_ctx[GPIO_ISR_CHAN_NUMOF];
#endif /* MODULE_PERIPH_GPIO_IRQ */

/**
 * @brief   Extract the port base address from the given pin identifier
 */
static inline GPIO_Type *_port(gpio_t pin)
{
    return (GPIO_Type *)(pin & ~(0x0f));
}

/**
 * @brief   Extract the port number form the given identifier
 *
 * The port number is extracted by looking at bits 10, 11, 12, 13 of the base
 * register addresses.
 */
static inline int _port_num(gpio_t pin)
{
    return (((pin >> 10) & 0x0f) - 2);
}

/**
 * @brief   Extract the pin number from the last 4 bit of the pin identifier
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

static inline void _port_enable_clock(gpio_t pin)
{
    periph_clk_en(APB2, (RCU_APB2EN_PAEN_Msk << _port_num(pin)));
}

/**
 * @brief   Check if the given mode is some kind of input mode
 * @param[in]   mode    Mode to check
 * @retval  1           @p mode is GPIO_IN, GPIO_IN_PD, or GPIO_IN_PU
 * @retval  0           @p mode is not an input mode
 */
static inline int gpio_mode_is_input(gpio_mode_t mode)
{
    return !(mode & 3);
}

static inline void _set_mode_or_af(GPIO_Type *port, unsigned pin_num,
                                   unsigned mode_or_af)
{
    volatile uint32_t *crl = (&port->CTL0 + (pin_num >> 3));
    uint32_t tmp = *crl;

    tmp &= ~(0xf << ((pin_num & 0x7) * 4));
    tmp |= ((mode_or_af & MODE_MASK) << ((pin_num & 0x7) * 4));
    *crl = tmp;
}

static inline bool _pin_is_output(GPIO_Type *port, unsigned pin_num)
{
    uint32_t reg = *(uint32_t *)(&port->CTL0 + (pin_num >> 3));

    return reg & (0x3 << ((pin_num & 0x7) << 2));
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    GPIO_Type *port = _port(pin);
    unsigned pin_num = _pin_num(pin);

    /* open-drain output with pull-up is not supported */
    if (mode == GPIO_OD_PU) {
        return -1;
    }

    /* enable the clock for the selected port */
    _port_enable_clock(pin);

    /* set pin mode */
    _set_mode_or_af(port, pin_num, mode);

    /* For input modes, ODR controls pull up settings */
    if (gpio_mode_is_input(mode)) {
        if (mode == GPIO_IN_PU) {
            cpu_reg_enable_bits(&port->OCTL, 1 << pin_num);
        }
        else {
            cpu_reg_disable_bits(&port->OCTL, 1 << pin_num);
        }
    }

    return 0; /* all OK */
}

void gpio_init_af(gpio_t pin, gpio_af_t af)
{
    GPIO_Type *port = _port(pin);
    unsigned pin_num = _pin_num(pin);

    /* enable the clock for the selected port */
    _port_enable_clock(pin);
    /* configure the pin */
    _set_mode_or_af(port, pin_num, af);
}

void gpio_init_analog(gpio_t pin)
{
    /* enable the GPIO port RCC */
    _port_enable_clock(pin);

    /* map the pin as analog input */
    int pin_num = _pin_num(pin);

    volatile uint32_t *pin_reg = (uint32_t *)(&_port(pin)->CTL0 + (pin_num >= 8));
    *pin_reg &= ~(0xfl << (4 * (pin_num - ((pin_num >= 8) * 8))));
}

bool gpio_read(gpio_t pin)
{
    GPIO_Type *port = _port(pin);
    unsigned pin_num = _pin_num(pin);

    if (_pin_is_output(port, pin_num)) {
        /* pin is output */
        return (port->OCTL & (1 << pin_num));
    }
    else {
        /* or input */
        return (port->ISTAT & (1 << pin_num));
    }
}

void gpio_set(gpio_t pin)
{
    _port(pin)->BOP = (1 << _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->BC = (1 << _pin_num(pin));
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        gpio_clear(pin);
    }
    else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, bool value)
{
    if (value) {
        gpio_set(pin);
    }
    else {
        gpio_clear(pin);
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ

/* Forward declaration of ISR */
static void _gpio_isr(unsigned irqn);

static inline unsigned _irq_num(unsigned pin_num)
{
    if (pin_num < 5) {
        return EXTI0_IRQn + pin_num;
    }
    if (pin_num < 10) {
        return EXTI5_9_IRQn;
    }
    return EXTI10_15_IRQn;
}

#ifndef NDEBUG
uint8_t exti_line_port[GPIO_ISR_CHAN_NUMOF];
#endif

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    assert(cb != NULL);

    int pin_num = _pin_num(pin);
    int port_num = _port_num(pin);

    /* disable interrupts on the channel we want to edit (just in case) */
    EXTI->INTEN &= ~(1 << pin_num);

    /* configure pin as input */
    gpio_init(pin, mode);

#ifndef NDEBUG
    /* GD32V has 16 EXTI lines for GPIO interrupts, where all ports share
     * the same EXTI line for the same pin. That means the pin PA<n> shares
     * the EXTI line with PB<n>, PC<n>, PD<n> and PE<n>. */
    if ((exti_ctx[pin_num].cb != 0) && (exti_line_port[pin_num] != port_num)) {
        LOG_ERROR("EXTI line for GPIO_PIN(%u, %u) is used by GPIO_PIN(%u, %u).\n",
                  port_num, pin_num, exti_line_port[pin_num], pin_num);
        assert(0);
    }
    exti_line_port[pin_num] = port_num;
#endif

    /* set callback */
    exti_ctx[pin_num].cb = cb;
    exti_ctx[pin_num].arg = arg;

    /* enable alternate function clock for the GPIO module */
    periph_clk_en(APB2, RCU_APB2EN_AFEN_Msk);

    /* configure the EXTI channel */
    volatile uint32_t *afio_exti_ss = &AFIO->EXTISS0 + (pin_num >> 2);

    *afio_exti_ss &= ~(0xfUL << ((pin_num & 0x03) * 4));
    *afio_exti_ss |= (uint32_t)port_num << ((pin_num & 0x03) * 4);

    /* configure the active flank */
    EXTI->RTEN &= ~(1 << pin_num);
    EXTI->RTEN |= ((flank & 0x1) << pin_num);
    EXTI->FTEN &= ~(1 << pin_num);
    EXTI->FTEN |= ((flank >> 1) << pin_num);

    /* clear any pending requests */
    EXTI->PD = (1 << pin_num);

    /* enable global pin interrupt */
    unsigned irqn = _irq_num(pin_num);

    clic_set_handler(irqn, _gpio_isr);
    clic_enable_interrupt(irqn, CPU_DEFAULT_IRQ_PRIO);

    /* unmask the pins interrupt channel */
    EXTI->INTEN |= (1 << pin_num);

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    EXTI->INTEN |= (1 << _pin_num(pin));
}

void gpio_irq_disable(gpio_t pin)
{
    EXTI->INTEN &= ~(1 << _pin_num(pin));
}

static void _gpio_isr(unsigned irqn)
{
    (void)irqn;

    /* read all pending interrupts wired to isr_exti */
    uint32_t pending_isr = EXTI->PD & GPIO_ISR_CHAN_MASK;

    /* clear by writing a 1 */
    EXTI->PD = pending_isr;

    /* only generate soft interrupts against lines which have their IMR set */
    pending_isr &= EXTI->INTEN;

    /* iterate over all set bits */
    uint8_t pin = 0;
    while (pending_isr) {
        pending_isr = bitarithm_test_and_clear(pending_isr, &pin);
        exti_ctx[pin].cb(exti_ctx[pin].arg);
    }
}
#endif

/** @} */
