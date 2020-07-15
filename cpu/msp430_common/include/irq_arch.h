/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430_common
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

#ifndef IRQ_ARCH_H
#define IRQ_ARCH_H

#include "irq.h"
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

extern volatile int __irq_is_in;

__attribute__((always_inline)) static inline unsigned int irq_disable(void)
{
    unsigned int state;
    __asm__ volatile(
        "mov.w r2, %[state]"                "\n\t"
        "bic %[gie], r2"                    "\n\t"
        /*
         * BEWARE: IRQs remain enabled for one instruction after clearing the
         * GIE bit in the status register (r2). Thus, the next instruction is
         * not only used to sanitize the IRQ state, but also delays the actual
         * critical section by one CPU cycle, so that IRQs are indeed disabled
         * by then.
         */
        "and %[gie], %[state]"              "\n\t"
        : [state]   "=r"(state)
        : [gie]     "i"(GIE)
        : "memory"
    );

    return state;
}

__attribute__((always_inline)) static inline unsigned int irq_enable(void)
{
    unsigned int state;
    __asm__ volatile(
        "mov.w r2, %[state]"                "\n\t"
        "bis %[gie], r2"                    "\n\t"
        /*
         * BEWARE: IRQs remain disabled for one instruction after setting the
         * GIE bit in the status register (r2). Thus, the next instruction is
         * not only used to sanitize the IRQ state, but also ensures that the
         * first instruction after this function is run with IRQs enabled.
         */
        "and %[gie], %[state]"              "\n\t"
        : [state]   "=r"(state)
        : [gie]     "i"(GIE)
        : "memory"
    );

    return state;
}

__attribute__((always_inline)) static inline void irq_restore(unsigned int state)
{
    __asm__ volatile(
        "bis %[state], r2"                    "\n\t"
        : /* no outputs */
        : [state]   "r"(state)
        : "memory"
    );
    /* BEWARE: IRQs remain disabled for up to one CPU cycle after this function
     * call. But that doesn't seem to be harmful.
     */
}

__attribute__((always_inline)) static inline int irq_is_in(void)
{
    return __irq_is_in;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* IRQ_ARCH_H */
