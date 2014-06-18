/* Copyright (C) 2005, 2006, 2007, 2008 by Thomas Hillebrandt and Heiko Will
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

#include "VIC.h"
#include <stdbool.h>

#define IRQ_MASK 0x00000080
#define FIQ_MASK 0x00000040
#define INT_MASK (IRQ_MASK | FIQ_MASK)

static inline unsigned __get_cpsr(void)
{
    unsigned long retval;
    asm volatile(" mrs  %0, cpsr" : "=r"(retval) : /* no inputs */);
    return retval;
}

int inISR(void)
{
    int retval;
    asm volatile(" mrs  %0, cpsr" : "=r"(retval) : /* no inputs */);
    return (retval & INTMode) == 18;
}

static inline void __set_cpsr(unsigned val)
{
    asm volatile(" msr  cpsr, %0" : /* no outputs */ : "r"(val));
}

unsigned disableIRQ(void)
{
    unsigned _cpsr;

    _cpsr = __get_cpsr();
    __set_cpsr(_cpsr | IRQ_MASK);
    return _cpsr;
}

unsigned restoreIRQ(unsigned oldCPSR)
{
    unsigned _cpsr;

    _cpsr = __get_cpsr();
    __set_cpsr((_cpsr & ~IRQ_MASK) | (oldCPSR & IRQ_MASK));
    return _cpsr;
}

unsigned IRQenabled(void)
{
    unsigned _cpsr;

    _cpsr = __get_cpsr();
    return (_cpsr & IRQ_MASK);
}

unsigned enableIRQ(void)
{
    unsigned _cpsr;

    _cpsr = __get_cpsr();
    __set_cpsr(_cpsr & ~IRQ_MASK);
    return _cpsr;
}

unsigned disableFIQ(void)
{
    unsigned _cpsr;

    _cpsr = __get_cpsr();
    __set_cpsr(_cpsr | FIQ_MASK);
    return _cpsr;
}

unsigned restoreFIQ(unsigned oldCPSR)
{
    unsigned _cpsr;

    _cpsr = __get_cpsr();
    __set_cpsr((_cpsr & ~FIQ_MASK) | (oldCPSR & FIQ_MASK));
    return _cpsr;
}

unsigned enableFIQ(void)
{
    unsigned _cpsr;

    _cpsr = __get_cpsr();
    __set_cpsr(_cpsr & ~FIQ_MASK);
    return _cpsr;
}
