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
 * @name    Flags for the current state of the ATmega MCU
 * @{
 */
#define ATMEGA_STATE_FLAG_ISR           (0x80U) /**< In ISR */
#define ATMEGA_STATE_FLAG_UART0_TX      (0x01U) /**< TX pending for UART 0 */
#define ATMEGA_STATE_FLAG_UART1_TX      (0x02U) /**< TX pending for UART 1 */
#define ATMEGA_STATE_FLAG_UART_TX(x)    (0x01U << x) /**< TX pending for UART x */
/** @} */

/**
 * @brief   Global variable containing the current state of the MCU
 *
 * @note    This variable is updated from IRQ context; access to it should
 *          be wrapped into @ref irq_disable and @ref irq_restore or
 *          @ref atmega_get_state should be used.
 *
 * Contents:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   7   6   5   4   3   2   1   0
 * +---+---+---+---+---+---+---+---+
 * |IRQ| unused            |TX1|TX0|
 * +---+---+---+---+---+---+---+---+
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * | Label  | Description                                                   |
 * |:-------|:--------------------------------------------------------------|
 * | IRQ    | This bit is set when in IRQ context                           |
 * | unused | This bits are currently not used                              |
 * | TX1    | This bit is set when on UART1 TX is pending                   |
 * | TX0    | This bit is set when on UART0 TX is pending                   |
 */
extern uint8_t atmega_state;

/**
 * @brief   Atomically read the state (@ref atmega_state)
 *
 * This function guarantees that the read is not optimized out, not reordered
 * and done atomically. This does not mean that by the time return value is
 * processed that it still reflects the value currently stored in
 * @ref atmega_state.
 *
 * Using ASM rather than C11 atomics has less overhead, as not every access to
 * the state has to be performed atomically: Those done from ISR will not be
 * interrupted (no support for nested interrupts) and barriers at the begin and
 * end of the ISRs make sure the access takes place before IRQ context is left.
 */
static inline uint8_t atmega_get_state(void)
{
    uint8_t state;
    __asm__ volatile(
        "lds   %[state], atmega_state       \n\t"
        : [state]   "=r" (state)
        :
        : "memory"

    );

    return state;
}

/**
 * @brief   Run this code on entering interrupt routines
 */
static inline void atmega_enter_isr(void)
{
    /* This flag is only called from IRQ context, and nested IRQs are not
     * supported as of now. The flag will be unset before the IRQ context is
     * left, so no need to use memory barriers or atomics here
     */
    atmega_state |= ATMEGA_STATE_FLAG_ISR;
}

/**
 * @brief   Check if TX on any present UART device is still pending
 *
 * @retval  !=0     At least on UART device is still sending data out
 * @retval  0       No UART is currently sending data
 */
static inline int atmega_is_uart_tx_pending(void)
{
    uint8_t state = atmega_get_state();
    return (state & (ATMEGA_STATE_FLAG_UART0_TX | ATMEGA_STATE_FLAG_UART1_TX));
}

/**
 * @brief Run this code on exiting interrupt routines
 */
void atmega_exit_isr(void);

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
