/*
 * Copyright (C) 2008-2009, Freie Universität Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_arm7_common
 * @{
 *
 * @file
 * @brief       Default implementations for ARM7 specific interrupt and
 *              exception handlers
 *
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include "thread.h"

#include "log.h"

void isr_fio(void)   __attribute__((interrupt("FIQ")));
//void isr_swi (void)   __attribute__((interrupt("SWI")));
void isr_undef(void) __attribute__((interrupt("UNDEF")));

volatile int arm_abortflag = 0;

void isr_fio(void)
{
    LOG_ERROR("Kernel Panic,\nEarly FIQ call\n");

    while (1) {}
}

void isr_swi(void)
{
    LOG_ERROR("Kernel Panic,\nEarly SWI call\n");

    while (1) {}
}

void abtorigin(const char *vector, unsigned long *lnk_ptr1)
{
    register unsigned long *lnk_ptr2;
    register unsigned long *sp;
    register unsigned int cpsr, spsr;

    __asm__ __volatile__ ("mrs %0, cpsr" : "=r" (cpsr));            // copy current mode
    __asm__ __volatile__ ("mrs %0, spsr" : "=r" (spsr));            // copy dabt generating mode
    __asm__ __volatile__ ("msr cpsr_c, %0" : : "r" (spsr));         // switch to dabt generating mode
    __asm__ __volatile__ ("mov %0, lr" : "=r" (lnk_ptr2));          // copy lr
    __asm__ __volatile__ ("mov %0, sp" : "=r" (sp));                // copy sp
    __asm__ __volatile__ ("msr cpsr_c, %0" : : "r" (cpsr));         // switch back to abt mode

    LOG_ERROR("#!%s abort at %p (0x%08lX) originating from %p (0x%08lX) in mode 0x%X\n",
              vector, (void *)lnk_ptr1, *(lnk_ptr1), (void *)lnk_ptr2, *(lnk_ptr2), spsr
              );

    while (1) {}
}

void isr_undef(void)
{
    /* cppcheck-suppress variableScope
     * (reason: used within __asm__ which cppcheck doesn't pick up) */
    register unsigned long *lnk_ptr;

    __asm__ __volatile__ ("sub %0, lr, #8" : "=r" (lnk_ptr));     // get aborting instruction

    if (arm_abortflag == 0) {
        arm_abortflag = 1;                                          // remember state (if printing should fail again)
        abtorigin("undef", lnk_ptr);
    }

    while (1) {}
}

void isr_pabt(void)
{
    /* cppcheck-suppress variableScope
     * (reason: used within __asm__ which cppcheck doesn't pick up) */
    register unsigned long *lnk_ptr;

    __asm__ __volatile__ ("sub %0, lr, #8" : "=r" (lnk_ptr));     // get aborting instruction

    if (arm_abortflag == 0) {
        arm_abortflag = 1;                                          // remember state (if printing should fail again)
        abtorigin("pabt", lnk_ptr);
    }

    while (1) {}
}

void isr_dabt(void)
{
    /* cppcheck-suppress variableScope
     * (reason: used within __asm__ which cppcheck doesn't pick up) */
    register unsigned long *lnk_ptr;

    __asm__ __volatile__ ("sub %0, lr, #8" : "=r" (lnk_ptr));     // get aborting instruction

    if (arm_abortflag == 0) {
        arm_abortflag = 1;                                          // remember state (if printing should fail again)
        abtorigin("data", lnk_ptr);
    }

    while (1) {}
}
