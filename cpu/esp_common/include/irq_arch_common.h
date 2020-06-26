/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#ifndef IRQ_ARCH_COMMON_H
#define IRQ_ARCH_COMMON_H

#include "irq.h"
#include "sched.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Indicates the interrupt nesting depth
 *
 * The variable is incremented on entry into and decremented on exit from an ISR.
 */
extern volatile uint32_t irq_interrupt_nesting;

/**
 * @name   Macros to enter and exit an ISR
 *
 * Since all the stuff is done in `_frxt_int_enter` and `_frxt_int_exit`, these
 * macros are doing nothing and are kept only for source code compatibility.
 *
 * @{
 */
#define irq_isr_enter()
#define irq_isr_exit()
/** @} */

/**
 * @name   Macros to enter and exit a critical region
 *
 * @note: since they use a local variable they can be used only in same function
 *
 * @{
 */
#define critical_enter()   int _irq_state = irq_disable()
#define critical_exit()    irq_restore(_irq_state)
/** @} */

/**
 * @name   Macros to enter and exit a critical region with state variable
 * @{
 */
#define critical_enter_var(m)   m = irq_disable()
#define critical_exit_var(m)    irq_restore(m)
/** @} */

/**
 * @brief Disable all maskable interrupts
 */
__attribute__((always_inline)) static inline unsigned int irq_disable(void)
{
    uint32_t state;

    /* read and set interrupt level (RSIL) */
    __asm__ volatile (
        /* this will block IRQs with prio <= intlevel */
        "rsil %[state], %[intlevel]"
        : [state]       "=a" (state)
        : [intlevel]    "I"(15) /* 15 is highest prio, blocking all IRQs */
        : "memory"
    );
    /* mask out everything else of the PS register that do not belong to
       interrupt level (bits 3..0) */
    state &= 0xf;

    return state;
}

/**
 * @brief Enable all maskable interrupts
 */
__attribute__((always_inline)) static inline unsigned int irq_enable(void)
{
    uint32_t state;

    /* read and set interrupt level (RSIL) */
    __asm__ volatile (
        "rsil %[state], %[intlevel]"
        : [state]       "=a"(state)
        : [intlevel]    "I"(0) /* 0 is lowest prio, unblocking all IRQs */
        : "memory"
    );
    /* mask out everything else of the PS register that do not belong to
       interrupt level (bits 3..0) */
    state &= 0xf;

    return state;
}

/**
 * @brief Restore the state of the IRQ flags
 */
__attribute__((always_inline)) static inline void irq_restore(unsigned int state)
{
    /* the rsil instruction requires an constant immediate, thus, we cannot
     * pass the state parameters via register. Instead, we rely on the fact
     * that RIOT does not support different IRQ levels, but instead only enables
     * or disables *all* IRQs. So if state is 0 (all IRQs enabled), we will
     * do so using rsil, otherwise we don't touch the interrupt level
     */
    if (!state) {
        uint32_t old_level;
        __asm__ volatile (
            "rsil %[old_level], %[intlevel]"
            : [old_level]   "=a"(old_level)
            : [intlevel]    "I"(0)
            : "memory"
        );
    }
}

/**
 * @brief See if the current context is inside an ISR
 */
__attribute__((always_inline)) static inline int irq_is_in(void)
{
    return irq_interrupt_nesting;
}

#ifdef __cplusplus
}
#endif

#endif /* IRQ_ARCH_COMMON_H */
