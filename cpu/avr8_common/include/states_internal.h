/*
 * Copyright (C) 2023 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_avr8_common
 * @{
 *
 * @file
 * @brief       States internal interface
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 */

#ifndef STATES_INTERNAL_H
#define STATES_INTERNAL_H

#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Internal flag which defines if uart state is stored on SRAM
 * @{
 */
#ifdef GPIOR0
#define AVR8_STATE_UART_USE_SRAM 0 /**< UART state using GPIOR registers. */
#else
#define AVR8_STATE_UART_USE_SRAM 1 /**< UART state using SRAM. */
#endif
/** @} */

/**
 * @name    UART TX pending state
 * @{
 *
 * @note    The content must be changed using the pair
 *          @ref avr8_uart_tx_set_pending and @ref avr8_uart_tx_clear_pending
 *          methods and the state is stored on @ref avr8_state_uart.
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
#if (AVR8_STATE_UART_USE_SRAM)
extern uint8_t avr8_state_uart_sram;            /**< UART state variable. */
#define avr8_state_uart avr8_state_uart_sram    /**< Definition for SRAM. */
#else
#define avr8_state_uart GPIOR0                  /**< Definition for GPIOR0. */
#endif
/** @} */

/**
 * @name    Internal flag which defines if IRQ state is stored on SRAM
 * @{
 */
#ifdef GPIOR1
#define AVR8_STATE_IRQ_USE_SRAM 0 /**< IRQ state using GPIOR registers. */
#else
#define AVR8_STATE_IRQ_USE_SRAM 1 /**< IRQ state using GPIOR registers. */
#endif
/** @} */

/**
 * @brief   MCU ISR State
 *
 * Contents:
 *
 * The `avr8_state_irq_count` is used to indicate that system is processing an
 * ISR for AVR-8 devices.  It stores how deep system is processing a nested
 * interrupt.  ATxmega have three selectable interrupt levels for any interrupt:
 * low, medium and high and are controlled by PMIC. ATmega requires that users
 * re-enable interrupts after executing the `_isr_handle` method to enable
 * nested IRQs in low priority interrupts. ATxmega PMIC is not enough to
 * determine the current state of MCU ISR in RIOT-OS because scheduler may
 * perform a context switch inside any IRQ.
 *
 * If the system is running outside an interrupt, `avr8_state_irq_count` will
 * have a 0 value.  When one or more interrupt vectors are activated,
 * `avr8_state_irq_count` will be incremented and have a value greater than 0.
 * These operations are performed by the pair @ref avr8_enter_isr and
 * @ref avr8_exit_isr.
 *
 * An 3-level nested IRQ illustration can be visualized below:
 *
 *                             int-3
 *                                ↯
 *                                +----------+
 *                                | high lvl |
 *                   int-2        +----------+
 *                      ↯         |          |
 *                      +---------+          +---------+
 *                      | mid lvl |          | mid lvl |
 *         int-1        +---------+          +---------+
 *            ↯         |                              |
 *            +---------+                              +---------+
 *            | low lvl |                              | low lvl |
 *            +---------+                              +---------+ ↯ can switch
 *            |                                                  | context here
 * +----------+                                                  +----------+
 * | thread A |                                                  | thread B |
 * +----------+                                                  +----------+
 *
 * At @ref avr8_exit_isr scheduler may require a switch context.  That can be
 * performed only when `avr8_state_irq_count` is equal to zero. This is
 * necessary to avoid stack corruption and since system is processing interrupts
 * since priority inheritance happened.
 *
 * The AVR-8 allow nested IRQ and MCUs cores perform with different behaviour, a
 * custom IRQ management was developed.  The code is simple and must use the
 * following skeleton:
 *
 * The `_isr_handle` method represents the body of an ISR routine.  This is the
 * code that handles the IRQ itself.  It can be a shared function and don't
 * need any special care.
 *
 * The IRQ method `_isr_handle` should have minimal parameters.  It is
 * recommended up to 2 and convention is to use the first parameter as the
 * instance and the second parameter usually represent the unit inside that
 * instance.
 *
 * Example: UART uses 1 parameter and Timers, usually 2.
 *
 * static inline void _uart_isr_handler(instance)
 * {
 *     ...
 * }
 * AVR8_ISR(UART_0_RXC_ISR, _uart_isr_handler, 0);
 *
 * static inline void _tmr_isr_handler(instance, channel)
 * {
 *     ...
 * }
 * AVR8_ISR(TIMER_0_ISRA, _tmr_isr_handler, 0, 0);
 * AVR8_ISR(TIMER_0_ISRB, _tmr_isr_handler, 0, 1);
 */
#if (AVR8_STATE_IRQ_USE_SRAM)
extern uint8_t avr8_state_irq_count_sram;               /**< IRQ state variable. */
#define avr8_state_irq_count avr8_state_irq_count_sram  /**< Definition for SRAM. */
#else
#define avr8_state_irq_count GPIOR1                     /**< Definition for GPIOR1. */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* STATES_INTERNAL_H */
/** @} */
