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
#include "sched.h"
#include "thread.h"
/**
 * For downwards compatibility with old RIOT code.
 * TODO: remove once core was adjusted
 */
#include "irq.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Use shared I2C functions
 * @{
 */
#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_WRITE_REG
#define PERIPH_I2C_NEED_READ_REGS
#define PERIPH_I2C_NEED_WRITE_REGS
/** @} */

/**
 * @brief global in-ISR state variable
 */
extern volatile uint8_t __in_isr;

/**
 * @brief Run this code on entering interrupt routines
 */
static inline void __enter_isr(void)
{
    __in_isr = 1;
}

/**
 * @brief   Exit ISR mode and yield with a return from interrupt. Use at the
 * end of ISRs in place of thread_yield_higher. If thread_yield is needed, use
 * thread_yield followed by thread_yield_isr instead of thread_yield alone.
 */
void thread_yield_isr(void);

/**
 * @brief Run this code on exiting interrupt routines
 */
static inline void __exit_isr(void)
{
    if (sched_context_switch_request) {
        thread_yield();
        __in_isr = 0;
        thread_yield_isr();
    }
    __in_isr = 0;
}

/**
 * @brief Initialization of the CPU
 */
void cpu_init(void);

/**
 * @brief   Print the last instruction's address
 */
static inline void __attribute__((always_inline)) cpu_print_last_instruction(void)
{
    uint8_t hi;
    uint8_t lo;
    uint16_t ptr;

    __asm__ volatile ("in __tmp_reg__, __SP_H__  \n\t"
                      "mov %0, __tmp_reg__       \n\t"
                      : "=g" (hi));

    __asm__ volatile ("in __tmp_reg__, __SP_L__  \n\t"
                      "mov %0, __tmp_reg__       \n\t"
                      : "=g" (lo));
    ptr = hi << 8 | lo;
    printf("Stack Pointer: 0x%04x\n", ptr);
}

/**
 * @brief   ATmega system clock prescaler settings
 *
 * Some CPUs may not support the highest prescaler settings
 */
enum {
    CPU_ATMEGA_CLK_SCALE_DIV1   = 0,
    CPU_ATMEGA_CLK_SCALE_DIV2   = 1,
    CPU_ATMEGA_CLK_SCALE_DIV4   = 2,
    CPU_ATMEGA_CLK_SCALE_DIV8   = 3,
    CPU_ATMEGA_CLK_SCALE_DIV16  = 4,
    CPU_ATMEGA_CLK_SCALE_DIV32  = 5,
    CPU_ATMEGA_CLK_SCALE_DIV64  = 6,
    CPU_ATMEGA_CLK_SCALE_DIV128 = 7,
    CPU_ATMEGA_CLK_SCALE_DIV256 = 8,
    CPU_ATMEGA_CLK_SCALE_DIV512 = 9,
};

/**
 * @brief   Initializes system clock prescaler
 */
static inline void atmega_set_prescaler(uint8_t clk_scale)
{
    /* Enable clock change */
    /* Must be assignment to set all other bits to zero, see datasheet */
    CLKPR = (1 << CLKPCE);

    /* Write clock within 4 cycles */
    CLKPR = clk_scale;
}

/**
 * @brief   Initializes avrlibc stdio
 */
void atmega_stdio_init(void);

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
/** @} */
