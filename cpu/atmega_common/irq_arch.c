/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file        irq_arch.c
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include "arch/irq_arch.h"
#include "cpu.h"

/**
 * @brief Macro returns state of the global interrupt register
 */
static uint8_t __get_interrupt_state(void);
static void __set_interrupt_state(uint8_t state);

__attribute__((always_inline)) static inline uint8_t  __get_interrupt_state(void)
{
    uint8_t sreg;
    asm volatile("in r0, __SREG__; \n\t"
                 "mov %0, r0       \n\t"
                 : "=g"(sreg)
                 :
                 : "r0");
    return sreg & (1 << 7);
}

__attribute__((always_inline)) inline void __set_interrupt_state(uint8_t state)
{
    asm volatile("mov r15,%0;       \n\t"
                 "in r16, __SREG__; \n\t"
                 "cbr r16,7;        \n\t"
                 "or r15,r16;       \n\t"
                 "out __SREG__, r15 \n\t"
                 :
                 : "g"(state)
                 : "r15", "r16");
}

/**
 * @brief Disable all maskable interrupts
 */
unsigned int irq_arch_disable(void)
{
    uint8_t mask = __get_interrupt_state();
    cli();
    return (unsigned int) mask;
}

/**
 * @brief Enable all maskable interrupts
 */
unsigned int irq_arch_enable(void)
{
    sei();
    return __get_interrupt_state();
}

/**
 * @brief Restore the state of the IRQ flags
 */
void irq_arch_restore(unsigned int state)
{
    __set_interrupt_state(state);
}

/**
 * @brief See if the current context is inside an ISR
 */
int irq_arch_in(void)
{
    /*
     * TODO: find a way to implement this function (e.g. a static variable dis- or
     * set and unset in each ISR)
     */
    return 0;
}
