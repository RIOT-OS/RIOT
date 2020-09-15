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
 * @brief   Converts a port into a pointer to its register structure
 */
static inline GPIO_TypeDef *_port(gpio_port_t port)
{
    return (GPIO_TypeDef *)port.reg;
}

/**
 * @brief   Convert a port to its port number
 *
 * The port number is extracted by looking at bits 10, 11, 12, 13 of the base
 * register addresses.
 */
static inline int _port_num(gpio_port_t port)
{
    return GPIO_CPU_PORT_NUM(port.reg);
}

static inline void port_init_clock(gpio_port_t port)
{
#if defined(CPU_FAM_STM32F0) || defined (CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L1)
    periph_clk_en(AHB, (RCC_AHBENR_GPIOAEN << _port_num(port)));
#elif defined (CPU_FAM_STM32L0) || defined(CPU_FAM_STM32G0)
    periph_clk_en(IOP, (RCC_IOPENR_GPIOAEN << _port_num(port)));
#elif defined (CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
      defined (CPU_FAM_STM32G4)
    periph_clk_en(AHB2, (RCC_AHB2ENR_GPIOAEN << _port_num(port)));
#ifdef PWR_CR2_IOSV
    if (port.reg == GPIOG_BASE) {
        /* Port G requires external power supply */
        periph_clk_en(APB1, RCC_APB1ENR1_PWREN);
        PWR->CR2 |= PWR_CR2_IOSV;
    }
#endif /* PWR_CR2_IOSV */
#else
    periph_clk_en(AHB1, (RCC_AHB1ENR_GPIOAEN << _port_num(port)));
#endif
}

static inline void set_mode(GPIO_TypeDef *port, int pin_num, unsigned mode)
{
    uint32_t tmp = port->MODER;
    tmp &= ~(0x3 << (2 * pin_num));
    tmp |=  ((mode & 0x3) << (2 * pin_num));
    port->MODER = tmp;
}

int gpio_cpu_init(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode)
{
    /* enable clock */
    port_init_clock(port);
    /* set mode */
    set_mode(_port(port), pin, mode);
    /* set pull resistor configuration */
    _port(port)->PUPDR &= ~(0x3 << (2 * pin));
    _port(port)->PUPDR |=  (((mode >> 2) & 0x3) << (2 * pin));
    /* set output mode */
    _port(port)->OTYPER &= ~(1 << pin);
    _port(port)->OTYPER |=  (((mode >> 4) & 0x1) << pin);
    /* set pin speed to maximum */
    _port(port)->OSPEEDR |= (3 << (2 * pin));

    return 0;
}

void gpio_init_af(gpio_t gpio, gpio_af_t af)
{
    GPIO_TypeDef *port = _port(gpio.port);
    gpio_pin_t pin =gpio.pin;

    /* enable clock */
    port_init_clock(gpio.port);
    /* set selected function */
    port->AFR[(pin > 7) ? 1 : 0] &= ~(0xf << ((pin & 0x07) * 4));
    port->AFR[(pin > 7) ? 1 : 0] |= (af << ((pin & 0x07) * 4));
    /* set pin to AF mode */
    set_mode(port, pin, 2);
}

void gpio_init_analog(gpio_t gpio)
{
    /* enable clock, needed as this function can be used without calling
     * gpio_init first */
#if defined(CPU_FAM_STM32F0) || defined (CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L1)
    periph_clk_en(AHB, (RCC_AHBENR_GPIOAEN << _port_num(gpio.port)));
#elif defined (CPU_FAM_STM32L0) || defined(CPU_FAM_STM32G0)
    periph_clk_en(IOP, (RCC_IOPENR_GPIOAEN << _port_num(gpio.port)));
#elif defined (CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
      defined (CPU_FAM_STM32G4)
    periph_clk_en(AHB2, (RCC_AHB2ENR_GPIOAEN << _port_num(gpio.port)));
#else
    periph_clk_en(AHB1, (RCC_AHB1ENR_GPIOAEN << _port_num(gpio.port)));
#endif
    /* set to analog mode */
    _port(gpio.port)->MODER |= (0x3 << ((uint32_t)gpio.pin << 1));
}

void gpio_cpu_irq_enable(gpio_port_t port, gpio_pin_t pin)
{
    (void)port;
    EXTI_REG_IMR |= (1 << pin);
}

void gpio_cpu_irq_disable(gpio_port_t port, gpio_pin_t pin)
{
    (void)port;
    EXTI_REG_IMR &= ~(1 << pin);
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
    gpio_cpu_init(port, pin, mode);

    /* enable global pin interrupt */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32G0)
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
    EXTI_REG_RTSR &= ~(1 << pin);
    EXTI_REG_RTSR |=  ((flank & 0x1) << pin);
    EXTI_REG_FTSR &= ~(1 << pin);
    EXTI_REG_FTSR |=  ((flank >> 1) << pin);

#if defined(CPU_FAM_STM32G0)
    /* enable specific pin as exti sources */
    EXTI->EXTICR[pin >> 2] &= ~(0xf << ((pin & 0x03) * 8));
    EXTI->EXTICR[pin >> 2] |= (port_num << ((pin & 0x03) * 8));
    /* clear any pending requests */
    EXTI->RPR1 = (1 << pin);
    EXTI->FPR1 = (1 << pin);
#else
    /* enable specific pin as exti sources */
    SYSCFG->EXTICR[pin >> 2] &= ~(0xf << ((pin & 0x03) * 4));
    SYSCFG->EXTICR[pin >> 2] |= (port_num << ((pin & 0x03) * 4));
    /* clear any pending requests */
    EXTI_REG_PR = (1 << pin);
#endif
    /* unmask the pins interrupt channel */
    EXTI_REG_IMR |= (1 << pin);

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
