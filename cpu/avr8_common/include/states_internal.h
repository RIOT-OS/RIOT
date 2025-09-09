/*
 * SPDX-FileCopyrightText: 2023 Gerson Fernando Budke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
 * @name    Global variable containing the current state of the MCU
 * @{
 *
 * @note    This variable is updated from IRQ context; access to it should
 *          be wrapped into @ref irq_disable and @ref irq_restore should be
 *          used.
 *
 * Contents:
 *
 * | Label  | Description                                                   |
 * |:-------|:--------------------------------------------------------------|
 * | IRQ    | This variable is incremented when in IRQ context              |
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

/** @} */
