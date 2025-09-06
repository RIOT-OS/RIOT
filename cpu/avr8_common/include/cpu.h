/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 * SPDX-FileCopyrightText: 2018 RWTH Aachen, Josua Arndt <jarndt@ias.rwth-aachen.de>
 * SPDX-FileCopyrightText: 2021-2023 Gerson Fernando Budke <nandojve@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
 * @brief   CPU cycles per busy wait loop
 */
#define CPU_CYCLES_PER_LOOP (7)

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
 * @brief   Run this code on entering interrupt routines
 */
static inline void avr8_enter_isr(void)
{
    /* This flag is only called from IRQ context, and nested IRQs are not
     * supported as of now. The flag will be unset before the IRQ context is
     * left, so no need to use memory barriers or atomics here
     */
    ++avr8_state_irq_count;
}

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
 * @brief Run this code on exiting interrupt routines
 */
void avr8_exit_isr(void);

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
        "ldi %D[dest], 0"                   "\n\t"
#if __AVR_3_BYTE_PC__
        "pop %C[dest] "      "\n\t"
#else
        "ldi %C[dest], 0"                   "\n\t"
#endif
        "pop %B[dest]"                      "\n\t"
        "pop %A[dest]"                      "\n\t"
        "push %A[dest]"                     "\n\t"
        "push %B[dest]"                     "\n\t"
#if __AVR_3_BYTE_PC__
        "push %C[dest] "      "\n\t"
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

/** @} */
