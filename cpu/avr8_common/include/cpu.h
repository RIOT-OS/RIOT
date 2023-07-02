/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2018 RWTH Aachen, Josua Arndt <jarndt@ias.rwth-aachen.de>
 *               2021-2023 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_avr8_common
 * @brief       Common implementations and headers for AVR-8 family based micro-controllers
 * @{
 *
 * @file
 * @brief       Basic definitions for the AVR-8 common module
 *
 * When ever you want to do something hardware related, that is accessing MCUs registers directly,
 * just include this file. It will then make sure that the MCU specific headers are included.
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 */

#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdint.h>

#include <avr/interrupt.h>
#include "architecture.h"
#include "cpu_conf.h"
#include "cpu_clock.h"
#include "sched.h"
#include "thread.h"
#include "states_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    BOD monitoring when CPU is on sleep
 * @{
 */
#ifndef AVR8_PM_DISABLE_BOD_ON_SLEEP
#define AVR8_PM_DISABLE_BOD_ON_SLEEP 0 /**< BOD is active on sleep */
#endif
/** @} */

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
 * @brief        Compute UART TX channel
 *
 * @param uart   The UART number
 */
#define AVR8_STATE_FLAG_UART_TX(uart)    (0x01U << uart)

/**
 * @brief        Set UART TX channel as pending
 *
 * @param uart   The UART number
 */
static inline void avr8_uart_tx_set_pending(unsigned uart)
{
    avr8_state_uart |= AVR8_STATE_FLAG_UART_TX(uart);
}

/**
 * @brief        Clear UART TX channel pending state
 *
 * @param uart   The UART number
 */
static inline void avr8_uart_tx_clear_pending(unsigned uart)
{
    avr8_state_uart &= ~AVR8_STATE_FLAG_UART_TX(uart);
}

/**
 * @brief   Check if TX on any present UART device is still pending
 *
 * @retval  !=0     At least on UART device is still sending data out
 * @retval  0       No UART is currently sending data
 */
static inline int avr8_is_uart_tx_pending(void)
{
    return avr8_state_uart;
}

/**
 * @brief Enter ISR routine
 *
 * It saves the register context for process an ISR.
 */
__attribute__((always_inline))
static inline void avr8_isr_prolog(void)
{
    /* This flag is only called from IRQ context.  The value will be handled
     * before ISR context is left by @ref avr8_isr_epilog.  ATxmega requires a
     * cli() as first instruction inside an ISR to create a critical section
     * around `avr8_state_irq_count`.
     */
    __asm__ volatile (
#if defined(CPU_ATXMEGA)
        "cli                               \n\t"
#endif
    /* Register pair r24/25 are used to update `avr8_state_irq_count` content.
     * This registers are used to test conditions related to context switch in
     * ISR at @ref avr8_isr_epilog.
     */
        "push   r24                        \n\t"
        "push   r25                        \n\t"
#if (AVR8_STATE_IRQ_USE_SRAM)
        "lds    r24, %[state]              \n\t"
        "subi   r24, 0xFF                  \n\t"
        "sts    %[state], r24              \n\t"
#else
        "in     r24, %[state]              \n\t"
        "subi   r24, 0xFF                  \n\t"
        "out    %[state], r24              \n\t"
#endif
#if defined(CPU_ATXMEGA)
        "sei                               \n\t"
#endif
        "push   __zero_reg__               \n\t"
        "push   __tmp_reg__                \n\t"
        "in     __tmp_reg__, __SREG__      \n\t"
        "push   __tmp_reg__                \n\t"
        "eor    __zero_reg__, __zero_reg__ \n\t"
#if __AVR_HAVE_RAMPD__
        "in     __tmp_reg__, __RAMPD__     \n\t"
        "push   __tmp_reg__                \n\t"
        "out    __RAMPD__, __zero_reg__    \n\t"
#endif
#if __AVR_HAVE_RAMPX__
        "in     __tmp_reg__, __RAMPX__     \n\t"
        "push   __tmp_reg__                \n\t"
        "out    __RAMPX__, __zero_reg__    \n\t"
#endif
#if __AVR_HAVE_RAMPZ__
        "in     __tmp_reg__, __RAMPZ__     \n\t"
        "push   __tmp_reg__                \n\t"
        "out    __RAMPZ__, __zero_reg__    \n\t"
#endif
        "push   r18                        \n\t"
        "push   r19                        \n\t"
        "push   r20                        \n\t"
        "push   r21                        \n\t"
        "push   r22                        \n\t"
        "push   r23                        \n\t"
        "push   r26                        \n\t"
        "push   r27                        \n\t"
        "push   r30                        \n\t"
        "push   r31                        \n\t"
        : /* no output */
#if (AVR8_STATE_IRQ_USE_SRAM)
        : [state] "" (avr8_state_irq_count)
#else
        : [state] "I" (_SFR_IO_ADDR(avr8_state_irq_count))
#endif
        : "memory"
    );
}

/**
 * @brief   Restore register context from ISR
 */
__attribute__((naked))
void avr8_isr_epilog(void);

/**
 * @brief   Run this code on entering interrupt routines
 *
 * Notes:
 * - This code must not be shared.
 * - This code must not be a call or jmp.
 */
__attribute__((always_inline))
static inline void avr8_enter_isr(void)
{
    avr8_isr_prolog();
}

/**
 * @brief Run this code on exiting interrupt routines
 */
__attribute__((always_inline))
static inline void avr8_exit_isr(void)
{
    /* Let's not add more address in stack and save some code sharing
     * avr8_isr_epilog.
     */
#if (FLASHEND <= 0x1FFF)
    __asm__ volatile ("rjmp avr8_isr_epilog" : : : "memory");
#else
    __asm__ volatile ("jmp avr8_isr_epilog" : : : "memory");
#endif
}

/**
 * @brief Initialization of the CPU clock
 */
void avr8_clk_init(void);

/**
 * @brief   Get the last instruction's address
 *
 * @details This works only if called in a function as first statement, as
 *          it relies on the return address to be the topmost item on the stack.
 */
static inline uinttxtptr_t __attribute__((always_inline)) cpu_get_caller_pc(void)
{
        uinttxtptr_t addr;
    __asm__ volatile(
        "ldi %D[dest], 0                   \n\t"
#if __AVR_3_BYTE_PC__
        "pop %C[dest]                      \n\t"
#else
        "ldi %C[dest], 0                   \n\t"
#endif
        "pop %B[dest]                      \n\t"
        "pop %A[dest]                      \n\t"
        "push %A[dest]                     \n\t"
        "push %B[dest]                     \n\t"
#if __AVR_3_BYTE_PC__
        "push %C[dest]                     \n\t"
#endif
        : [dest]    "=r"(addr)
        : /* no inputs */
        : "memory"
    );

    /* addr now contains instruction to return to, subtract one to get
     * the instruction that called this function. Also multiply by two to get
     * the byte position, rather than the (16 bit) instruction position */
    addr = (addr - 1 ) * 2;
    return addr;
}

/**
 * @brief   Initializes avrlibc stdio
 */
void avr8_stdio_init(void);

/**
 * @brief   Print reset cause
 */
void avr8_reset_cause(void);

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
/** @} */
