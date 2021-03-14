/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2018 RWTH Aachen, Josua Arndt <jarndt@ias.rwth-aachen.de>
 *               2021 Gerson Fernando Budke <nandojve@gmail.com>
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
#include "cpu_conf.h"
#include "cpu_clock.h"
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
 * @brief   MCU ISR State
 *
 * Contents:
 *
 * The General Purpose I/O Register 1 (GPIOR1) is used to flag if system is
 * processing an ISR for ATmega devices.  It stores how deep system is
 * processing a nested interrupt.  ATxmega have three selectable interrupt
 * levels for any interrupt: low, medium and high and are controlled by PMIC.
 * ATmega requires that users re-enable interrupts after executing
 * @ref avr8_enter_isr method to enable nested IRQs in low priority interrupts.
 * ATxmega PMIC should be used to determine the current state of MCU ISR.
 *
 * If the system is running outside an interrupt, GPIOR1 will have always
 * value 0, on any configuration.  When one or more interrupt vectors are
 * activated, GPIOR1 will have a value greater than 0.  These operations are
 * performed by the pair @ref avr8_enter_isr and @ref avr8_exit_isr.
 *
 * An 3-level nested IRQ illustration can be visualized below:
 *
 *                           int-3
 *                              ↯
 *                              +----------+
 *                              | high lvl |
 *                 int-2        +----------+
 *                    ↯         |          |
 *                    +---------+          +---------+
 *                    | mid lvl |          | mid lvl |
 *       int-1        +---------+          +---------+
 *          ↯         |                              |
 *          +---------+                              +---------+
 *          | low lvl |                              | low lvl |
 *          +---------+                              +---------+ ↯ can switch
 *          |                                                  | context here
 * +--------+                                                  +--------+
 * | thread |                                                  | thread |
 * +--------+                                                  +--------+
 *
 * At @ref avr8_exit_isr scheduler may require a switch context.  That can be
 * performed in the following situation to avoid stack corruption:
 *  - ATmega: when GPIOR1 is equal to 0
 *  - ATxmega: when PMIC.STATUS indicate a low level IRQ
 */

/**
 * @brief   Flags TX pending for UART x
 */
#define AVR8_STATE_FLAG_UART_TX(x)    (0x01U << x)

/**
 * @brief   MCU UART TX pending state
 *
 * @note    The content must be changed using the pair
 *          @ref avr8_uart_tx_set_pending and @ref avr8_uart_tx_clear_pending
 *          methods.  The variable uses General Purpose IO Register 0 (GPIOR0)
 *          which allows atomic operations and don't require a critical
 *          section.
 *
 * Contents:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   7   6   5   4   3   2   1   0
 * +---+---+---+---+---+---+---+---+
 * |TX7|TX6|TX5|TX4|TX3|TX2|TX1|TX0|
 * +---+---+---+---+---+---+---+---+
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * | Label  | Description                                                   |
 * |:-------|:--------------------------------------------------------------|
 * | TX7    | This bit is set when on UART7 TX is pending                   |
 * | TX6    | This bit is set when on UART6 TX is pending                   |
 * | TX5    | This bit is set when on UART5 TX is pending                   |
 * | TX4    | This bit is set when on UART4 TX is pending                   |
 * | TX3    | This bit is set when on UART3 TX is pending                   |
 * | TX2    | This bit is set when on UART2 TX is pending                   |
 * | TX1    | This bit is set when on UART1 TX is pending                   |
 * | TX0    | This bit is set when on UART0 TX is pending                   |
 */

/**
 * @brief   Run this code on entering interrupt routines
 */
static inline void avr8_enter_isr(void)
{
    /* This flag is only called from IRQ context. The value will be handled
     * before ISR context is left by @ref avr8_exit_isr.
     */
#if !defined(CPU_ATXMEGA)
    ++GPIOR1;
#endif
}

/**
 * @brief   Check if TX on any present UART device is still pending
 *
 * @retval  !=0     At least on UART device is still sending data out
 * @retval  0       No UART is currently sending data
 */
static inline int avr8_is_uart_tx_pending(void)
{
    return GPIOR0;
}

/**
 * @brief        Set UART TX channel as pending
 *
 * General Purpose IO Register 0 (GPIOR0) allows RMW which eliminates critical
 * section.
 *
 * @param uart   The UART number
 */
static inline void avr8_uart_tx_set_pending(unsigned uart)
{
    GPIOR0 |= AVR8_STATE_FLAG_UART_TX(uart);
}

/**
 * @brief        Clear UART TX channel pending state
 *
 * General Purpose IO Register 0 (GPIOR0) allows RMW which eliminates critical
 * section.
 *
 * @param uart   The UART number
 */
static inline void avr8_uart_tx_clear_pending(unsigned uart)
{
    GPIOR0 &= ~AVR8_STATE_FLAG_UART_TX(uart);
}

/**
 * @brief Run this code on exiting interrupt routines
 */
void avr8_exit_isr(void);

/**
 * @brief Initialization of the CPU
 */
void cpu_init(void);

/**
 * @brief Initialization of the CPU clock
 */
void avr8_clk_init(void);

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
