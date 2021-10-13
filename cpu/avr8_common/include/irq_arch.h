/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2018 RWTH Aachen, Josua Arndt <jarndt@ias.rwth-aachen.de>
 *               2020 Otto-von-Guericke-Universität Magdeburg
 *               2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_avr8_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 */

#ifndef IRQ_ARCH_H
#define IRQ_ARCH_H

#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Disable all maskable interrupts
 */
__attribute__((always_inline)) static inline unsigned int irq_disable(void)
{
    uint8_t mask;
    __asm__ volatile(
        "in %[dest], __SREG__"      "\n\t"
        "cli"                       "\n\t"
        : [dest]    "=r"(mask)
        : /* no inputs */
        : "memory"
    );
    return mask;
}

/**
 * @brief Enable all maskable interrupts
 */
__attribute__((always_inline)) static inline unsigned int irq_enable(void)
{
    uint8_t mask;
    __asm__ volatile(
        "in %[dest], __SREG__"      "\n\t"
        "sei"                       "\n\t"
        : [dest]    "=r"(mask)
        : /* no inputs */
        : "memory"
    );
    return mask;
}

/**
 * @brief Restore the state of the IRQ flags
 */
__attribute__((always_inline)) static inline void irq_restore(unsigned int _state)
{
    uint8_t state = (uint8_t)_state;
    /*
     * Implementation in pseudo-code:
     *
     * disable_irqs();
     * if (state & BIT7) {
     *    enable_irqs();
     * }
     *
     * This takes 3 CPU Cycles if BIT7 is set (IRQs are enabled), otherwise 2.
     */
    __asm__ volatile(
        "cli"                       "\n\t"
        "sbrc %[state], 7"          "\n\t"
        "sei"                       "\n\t"
        : /* no outputs */
        : [state]           "r"(state)
        : "memory"
    );
}

/**
 * @brief See if the current context is inside an ISR
 */
__attribute__((always_inline)) static inline int irq_is_in(void)
{
    uint8_t state = avr8_get_state();
    return (state & AVR8_STATE_FLAG_ISR);
}

/**
 * @brief Test if interrupts are currently enabled
 */
__attribute__((always_inline)) static inline int irq_is_enabled(void)
{
    uint8_t mask;
    __asm__ volatile(
        "in %[dest], __SREG__"      "\n\t"
        : [dest]    "=r"(mask)
        : /* no inputs */
        : "memory"
    );
    return mask & (1 << 7);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* IRQ_ARCH_H */
