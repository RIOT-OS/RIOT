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

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <stdio.h>

#include "architecture.h"
#include "cpu_clock.h"
#include "cpu_conf.h"
#include "macros/xtstr.h"
#include "sched.h"
#include "states_internal.h"
#include "thread.h"

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
 * @name    Manually provide ISR() macro for LLVM compatibility
 * @{
 */
#define ISR_BLOCK
#define ISR_NOBLOCK    __attribute__((interrupt))
#define ISR_NAKED      __attribute__((naked))
#define ISR_ALIASOF(v) __attribute__((alias(XTSTR(v))))

#if ((__GNUC__ == 4 && __GNUC_MINOR__ >= 1) || (__GNUC__ > 4)) && !defined(__clang__)
#  define __INTR_ATTRS used, externally_visible
#else /* GCC < 4.1 or LLVM */
#  define __INTR_ATTRS used
#endif

#ifdef __cplusplus
#  define ISR(vector, ...)            \
    extern "C" void vector (void) __attribute__ ((signal,__INTR_ATTRS)) __VA_ARGS__; \
    void vector (void)
#else
#  define ISR(vector, ...)            \
    void vector (void) __attribute__ ((signal,__INTR_ATTRS)) __VA_ARGS__; \
    void vector (void)
#endif

/** @} */

/**
 * @name    Manually provide IRQ helpers for LLVM compatibility
 * @{
 */
#define sei()   __asm__ __volatile__ ("sei" ::: "memory")
#define cli()   __asm__ __volatile__ ("cli" ::: "memory")
/** @} */


/**
 * @name    Manually provide used WDT stuff for compatibility with LLVM
 * @{
 */
#define WDTO_250MS  4

#if defined(WDTCSR)
#  define _WD_CONTROL_REG     WDTCSR
#elif defined(WDTCR)
#  define _WD_CONTROL_REG     WDTCR
#else
#  define _WD_CONTROL_REG     WDT
#endif

#if defined(WDTOE)
#define _WD_CHANGE_BIT      WDTOE
#else
#define _WD_CHANGE_BIT      WDCE
#endif

#if defined(WDP3)
# define _WD_PS3_MASK       _BV(WDP3)
#else
# define _WD_PS3_MASK       0x00
#endif

#define wdt_reset() __asm__ volatile ("wdr" : : :)

static inline
__attribute__((__always_inline__))
void wdt_enable (const uint8_t value)
{
    __asm__ volatile (
            "in r0,0x3f" "\n\t"
            "cli" "\n\t"
            "wdr" "\n\t"
            "sts %0, %1" "\n\t"
            "out 0x3f,r0" "\n\t"
            "sts %0, %2" "\n \t"
            : /* no outputs */
            : "n" (_SFR_MEM_ADDR(_WD_CONTROL_REG)),
            "r" ((uint8_t)(_BV(_WD_CHANGE_BIT) | _BV(WDE))),
            "r" ((uint8_t) ((value & 0x08 ? _WD_PS3_MASK : 0x00) |
                    _BV(WDE) | (value & 0x07)) )
            : "r0"
    );
}

static inline
__attribute__ ((__always_inline__))
void wdt_disable (void)
{
    register uint8_t temp_reg;
    __asm__ __volatile__ (
        "in r0,0x3f"    "\n\t"
        "cli"                        "\n\t"
        "wdr"                        "\n\t"
        "lds %[TEMPREG],%[WDTREG]"   "\n\t"
        "ori %[TEMPREG],%[WDCE_WDE]" "\n\t"
        "sts %[WDTREG],%[TEMPREG]"   "\n\t"
        "sts %[WDTREG],r1" "\n\t"
        "out 0x3f,r0"   "\n\t"
        : [TEMPREG] "=d" (temp_reg)
        : [WDTREG]  "n"  (_SFR_MEM_ADDR(_WD_CONTROL_REG)),
        [WDCE_WDE]  "n"  ((uint8_t)(_BV(_WD_CHANGE_BIT) | _BV(WDE)))
        : "r0"
    );
}
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
#if __clang__
    /* TODO: On LLVM allocation of an output register fails when the variable
     * is larger than the register */
    return 0;
#else
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
#endif
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
