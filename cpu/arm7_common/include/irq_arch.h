/*
 * Copyright (C) 2005, 2006, 2007, 2008 by Thomas Hillebrandt and Heiko Will
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_arm7_common
 * @{
 *
 * @file
 * @brief           Implementation of the kernels irq interface
 *
 * @author          Heiko Will <hwill@inf.fu-berlin.de>
 */

#ifndef IRQ_ARCH_H
#define IRQ_ARCH_H

#include "VIC.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IRQ_MASK 0x00000080

static inline unsigned __get_cpsr(void)
{
    unsigned long retval;
    __asm__ volatile(" mrs  %0, cpsr" : "=r"(retval) : /* no inputs */ : "memory");
    return retval;
}

static inline void __set_cpsr(unsigned val)
{
    __asm__ volatile(" msr  cpsr, %0" : /* no outputs */ : "r"(val) : "memory");
}

static inline int irq_is_in(void)
{
    int retval;
    __asm__ volatile(" mrs  %0, cpsr" : "=r"(retval) : /* no inputs */ : "memory");
    return (retval & INTMode) == 18;
}

static inline __attribute__((always_inline)) unsigned irq_disable(void)
{
    unsigned _cpsr;

    _cpsr = __get_cpsr();
    __set_cpsr(_cpsr | IRQ_MASK);
    return _cpsr;
}

static inline __attribute__((always_inline)) void irq_restore(unsigned oldCPSR)
{
    __set_cpsr(oldCPSR);
}

static inline __attribute__((always_inline)) unsigned irq_enable(void)
{
    unsigned _cpsr;

    _cpsr = __get_cpsr();
    __set_cpsr(_cpsr & ~IRQ_MASK);
    return _cpsr;
}

static inline __attribute__((always_inline)) int irq_is_enabled(void)
{
    return !(__get_cpsr() & IRQ_MASK);
}

#ifdef __cplusplus
}
#endif

#endif /* IRQ_ARCH_H */
/** @} */
