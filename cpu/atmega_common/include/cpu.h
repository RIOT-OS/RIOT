/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2018 RWTH Aachen, Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @brief       Common implementations and headers for ATmega family based micro-controllers
 * @{
 *
 * @file
 * @brief       Basic definitions for the ATmega common module
 *
 * When ever you want to do something hardware related, that is accessing MCUs registers directly,
 * just include this file. It will then make sure that the MCU specific headers are included.
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 */

#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdint.h>

#include <avr/interrupt.h>
#include "cpu_conf.h"

/**
 * For downwards compatibility with old RIOT code.
 * TODO: remove once core was adjusted
 */
#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief global in-ISR state variable
 */
extern volatile uint8_t __in_isr;

/**
 * @brief Flag entering of an ISR
 */
static inline void __enter_isr(void)
{
    __in_isr = 1;
}

/**
 * @brief Flag exiting of an ISR
 */
static inline void __exit_isr(void)
{
    __in_isr = 0;
}

/**
 * @brief Initialization of the CPU
 */
void cpu_init(void);

/**
 * @brief   Print the last instruction's address
 */
__attribute__((always_inline)) static inline void cpu_print_last_instruction(void)
{
    uint8_t hi;
    uint8_t lo;
    uint16_t ptr;

    __asm__ volatile( "in __tmp_reg__, __SP_H__  \n\t"
                      "mov %0, __tmp_reg__       \n\t"
                      : "=g"(hi) );

    __asm__ volatile( "in __tmp_reg__, __SP_L__  \n\t"
                      "mov %0, __tmp_reg__       \n\t"
                      : "=g"(lo) );
    ptr = hi<<8 | lo;
    printf("Stack Pointer: 0x%04x\n", ptr);
}

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
/** @} */
