/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *               2015 Hamburg University of Applied Sciences
 *               2017 Inria
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Katja Kirstein <katja.kirstein@haw-hamburg.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#ifndef CPU_FAM_STM32F1

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"

/**
 * @brief   The STM32F0 family has 16 external interrupt lines
 */
#define EXTI_NUMOF          (16U)

/**
 * @brief   Allocate memory for one callback and argument per EXTI channel
 */
static gpio_isr_ctx_t isr_ctx[EXTI_NUMOF];

/**
 * @brief   Extract the port base address from the given pin identifier
 */
static inline GPIO_TypeDef *_port(gpio_t pin)
{
    return (GPIO_TypeDef *)(pin & ~(0x0f));
}

/**
 * @brief   Extract the port number form the given identifier
 *
 * The port number is extracted by looking at bits 10, 11, 12, 13 of the base
 * register addresses.
 */
static inline int _port_num(gpio_t pin)
{
    return ((pin >> 10) & 0x0f);
}

/**
 * @brief   Extract the pin number from the last 4 bit of the pin identifier
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    GPIO_TypeDef *port = _port(pin);
    int pin_num = _pin_num(pin);

    /* enable clock */
#if defined(CPU_FAM_STM32F0) || defined (CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L1)
    periph_clk_en(AHB, (RCC_AHBENR_GPIOAEN << _port_num(pin)));
#elif defined (CPU_FAM_STM32L0)
    periph_clk_en(IOP, (RCC_IOPENR_GPIOAEN << _port_num(pin)));
#else
    periph_clk_en(AHB1, (RCC_AHB1ENR_GPIOAEN << _port_num(pin)));
#endif

    /* set mode */
    port->MODER &= ~(0x3 << (2 * pin_num));
    port->MODER |=  ((mode & 0x3) << (2 * pin_num));
    /* set pull resistor configuration */
    port->PUPDR &= ~(0x3 << (2 * pin_num));
    port->PUPDR |=  (((mode >> 2) & 0x3) << (2 * pin_num));
    /* set output mode */
    port->OTYPER &= ~(1 << pin_num);
    port->OTYPER |=  (((mode >> 4) & 0x1) << pin_num);
    /* finally set pin speed to maximum and reset output */
    port->OSPEEDR |= (3 << (2 * pin_num));
    port->BSRR = (1 << (pin_num + 16));

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    int pin_num = _pin_num(pin);
    int port_num = _port_num(pin);

    /* set callback */
    isr_ctx[pin_num].cb = cb;
    isr_ctx[pin_num].arg = arg;

    /* enable clock of the SYSCFG module for EXTI configuration */
#ifdef CPU_FAN_STM32F0
    periph_clk_en(APB2, RCC_APB2ENR_SYSCFGCOMPEN);
#else
    periph_clk_en(APB2, RCC_APB2ENR_SYSCFGEN);
#endif

    /* initialize pin as input */
    gpio_init(pin, mode);

    /* enable global pin interrupt */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32L0)
    if (pin_num < 2) {
        NVIC_EnableIRQ(EXTI0_1_IRQn);
    }
    else if (pin_num < 4) {
        NVIC_EnableIRQ(EXTI2_3_IRQn);
    }
    else {
        NVIC_EnableIRQ(EXTI4_15_IRQn);
    }
#else
    if (pin_num < 5) {
        NVIC_EnableIRQ(EXTI0_IRQn + pin_num);
    }
    else if (pin_num < 10) {
        NVIC_EnableIRQ(EXTI9_5_IRQn);
    }
    else {
        NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
#endif
    /* configure the active flank */
    EXTI->RTSR &= ~(1 << pin_num);
    EXTI->RTSR |=  ((flank & 0x1) << pin_num);
    EXTI->FTSR &= ~(1 << pin_num);
    EXTI->FTSR |=  ((flank >> 1) << pin_num);
    /* enable specific pin as exti sources */
    SYSCFG->EXTICR[pin_num >> 2] &= ~(0xf << ((pin_num & 0x03) * 4));
    SYSCFG->EXTICR[pin_num >> 2] |= (port_num << ((pin_num & 0x03) * 4));

    /* clear any pending requests */
    EXTI->PR = (1 << pin);
    /* unmask the pins interrupt channel */
    EXTI->IMR |= (1 << pin);

    return 0;
}

void gpio_init_af(gpio_t pin, gpio_af_t af)
{
    GPIO_TypeDef *port = _port(pin);
    uint32_t pin_num = _pin_num(pin);

    /* set pin to AF mode */
    port->MODER &= ~(3 << (2 * pin_num));
    port->MODER |= (2 << (2 * pin_num));
    /* set selected function */
    port->AFR[(pin_num > 7) ? 1 : 0] &= ~(0xf << ((pin_num & 0x07) * 4));
    port->AFR[(pin_num > 7) ? 1 : 0] |= (af << ((pin_num & 0x07) * 4));
}

void gpio_init_analog(gpio_t pin)
{
    /* enable clock, needed as this function can be used without calling
     * gpio_init first */
#if defined(CPU_FAM_STM32F0) || defined (CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L1)
    periph_clk_en(AHB, (RCC_AHBENR_GPIOAEN << _port_num(pin)));
#elif defined (CPU_FAM_STM32L0)
    periph_clk_en(IOP, (RCC_IOPENR_GPIOAEN << _port_num(pin)));
#else
    periph_clk_en(AHB1, (RCC_AHB1ENR_GPIOAEN << _port_num(pin)));
#endif
    /* set to analog mode */
    _port(pin)->MODER |= (0x3 << (2 * _pin_num(pin)));
}

void gpio_irq_enable(gpio_t pin)
{
    EXTI->IMR |= (1 << _pin_num(pin));
}

void gpio_irq_disable(gpio_t pin)
{
    EXTI->IMR &= ~(1 << _pin_num(pin));
}

int gpio_read(gpio_t pin)
{
    if (_port(pin)->MODER & (0x3 << (_pin_num(pin) * 2))) {
        return _port(pin)->ODR & (1 << _pin_num(pin));
    }
    else {
        return _port(pin)->IDR & (1 << _pin_num(pin));
    }
}

void gpio_set(gpio_t pin)
{
    _port(pin)->BSRR = (1 << _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->BSRR = (1 << (_pin_num(pin) + 16));
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        gpio_clear(pin);
    } else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        gpio_set(pin);
    } else {
        gpio_clear(pin);
    }
}

void isr_exti(void)
{
    /* only generate interrupts against lines which have their IMR set */
    uint32_t pending_isr = (EXTI->PR & EXTI->IMR);
    for (size_t i = 0; i < EXTI_NUMOF; i++) {
        if (pending_isr & (1 << i)) {
            EXTI->PR = (1 << i);        /* clear by writing a 1 */
            isr_ctx[i].cb(isr_ctx[i].arg);
        }
    }
    cortexm_isr_end();
}

#else
typedef int dont_be_pedantic;
#endif
