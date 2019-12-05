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
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Katja Kirstein <katja.kirstein@haw-hamburg.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */


#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"

/* this implementation is not valid for the stm32f1 */
#ifndef CPU_FAM_STM32F1

#ifdef MODULE_PERIPH_GPIO_IRQ
/**
 * @brief   The STM32F0 family has 16 external interrupt lines
 */
#define EXTI_NUMOF          (16U)

/**
 * @brief   Allocate memory for one callback and argument per EXTI channel
 */
static gpio_isr_ctx_t isr_ctx[EXTI_NUMOF];
#endif /* MODULE_PERIPH_GPIO_IRQ */

/**
 * @brief   Extract the port number form the given device address
 *
 * The port number is extracted by looking at bits 10, 11, 12, 13 of the base
 * register addresses.
 */
static inline int _port_num(gpio_port_t port)
{
    return ((port.reg >> 10) & 0x0f);
}

/**
 * @brief   Convert port register address to port register structure
 */
static inline GPIO_TypeDef *_port(gpio_port_t port)
{
    return (GPIO_TypeDef *)port.reg;
}

int gpio_cpu_init(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode)
{
    /* enable clock */
#if defined(CPU_FAM_STM32F0) || defined (CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L1)
    periph_clk_en(AHB, (RCC_AHBENR_GPIOAEN << _port_num(port)));
#elif defined (CPU_FAM_STM32L0)
    periph_clk_en(IOP, (RCC_IOPENR_GPIOAEN << _port_num(port)));
#elif defined (CPU_FAM_STM32L4)
    periph_clk_en(AHB2, (RCC_AHB2ENR_GPIOAEN << _port_num(port)));
#ifdef PWR_CR2_IOSV
    if (_port(port) == GPIOG) {
        /* Port G requires external power supply */
        periph_clk_en(APB1, RCC_APB1ENR1_PWREN);
        PWR->CR2 |= PWR_CR2_IOSV;
    }
#endif /* PWR_CR2_IOSV */
#else
    periph_clk_en(AHB1, (RCC_AHB1ENR_GPIOAEN << _port_num(port)));
#endif
    /* set mode */
    _port(port)->MODER &= ~(0x3 << (pin << 1));
    _port(port)->MODER |=  ((mode & 0x3) << (pin << 1));
    /* set pull resistor configuration */
    _port(port)->PUPDR &= ~(0x3 << (pin << 1));
    _port(port)->PUPDR |=  (((mode >> 2) & 0x3) << (pin << 1));
    /* set output mode */
    _port(port)->OTYPER &= ~(1 << pin);
    _port(port)->OTYPER |=  (((mode >> 4) & 0x1) << pin);
    /* set pin speed to maximum */
    _port(port)->OSPEEDR |= (3 << (pin << 1));

    return 0;
}

void gpio_init_af(gpio_t gpio, gpio_af_t af)
{
    GPIO_TypeDef *port = _port(GPIO_PORT(gpio));
    gpio_pin_t pin = gpio.pin;

    /* set pin to AF mode */
    port->MODER &= ~(3 << (pin << 1));
    port->MODER |= (2 << (pin << 1));
    /* set selected function */
    port->AFR[(pin > 7) ? 1 : 0] &= ~(0xf << ((pin & 0x07) << 2));
    port->AFR[(pin > 7) ? 1 : 0] |= (af << ((pin & 0x07) << 2));
}

void gpio_init_analog(gpio_t gpio)
{
    /* enable clock, needed as this function can be used without calling
     * gpio_init first */
#if defined(CPU_FAM_STM32F0) || defined (CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L1)
    periph_clk_en(AHB, (RCC_AHBENR_GPIOAEN << _port_num(GPIO_PORT(gpio))));
#elif defined (CPU_FAM_STM32L0)
    periph_clk_en(IOP, (RCC_IOPENR_GPIOAEN << _port_num(GPIO_PORT(gpio))));
#elif defined (CPU_FAM_STM32L4)
    periph_clk_en(AHB2, (RCC_AHB2ENR_GPIOAEN << _port_num(GPIO_PORT(gpio))));
#else
    periph_clk_en(AHB1, (RCC_AHB1ENR_GPIOAEN << _port_num(GPIO_PORT(gpio))));
#endif
    /* set to analog mode */
    _port(GPIO_PORT(gpio))->MODER |= (0x3 << ((uint32_t)gpio.pin << 1));
}

gpio_mask_t gpio_cpu_read(gpio_port_t port)
{
    return _port(port)->IDR;
}

void gpio_cpu_set(gpio_port_t port, gpio_mask_t pins)
{
    _port(port)->BSRR = pins;
}

void gpio_cpu_clear(gpio_port_t port, gpio_mask_t pins)
{
    _port(port)->BSRR = ((uint32_t)pins << 16);
}

void gpio_cpu_toggle(gpio_port_t port, gpio_mask_t pins)
{
    _port(port)->ODR = _port(port)->ODR ^ pins;
}

void gpio_cpu_write(gpio_port_t port, gpio_mask_t values)
{
    _port(port)->ODR = values;
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int gpio_cpu_init_int(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode,
                      gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int port_num = _port_num(port);

    /* set callback */
    isr_ctx[pin].cb = cb;
    isr_ctx[pin].arg = arg;

    /* enable clock of the SYSCFG module for EXTI configuration */
    periph_clk_en(APB2, RCC_APB2ENR_SYSCFGEN);

    /* initialize pin as input */
    gpio_cpu_init(port, pin, mode);

    /* enable global pin interrupt */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32L0)
    if (pin < 2) {
        NVIC_EnableIRQ(EXTI0_1_IRQn);
    }
    else if (pin < 4) {
        NVIC_EnableIRQ(EXTI2_3_IRQn);
    }
    else {
        NVIC_EnableIRQ(EXTI4_15_IRQn);
    }
#else
    if (pin < 5) {
        NVIC_EnableIRQ(EXTI0_IRQn + pin);
    }
    else if (pin < 10) {
        NVIC_EnableIRQ(EXTI9_5_IRQn);
    }
    else {
        NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
#endif
    /* configure the active flank */
    EXTI->RTSR &= ~(1 << pin);
    EXTI->RTSR |=  ((flank & 0x1) << pin);
    EXTI->FTSR &= ~(1 << pin);
    EXTI->FTSR |=  ((flank >> 1) << pin);
    /* enable specific pin as exti sources */
    SYSCFG->EXTICR[pin >> 2] &= ~(0xf << ((pin & 0x03) * 4));
    SYSCFG->EXTICR[pin >> 2] |= (port_num << ((pin & 0x03) * 4));

    /* clear any pending requests */
    EXTI->PR = (1 << pin);
    /* unmask the pins interrupt channel */
    EXTI->IMR |= (1 << pin);

    return 0;
}

void gpio_cpu_irq_enable(gpio_port_t port, gpio_pin_t pin)
{
    (void)port;
    EXTI->IMR |= (1 << pin);
}

void gpio_cpu_irq_disable(gpio_port_t port, gpio_pin_t pin)
{
    (void)port;
    EXTI->IMR &= ~(1 << pin);
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
#endif /* MODULE_PERIPH_GPIO_IRQ */

#else
typedef int dont_be_pedantic;
#endif
