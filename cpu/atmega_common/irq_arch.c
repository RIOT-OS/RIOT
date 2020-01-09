/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2018 RWTH Aachen, Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include "irq.h"
#include "cpu.h"

/**
 * @brief Macro returns state of the global interrupt register
 */
static uint8_t atmega_get_interrupt_state(void);
static void atmega_set_interrupt_state(uint8_t state);

volatile uint8_t atmega_in_isr = 0;

__attribute__((always_inline)) static inline uint8_t atmega_get_interrupt_state(void)
{
    uint8_t sreg;
    __asm__ volatile( "in __tmp_reg__, __SREG__ \n\t"
                      "mov %0, __tmp_reg__      \n\t"
                      : "=g"(sreg) );
    return sreg & (1 << 7);
}

__attribute__((always_inline)) inline void atmega_set_interrupt_state(uint8_t state)
{
    __asm__ volatile( "mov r15,%0        \n\t"
                      "in r16, __SREG__  \n\t"
                      "cbr r16,7         \n\t"
                      "or r15,r16        \n\t"
                      "out __SREG__, r15 \n\t"
                      :
                      : "g"(state)
                      : "r15", "r16", "memory");
}

/**
 * @brief Disable all maskable interrupts
 */
unsigned int irq_disable(void)
{
    uint8_t mask = atmega_get_interrupt_state();
    cli(); /* <-- acts as memory barrier, see doc of avr-libc */
    return (unsigned int) mask;
}

/**
 * @brief Enable all maskable interrupts
 */
unsigned int irq_enable(void)
{
    uint8_t mask = atmega_get_interrupt_state();
    sei(); /* <-- acts as memory barrier, see doc of avr-libc */
    return mask;
}

/**
 * @brief Restore the state of the IRQ flags
 */
void irq_restore(unsigned int state)
{
    atmega_set_interrupt_state(state);
}

/**
 * @brief See if the current context is inside an ISR
 */
int irq_is_in(void)
{
    int result = atmega_in_isr;
    __asm__ volatile("" ::: "memory");
    return result;
}
