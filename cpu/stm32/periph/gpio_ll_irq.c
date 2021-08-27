/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *               2015 Hamburg University of Applied Sciences
 *               2017-2020 Inria
 *               2017 OTA keys S.A.
 *               2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_gpio_ll_irq
 * @{
 *
 * @file
 * @brief       IRQ implementation of the GPIO Low-Level API for STM32 (except F1)
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Katja Kirstein <katja.kirstein@haw-hamburg.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>

#include "cpu.h"
#include "bitarithm.h"
#include "periph/gpio_ll_irq.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define EXTI_NUMOF          (16U)
#define EXTI_MASK           (0xFFFF)

#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32U5) || \
    defined(CPU_FAM_STM32WL)
#  define EXTI_REG_RTSR     (EXTI->RTSR1)
#  define EXTI_REG_FTSR     (EXTI->FTSR1)
#  define EXTI_REG_SWIER    (EXTI->SWIER1)
#  define EXTI_REG_IMR      (EXTI->IMR1)
#  if !defined(CPU_FAM_STM32G0) && !defined(CPU_FAM_STM32L5) && \
      !defined(CPU_FAM_STM32U5) && !defined(CPU_FAM_STM32MP1)
#    define EXTI_REG_PR       (EXTI->PR1)
#  endif
#elif defined(CPU_FAM_STM32MP1)
#  define EXTI_REG_RTSR     (EXTI->RTSR1)
#  define EXTI_REG_FTSR     (EXTI->FTSR1)
#  define EXTI_REG_PR       (EXTI->PR1)
#  define EXTI_REG_SWIER    (EXTI->SWIER1)
#  define EXTI_REG_IMR      (EXTI_C2->IMR1)
#else
#  define EXTI_REG_RTSR     (EXTI->RTSR)
#  define EXTI_REG_FTSR     (EXTI->FTSR)
#  define EXTI_REG_PR       (EXTI->PR)
#  define EXTI_REG_SWIER    (EXTI->SWIER)
#  define EXTI_REG_IMR      (EXTI->IMR)
#endif

void gpio_ll_irq_mask(gpio_port_t port, uint8_t pin)
{
    (void)port;
    EXTI_REG_IMR &= ~(1 << pin);
}

void gpio_ll_irq_unmask_and_clear(gpio_port_t port, uint8_t pin)
{
    (void)port;
    EXTI_REG_IMR |= (1 << pin);
}

struct isr_ctx {
    gpio_ll_cb_t cb;
    void *arg;
};

static struct isr_ctx isr_ctx[EXTI_NUMOF];
static uint16_t level_triggered;

static IRQn_Type get_irqn(uint8_t pin)
{
#if defined(CPU_FAM_STM32L5) ||  defined(CPU_FAM_STM32U5)
    return EXTI0_IRQn + pin;
#elif defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32G0)
    if (pin < 2) {
        return EXTI0_1_IRQn;
    }
    else if (pin < 4) {
        return EXTI2_3_IRQn;
    }
    else {
        return EXTI4_15_IRQn;
    }
#elif defined(CPU_FAM_STM32MP1)
    if (pin < 5) {
        return EXTI0_IRQn + pin;
    }
    else if (pin < 6) {
        return EXTI5_IRQn;
    }
    else if (pin < 10) {
        return EXTI6_IRQn + pin - 6;
    }
    else if (pin < 11) {
        return EXTI10_IRQn;
    }
    else if (pin < 12) {
        return EXTI11_IRQn;
    }
    else if (pin < 14) {
        return EXTI12_IRQn + pin - 12;
    }
    else if (pin < 15) {
        return EXTI14_IRQn;
    }
    else {
        return EXTI15_IRQn;
    }
#else
    if (pin < 5) {
        return EXTI0_IRQn + pin;
    }
    else if (pin < 10) {
        return EXTI9_5_IRQn;
    }
    else {
        return EXTI15_10_IRQn;
    }
#endif
}

static void clear_pending_irqs(uint8_t pin)
{
#if defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32L5) || \
    defined(CPU_FAM_STM32U5) || defined(CPU_FAM_STM32MP1)
    /* clear any pending requests */
    EXTI->RPR1 = (1 << pin);
    EXTI->FPR1 = (1 << pin);
#else
    /* clear any pending requests */
    EXTI_REG_PR = (1 << pin);
#endif
}

static void set_exti_port(uint8_t exti_num, uint8_t port_num)
{
#if defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32L5) || \
    defined(CPU_FAM_STM32U5)
    /* enable specific pin as exti sources */
    EXTI->EXTICR[exti_num >> 2] &= ~(0xf << ((exti_num & 0x03) * 8));
    EXTI->EXTICR[exti_num >> 2] |= (port_num << ((exti_num & 0x03) * 8));
#elif defined(CPU_FAM_STM32MP1)
    /* enable specific pin as exti sources */
    EXTI->EXTICR[exti_num >> 2] &= ~(0xf << ((exti_num & 0x03) * 4));
    EXTI->EXTICR[exti_num >> 2] |= (port_num << ((exti_num & 0x03) * 4));
#else
    /* enable specific pin as exti sources */
    SYSCFG->EXTICR[exti_num >> 2] &= ~(0xf << ((exti_num & 0x03) * 4));
    SYSCFG->EXTICR[exti_num >> 2] |= (port_num << ((exti_num & 0x03) * 4));
#endif
}

static uint8_t get_exti_port(uint8_t exti_num)
{
#if defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32L5) || \
    defined(CPU_FAM_STM32U5)
    /* enable specific pin as exti sources */
    return 0xf & (EXTI->EXTICR[exti_num >> 2] >> ((exti_num & 0x03) * 8));
#elif defined(CPU_FAM_STM32MP1)
    /* enable specific pin as exti sources */
    return 0xf & (EXTI->EXTICR[exti_num >> 2] >> ((exti_num & 0x03) * 4));
#else
    /* enable specific pin as exti sources */
    return 0xf & (SYSCFG->EXTICR[exti_num >> 2] >> ((exti_num & 0x03) * 4));
#endif
}

int gpio_ll_irq(gpio_port_t port, uint8_t pin, gpio_irq_trig_t trig, gpio_ll_cb_t cb, void *arg)
{
    unsigned irq_state = irq_disable();
    int port_num = GPIO_PORT_NUM(port);

    /* set callback */
    isr_ctx[pin].cb = cb;
    isr_ctx[pin].arg = arg;

    /* enable clock of the SYSCFG module for EXTI configuration */
#if !defined(CPU_FAM_STM32WB) && !defined(CPU_FAM_STM32MP1) && \
    !defined(CPU_FAM_STM32WL)
#ifdef CPU_FAM_STM32F0
    periph_clk_en(APB2, RCC_APB2ENR_SYSCFGCOMPEN);
#elif defined(CPU_FAM_STM32G0)
    periph_clk_en(APB12, RCC_APBENR2_SYSCFGEN);
#elif defined(CPU_FAM_STM32U5)
    periph_clk_en(APB3, RCC_APB3ENR_SYSCFGEN);
#else
    periph_clk_en(APB2, RCC_APB2ENR_SYSCFGEN);
#endif
#endif

    /* enable global pin interrupt */
    NVIC_EnableIRQ(get_irqn(pin));

    /* configure trigger */
    if (trig & GPIO_TRIGGER_EDGE_RISING) {
        EXTI_REG_RTSR |= 1UL << pin;
    }
    else {
        EXTI_REG_RTSR &= ~(1UL << pin);
    }
    if (trig & GPIO_TRIGGER_EDGE_FALLING) {
        EXTI_REG_FTSR |=  1UL << pin;
    }
    else {
        EXTI_REG_FTSR &= ~(1UL << pin);
    }

    set_exti_port(pin, port_num);

    clear_pending_irqs(pin);
    gpio_ll_irq_unmask_and_clear(port, pin);

    if (trig & GPIO_TRIGGER_LEVEL) {
        level_triggered |= 1UL << pin;
        /* if input is already at trigger level there might be no flank, so issue soft IRQ */
        uint32_t actual_level = gpio_ll_read(port) & (1UL << pin);
        uint32_t trigger_level = EXTI_REG_RTSR & (1UL << pin);
        if (actual_level == trigger_level) {
            EXTI_REG_SWIER = 1UL << pin;
        }
    }
    else {
        level_triggered &= ~(1UL << pin);
    }

    irq_restore(irq_state);

    return 0;
}

void isr_exti(void)
{
#if defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32L5) || \
    defined(CPU_FAM_STM32U5) || defined(CPU_FAM_STM32MP1)
    /* get all interrupts handled by this ISR */
    uint32_t pending_rising_isr = (EXTI->RPR1 & EXTI_MASK);
    uint32_t pending_falling_isr = (EXTI->FPR1 & EXTI_MASK);

    /* clear by writing a 1 */
    EXTI->RPR1 = pending_rising_isr;
    EXTI->FPR1 = pending_falling_isr;

    /* only generate interrupts against lines which have their IMR set */
    uint32_t pending_isr = (pending_rising_isr | pending_falling_isr) & EXTI_REG_IMR;
#else
    /* read all pending interrupts wired to isr_exti */
    uint32_t pending_isr = (EXTI_REG_PR & EXTI_MASK);

    /* clear by writing a 1 */
    EXTI_REG_PR = pending_isr;

    /* only generate soft interrupts against lines which have their IMR set */
    pending_isr &= EXTI_REG_IMR;
#endif

    /* iterate over all set bits */
    uint8_t pin = 0;
    while (pending_isr) {
        pending_isr = bitarithm_test_and_clear(pending_isr, &pin);
        isr_ctx[pin].cb(isr_ctx[pin].arg);
        /* emulate level triggered IRQs by asserting the IRQ again in software, if needed */
        if (level_triggered & (1UL << pin)) {
            /* Trading a couple of CPU cycles to not having to store port connected to EXTI in RAM.
             * A simple look up table would save ~6 instructions for the cost 64 byte or RAM. */
            gpio_port_t port = GPIO_PORT(get_exti_port(pin));
            uint32_t actual_level = gpio_ll_read(port) & (1UL << pin);
            uint32_t trigger_level = EXTI_REG_RTSR & (1UL << pin);
            if (actual_level == trigger_level) {
                EXTI_REG_SWIER = 1UL << pin;
            }
        }
    }
    cortexm_isr_end();
}
