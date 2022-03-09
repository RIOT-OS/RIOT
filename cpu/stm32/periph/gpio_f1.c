/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation for STM32F1
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "bitarithm.h"
#include "periph/gpio.h"
#include "periph_cpu.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

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
 * @brief   Extract the pin's port base address from the given pin identifier
 */
static inline GPIO_TypeDef *_port(gpio_t pin)
{
    return (GPIO_TypeDef *)(pin & ~(0x0f));
}

/**
 * @brief   Extract the port number from the given pin identifier
 *
 * Isolating bits 10 to 13 of the port base addresses leads to unique port
 * numbers.
 */
static inline int _port_num(gpio_t pin)
{
    return (((pin >> 10) & 0x0f) - 2);
}

/**
 * @brief   Get the pin number from the pin identifier, encoded in the LSB 4 bit
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

/**
 * @brief   Check if the given mode is some kind of input mdoe
 * @param[in]   mode    Mode to check
 * @retval  1           @p mode is GPIO_IN, GPIO_IN_PD, or GPIO_IN_PU
 * @retval  0           @p mode is not an input mode
 */
static inline int gpio_mode_is_input(gpio_mode_t mode)
{
    return !(mode & 3);
}

static inline void set_mode_or_af(GPIO_TypeDef *port, int pin_num,
                                  unsigned mode_or_af)
{
    volatile uint32_t *crl = (&port->CRL + (pin_num >> 3));
    uint32_t tmp = *crl;
    tmp &= ~(0xf << ((pin_num & 0x7) * 4));
    tmp |= ((mode_or_af & MODE_MASK) << ((pin_num & 0x7) * 4));
    *crl = tmp;
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    GPIO_TypeDef *port = _port(pin);
    int pin_num = _pin_num(pin);

    /* open-drain output with pull-up is not supported */
    if (mode == GPIO_OD_PU) {
        return -1;
    }

    /* enable the clock for the selected port */
    periph_clk_en(APB2, (RCC_APB2ENR_IOPAEN << _port_num(pin)));

    /* set pin mode */
    set_mode_or_af(port, pin_num, mode);

    /* For input modes, ODR controls pull up settings */
    if (gpio_mode_is_input(mode)) {
        if (mode == GPIO_IN_PU)
            port->ODR |= 1 << pin_num;
        else
            port->ODR &= ~(1 << pin_num);
    }

    return 0; /* all OK */
}

void gpio_init_af(gpio_t pin, gpio_af_t af)
{
    int pin_num = _pin_num(pin);
    GPIO_TypeDef *port = _port(pin);

    /* enable the clock for the selected port */
    periph_clk_en(APB2, (RCC_APB2ENR_IOPAEN << _port_num(pin)));
    /* configure the pin */
    set_mode_or_af(port, pin_num, af);
}

void gpio_init_analog(gpio_t pin)
{
    /* enable the GPIO port RCC */
    periph_clk_en(APB2, (RCC_APB2ENR_IOPAEN << _port_num(pin)));

    /* map the pin as analog input */
    int pin_num = _pin_num(pin);
    *(uint32_t *)(&_port(pin)->CRL + (pin_num >= 8)) &= ~(0xfl << (4 * (pin_num - ((pin_num >= 8) * 8))));
}

int gpio_read(gpio_t pin)
{
    GPIO_TypeDef *port = _port(pin);
    int pin_num = _pin_num(pin);

    if (*(uint32_t *)(&port->CRL + (pin_num >> 3)) & (0x3 << ((pin_num & 0x7) << 2))) {
        /* pin is output */
        return (port->ODR & (1 << pin_num));
    }
    else {
        /* or input */
        return (port->IDR & (1 << pin_num));
    }
}

void gpio_set(gpio_t pin)
{
    _port(pin)->BSRR = (1 << _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->BRR = (1 << _pin_num(pin));
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

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        _port(pin)->BSRR = (1 << _pin_num(pin));
    }
    else {
        _port(pin)->BRR = (1 << _pin_num(pin));
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    int pin_num = _pin_num(pin);

    /* disable interrupts on the channel we want to edit (just in case) */
    EXTI->IMR &= ~(1 << pin_num);
    /* configure pin as input */
    gpio_init(pin, mode);
    /* set callback */
    exti_ctx[pin_num].cb = cb;
    exti_ctx[pin_num].arg = arg;
    /* enable alternate function clock for the GPIO module */
    periph_clk_en(APB2, RCC_APB2ENR_AFIOEN);
    /* configure the EXTI channel */
    AFIO->EXTICR[pin_num >> 2] &= ~(0xf << ((pin_num & 0x3) * 4));
    AFIO->EXTICR[pin_num >> 2] |=  (_port_num(pin) << ((pin_num & 0x3) * 4));
    /* configure the active flank */
    EXTI->RTSR &= ~(1 << pin_num);
    EXTI->RTSR |=  ((flank & 0x1) << pin_num);
    EXTI->FTSR &= ~(1 << pin_num);
    EXTI->FTSR |=  ((flank >> 1) << pin_num);
    /* active global interrupt for the selected port */
    if (pin_num < 5) {
        NVIC_EnableIRQ(EXTI0_IRQn + pin_num);
    }
    else if (pin_num < 10) {
        NVIC_EnableIRQ(EXTI9_5_IRQn);
    }
    else {
        NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
    /* clear event mask */
    EXTI->EMR &= ~(1 << pin_num);
    /* unmask the pins interrupt channel */
    EXTI->IMR |= (1 << pin_num);
    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    EXTI->IMR |= (1 << _pin_num(pin));
}

void gpio_irq_disable(gpio_t pin)
{
    EXTI->IMR &= ~(1 << _pin_num(pin));
}

void isr_exti(void)
{
    /* read all pending interrupts wired to isr_exti */
    uint32_t pending_isr = (EXTI->PR & GPIO_ISR_CHAN_MASK);

    /* clear by writing a 1 */
    EXTI->PR = pending_isr;

    /* only generate soft interrupts against lines which have their IMR set */
    pending_isr &= EXTI->IMR;

    /* iterate over all set bits */
    uint8_t pin = 0;
    while (pending_isr) {
        pending_isr = bitarithm_test_and_clear(pending_isr, &pin);
        exti_ctx[pin].cb(exti_ctx[pin].arg);
    }
    cortexm_isr_end();
}
#endif /* MODULE_PERIPH_GPIO_IRQ */
