/* Copyright (C) 2005, 2006, 2007, 2008 by Thomas Hillebrandt and Heiko Will

This file is part of the Micro-mesh SensorWeb Firmware.

Micro-Mesh is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

Micro-Mesh is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Micro-Mesh; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

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


