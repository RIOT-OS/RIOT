/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2018 RWTH Aachen, Josua Arndt <jarndt@ias.rwth-aachen.de>
 *               2020 Otto-von-Guericke-Universität Magdeburg
 *               2021-2023 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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


#include <stdbool.h>
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
__attribute__((always_inline)) static inline bool irq_is_in(void)
{
    bool is_in = avr8_state_irq_count > 0;

    __asm__ volatile ("" : : : "memory");

    return is_in;
}

/**
 * @brief Test if interrupts are currently enabled
 */
__attribute__((always_inline)) static inline bool irq_is_enabled(void)
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

/**
 * @brief Define an AVR-8 ISR
 *
 * This macro hides all RIOT-OS port internal details from user implementation.
 * The user should use this macro passing at least two parameters. The first is
 * the interrupt vector and the second one is the function to be called. Zero or
 * more optional parameters can be passed the function one by one using the
 * variable length argument list.
 *
 * It is recommended that user define a `static inline void` function to
 * the implement the interrupt. The function can accept argument list based on
 * implementation details.
 *
 * Example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * static inline void _my_isr_handler(int index)
 * {
 *     do_something(index);
 * }
 * AVR8_ISR(PERIPHERAL_INSTANCE_0_ISR, _my_isr_handler, 0);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define AVR8_ISR(vector, function, ...) \
    ISR(vector, ISR_BLOCK)              \
    {                                   \
        avr8_enter_isr();               \
        function(__VA_ARGS__);          \
        avr8_exit_isr();                \
    }

#ifdef __cplusplus
}
#endif

/** @} */
