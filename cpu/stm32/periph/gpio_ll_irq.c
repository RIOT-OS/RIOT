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
 * @brief       IRQ implementation of the GPIO Low-Level API for STM32
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

#if defined(EXTI_SWIER_SWI0) || defined(EXTI_SWIER_SWIER0)
#  define EXTI_REG_SWIER        (EXTI->SWIER)
#elif defined(EXTI_SWIER1_SWI0) || defined(EXTI_SWIER1_SWIER0)
#  define EXTI_REG_SWIER        (EXTI->SWIER1)
#endif

#if defined(EXTI_RTSR_RT0) || defined(EXTI_RTSR_TR0)
#  define EXTI_REG_RTSR         (EXTI->RTSR)
#elif defined(EXTI_RTSR1_RT0) || defined(EXTI_RTSR1_TR0)
#  define EXTI_REG_RTSR         (EXTI->RTSR1)
#endif

#if defined(EXTI_FTSR_FT0) || defined(EXTI_FTSR_TR0)
#  define EXTI_REG_FTSR         (EXTI->FTSR)
#elif defined(EXTI_FTSR1_FT0) || defined (EXTI_FTSR1_TR0)
#  define EXTI_REG_FTSR         (EXTI->FTSR1)
#endif

#if defined(EXTI_PR_PR0)
#  define EXTI_REG_PR           (EXTI->PR)
#elif defined(EXTI_PR1_PIF0)
#  define EXTI_REG_PR           (EXTI->PR1)
#else
#  define EXTI_REG_FPR          (EXTI->FPR1)
#  define EXTI_REG_RPR          (EXTI->RPR1)
#endif

#if defined(EXTI_C2_BASE)
#  define EXTI_REG_IMR          (EXTI_C2->IMR1)
#elif defined(EXTI_IMR_IM0)
#  define EXTI_REG_IMR          (EXTI->IMR)
#elif defined(EXTI_IMR1_IM0)
#  define EXTI_REG_IMR          (EXTI->IMR1)
#endif

#if defined(RCC_APB2ENR_SYSCFGCOMPEN)
#  define SYSFG_CLOCK           APB2
#  define SYSFG_ENABLE_MASK     RCC_APB2ENR_SYSCFGCOMPEN
#elif defined(RCC_APB2ENR_SYSCFGEN)
#  define SYSFG_CLOCK           APB2
#  define SYSFG_ENABLE_MASK     RCC_APB2ENR_SYSCFGEN
#elif defined(RCC_APB3ENR_SYSCFGEN)
#  define SYSFG_CLOCK           APB3
#  define SYSFG_ENABLE_MASK     RCC_APB3ENR_SYSCFGEN
#endif

#if defined(EXTI_EXTICR1_EXTI0)
#  define EXTICR_REG(num)       (EXTI->EXTICR[(num) >> 2])
#elif defined(SYSCFG_EXTICR1_EXTI0)
#  define EXTICR_REG(num)       (SYSCFG->EXTICR[(num) >> 2])
#elif defined(AFIO_EXTICR1_EXTI0)
#  define EXTICR_REG(num)       (AFIO->EXTICR[(num) >> 2])
#endif

#if defined(SYSCFG_EXTICR1_EXTI1_Pos)
#  define EXTICR_FIELD_SIZE     SYSCFG_EXTICR1_EXTI1_Pos
#elif defined(EXTI_EXTICR1_EXTI1_Pos)
#  define EXTICR_FIELD_SIZE     EXTI_EXTICR1_EXTI1_Pos
#elif defined(AFIO_EXTICR1_EXTI1_Pos)
#  define EXTICR_FIELD_SIZE     AFIO_EXTICR1_EXTI1_Pos
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
    /* TODO: Come up with a way that this doesn't need updates whenever a new
     * MCU family gets added */
#if defined(CPU_FAM_STM32L5) ||  defined(CPU_FAM_STM32U5)
    return EXTI0_IRQn + pin;
#elif defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32C0)
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
#ifdef EXTI_REG_PR
    /* same IRQ flag no matter if falling or rising edge detected */
    EXTI_REG_PR = (1U << pin);
#else
    /* distinct IRQ flags for falling and rising edge, clearing both */
    EXTI_REG_FPR = (1U << pin);
    EXTI_REG_RPR = (1U << pin);
#endif
}

static void set_exti_port(uint8_t exti_num, uint8_t port_num)
{
    uint32_t tmp = EXTICR_REG(exti_num);
    tmp &= ~(0xf << ((exti_num & 0x03) * EXTICR_FIELD_SIZE));
    tmp |= (port_num << ((exti_num & 0x03) * EXTICR_FIELD_SIZE));
    EXTICR_REG(exti_num) = tmp;
}

static uint8_t get_exti_port(uint8_t exti_num)
{
    uint32_t reg = EXTICR_REG(exti_num);
    reg >>= (exti_num & 0x03) * EXTICR_FIELD_SIZE;
    return reg & 0xf;
}

int gpio_ll_irq(gpio_port_t port, uint8_t pin, gpio_irq_trig_t trig, gpio_ll_cb_t cb, void *arg)
{
    unsigned irq_state = irq_disable();
    int port_num = gpio_port_num(port);

    /* set callback */
    isr_ctx[pin].cb = cb;
    isr_ctx[pin].arg = arg;

    /* enable clock of the SYSCFG module for EXTI configuration */
#ifdef SYSFG_CLOCK
    periph_clk_en(SYSFG_CLOCK, SYSFG_ENABLE_MASK);
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

static uint32_t get_and_clear_pending_irqs(void)
{
#ifdef EXTI_REG_PR
    /* only one pending IRQ flag register for both falling and rising flanks */
    uint32_t pending_isr = (EXTI_REG_PR & EXTI_MASK);

    /* clear by writing a 1 */
    EXTI_REG_PR = pending_isr;
    return pending_isr;
#else
    /* distinct registers for pending IRQ flags depending on rising or falling
     * flank */
    uint32_t pending_rising_isr = (EXTI_REG_RPR & EXTI_MASK);
    uint32_t pending_falling_isr = (EXTI_REG_FPR & EXTI_MASK);

    /* clear by writing a 1 */
    EXTI->RPR1 = pending_rising_isr;
    EXTI->FPR1 = pending_falling_isr;

    return pending_rising_isr | pending_falling_isr;
#endif
}

void isr_exti(void)
{
    uint32_t pending_isr = get_and_clear_pending_irqs();

    /* only generate soft interrupts against lines which have their IMR set */
    pending_isr &= EXTI_REG_IMR;

    /* iterate over all set bits */
    uint8_t pin = 0;
    while (pending_isr) {
        pending_isr = bitarithm_test_and_clear(pending_isr, &pin);
        isr_ctx[pin].cb(isr_ctx[pin].arg);
        /* emulate level triggered IRQs by asserting the IRQ again in software, if needed */
        if (level_triggered & (1UL << pin)) {
            /* Trading a couple of CPU cycles to not having to store port connected to EXTI in RAM.
             * A simple look up table would save ~6 instructions for the cost 64 bytes of RAM. */
            gpio_port_t port = gpio_port(get_exti_port(pin));
            uint32_t actual_level = gpio_ll_read(port) & (1UL << pin);
            uint32_t trigger_level = EXTI_REG_RTSR & (1UL << pin);
            if (actual_level == trigger_level) {
                EXTI_REG_SWIER = 1UL << pin;
            }
        }
    }
    cortexm_isr_end();
}
