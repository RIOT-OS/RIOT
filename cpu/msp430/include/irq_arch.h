/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_msp430
* @{
 *
 * @file
 * @brief       ISR related functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 */


#include <stdbool.h>
#include <msp430.h>
#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * gcc warns for missing NOPs before/after interrupt enable/disable.
 * so I added the NOP instructions, even though they might not be necessary
 * due to following AND. // Kaspar
 */

extern volatile int __irq_is_in;
#define _GENERAL_INTERRUPT_ENABLE   (0x0008)

__attribute__((always_inline)) static inline unsigned int irq_disable(void)
{
    unsigned int state;
    __asm__ volatile(
        "mov.w SR, %[state]"                "\n\t"
        "bic %[gie], SR"                    "\n\t"
        "nop"                               "\n\t"
        "and %[gie], %[state]"              "\n\t"
        : [state]   "=r"(state)
        : [gie]     "i"(_GENERAL_INTERRUPT_ENABLE)
        : "memory"
    );

    return state;
}

__attribute__((always_inline)) static inline unsigned int irq_enable(void)
{
    unsigned int state;
    __asm__ volatile(
        "mov.w SR, %[state]"                "\n\t"
        "nop"                               "\n\t"
        "bis %[gie], SR"                    "\n\t"
        "nop"                               "\n\t"
        "and %[gie], %[state]"              "\n\t"
        : [state]   "=r"(state)
        : [gie]     "i"(_GENERAL_INTERRUPT_ENABLE)
        : "memory"
    );

    return state;
}

__attribute__((always_inline)) static inline void irq_restore(unsigned int state)
{
    __asm__ volatile(
        "bis %[state], SR"                    "\n\t"
        "nop"                                 "\n\t"
        : /* no outputs */
        : [state]   "r"(state)
        : "memory"
    );
}

__attribute__((always_inline)) static inline bool irq_is_in(void)
{
    return __irq_is_in;
}

__attribute__((always_inline)) static inline bool irq_is_enabled(void)
{
    unsigned int state;
    __asm__ volatile(
        "mov.w SR,%[state]"                   "\n\t"
        : [state]   "=r"(state)
        : /* no inputs */
        : "memory"
    );

    return (state & GIE);
}

#ifdef __cplusplus
}
#endif

/** @} */
