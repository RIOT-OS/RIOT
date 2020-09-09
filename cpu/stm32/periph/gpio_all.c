/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *               2015 Hamburg University of Applied Sciences
 *               2017-2020 Inria
 *               2017 OTA keys S.A.
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
#include "bitarithm.h"
#include "periph/gpio.h"
#include "periph_conf.h"

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

#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0)
#define EXTI_REG_RTSR       (EXTI->RTSR1)
#define EXTI_REG_FTSR       (EXTI->FTSR1)
#define EXTI_REG_PR         (EXTI->PR1)
#define EXTI_REG_IMR        (EXTI->IMR1)
#else
#define EXTI_REG_RTSR       (EXTI->RTSR)
#define EXTI_REG_FTSR       (EXTI->FTSR)
#define EXTI_REG_PR         (EXTI->PR)
#define EXTI_REG_IMR        (EXTI->IMR)
#endif

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

static inline void port_init_clock(GPIO_TypeDef *port, gpio_t pin)
{
    (void)port; /* <-- Only used for when port G requires special handling */
#if defined(CPU_FAM_STM32F0) || defined (CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L1)
    periph_clk_en(AHB, (RCC_AHBENR_GPIOAEN << _port_num(pin)));
#elif defined (CPU_FAM_STM32L0) || defined(CPU_FAM_STM32G0)
    periph_clk_en(IOP, (RCC_IOPENR_GPIOAEN << _port_num(pin)));
#elif defined (CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
      defined (CPU_FAM_STM32G4)
    periph_clk_en(AHB2, (RCC_AHB2ENR_GPIOAEN << _port_num(pin)));
#ifdef PWR_CR2_IOSV
    if (port == GPIOG) {
        /* Port G requires external power supply */
        periph_clk_en(APB1, RCC_APB1ENR1_PWREN);
        PWR->CR2 |= PWR_CR2_IOSV;
    }
#endif /* PWR_CR2_IOSV */
#else
    periph_clk_en(AHB1, (RCC_AHB1ENR_GPIOAEN << _port_num(pin)));
#endif
}

static inline void set_mode(GPIO_TypeDef *port, int pin_num, unsigned mode)
{
    uint32_t tmp = port->MODER;
    tmp &= ~(0x3 << (2 * pin_num));
    tmp |=  ((mode & 0x3) << (2 * pin_num));
    port->MODER = tmp;
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    GPIO_TypeDef *port = _port(pin);
    int pin_num = _pin_num(pin);

    /* enable clock */
    port_init_clock(port, pin);
    /* set mode */
    set_mode(port, pin_num, mode);
    /* set pull resistor configuration */
    port->PUPDR &= ~(0x3 << (2 * pin_num));
    port->PUPDR |=  (((mode >> 2) & 0x3) << (2 * pin_num));
    /* set output mode */
    port->OTYPER &= ~(1 << pin_num);
    port->OTYPER |=  (((mode >> 4) & 0x1) << pin_num);
    /* set pin speed to maximum */
    port->OSPEEDR |= (3 << (2 * pin_num));

    return 0;
}

void gpio_init_af(gpio_t pin, gpio_af_t af)
{
    GPIO_TypeDef *port = _port(pin);
    uint32_t pin_num = _pin_num(pin);

    /* enable clock */
    port_init_clock(port, pin);
    /* set selected function */
    port->AFR[(pin_num > 7) ? 1 : 0] &= ~(0xf << ((pin_num & 0x07) * 4));
    port->AFR[(pin_num > 7) ? 1 : 0] |= (af << ((pin_num & 0x07) * 4));
    /* set pin to AF mode */
    set_mode(port, pin_num, 2);
}

void gpio_init_analog(gpio_t pin)
{
    /* enable clock, needed as this function can be used without calling
     * gpio_init first */
#if defined(CPU_FAM_STM32F0) || defined (CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L1)
    periph_clk_en(AHB, (RCC_AHBENR_GPIOAEN << _port_num(pin)));
#elif defined (CPU_FAM_STM32L0) || defined(CPU_FAM_STM32G0)
    periph_clk_en(IOP, (RCC_IOPENR_GPIOAEN << _port_num(pin)));
#elif defined (CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
      defined (CPU_FAM_STM32G4)
    periph_clk_en(AHB2, (RCC_AHB2ENR_GPIOAEN << _port_num(pin)));
#else
    periph_clk_en(AHB1, (RCC_AHB1ENR_GPIOAEN << _port_num(pin)));
#endif
    /* set to analog mode */
    _port(pin)->MODER |= (0x3 << (2 * _pin_num(pin)));
}

void gpio_irq_enable(gpio_t pin)
{
    EXTI_REG_IMR |= (1 << _pin_num(pin));
}

void gpio_irq_disable(gpio_t pin)
{
    EXTI_REG_IMR &= ~(1 << _pin_num(pin));
}

int gpio_read(gpio_t pin)
{
    return (_port(pin)->IDR & (1 << _pin_num(pin)));
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

#ifdef MODULE_PERIPH_GPIO_IRQ
int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    int pin_num = _pin_num(pin);
    int port_num = _port_num(pin);

    /* set callback */
    isr_ctx[pin_num].cb = cb;
    isr_ctx[pin_num].arg = arg;

    /* enable clock of the SYSCFG module for EXTI configuration */
#ifndef CPU_FAM_STM32WB
#ifdef CPU_FAM_STM32F0
    periph_clk_en(APB2, RCC_APB2ENR_SYSCFGCOMPEN);
#elif defined(CPU_FAM_STM32G0)
    periph_clk_en(APB12, RCC_APBENR2_SYSCFGEN);
#else
    periph_clk_en(APB2, RCC_APB2ENR_SYSCFGEN);
#endif
#endif

    /* initialize pin as input */
    gpio_init(pin, mode);

    /* enable global pin interrupt */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32G0)
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
    EXTI_REG_RTSR &= ~(1 << pin_num);
    EXTI_REG_RTSR |=  ((flank & 0x1) << pin_num);
    EXTI_REG_FTSR &= ~(1 << pin_num);
    EXTI_REG_FTSR |=  ((flank >> 1) << pin_num);

#if defined(CPU_FAM_STM32G0)
    /* enable specific pin as exti sources */
    EXTI->EXTICR[pin_num >> 2] &= ~(0xf << ((pin_num & 0x03) * 8));
    EXTI->EXTICR[pin_num >> 2] |= (port_num << ((pin_num & 0x03) * 8));
    /* clear any pending requests */
    EXTI->RPR1 = (1 << pin_num);
    EXTI->FPR1 = (1 << pin_num);
#else
    /* enable specific pin as exti sources */
    SYSCFG->EXTICR[pin_num >> 2] &= ~(0xf << ((pin_num & 0x03) * 4));
    SYSCFG->EXTICR[pin_num >> 2] |= (port_num << ((pin_num & 0x03) * 4));
    /* clear any pending requests */
    EXTI_REG_PR = (1 << pin_num);
#endif
    /* unmask the pins interrupt channel */
    EXTI_REG_IMR |= (1 << pin_num);

    return 0;
}

void isr_exti(void)
{
#if defined(CPU_FAM_STM32G0)
    /* only generate interrupts against lines which have their IMR set */
    uint32_t pending_rising_isr = (EXTI->RPR1 & EXTI_REG_IMR);
    uint32_t pending_falling_isr = (EXTI->FPR1 & EXTI_REG_IMR);

    /* clear by writing a 1 */
    EXTI->RPR1 = pending_rising_isr;
    EXTI->FPR1 = pending_falling_isr;

    uint32_t pending_isr = pending_rising_isr | pending_falling_isr;
#else
    /* only generate interrupts against lines which have their IMR set */
    uint32_t pending_isr = (EXTI_REG_PR & EXTI_REG_IMR);

    /* clear by writing a 1 */
    EXTI_REG_PR = pending_isr;
#endif

    /* iterate over all set bits */
    uint8_t pin = 0;
    while (pending_isr) {
        pending_isr = bitarithm_test_and_clear(pending_isr, &pin);
        isr_ctx[pin].cb(isr_ctx[pin].arg);
    }
    cortexm_isr_end();
}
#endif /* MODULE_PERIPH_GPIO_IRQ */
