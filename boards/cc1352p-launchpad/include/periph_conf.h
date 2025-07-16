/*
 * SPDX-FileCopyrightText: 2020 Locha Inc
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         boards_cc1352p_launchpad
 * @{
 *
 * @file
 * @brief           Peripheral MCU configuration for TI CC1312 LaunchPad
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @author          Luis A. Ruiz <luisan00@hotmail.com>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @name    Clock configuration
* @{
*/
/* the main clock is fixed to 48MHZ */
#define CLOCK_CORECLOCK     (48000000U)
/** @} */

/**
* @name    Timer configuration
*
* General purpose timers (GPT[0-3]) are configured consecutively and in order
* (without gaps) starting from GPT0, i.e. if multiple timers are enabled.
*
* @{
*/
static const timer_conf_t timer_config[] = {
 {
     .cfg = GPT_CFG_16T,
     .chn = 2,
 },
 {
     .cfg = GPT_CFG_32T,
     .chn = 1,
 },
 {
     .cfg = GPT_CFG_16T,
     .chn = 2,
 },
 {
     .cfg = GPT_CFG_32T,
     .chn = 1,
 }
};

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 *
 * The used LAUNCHXL-CC1352P1 board has available a single UART device through
 * the debugger, so all we need to configure are the RX and TX pins.
 *
 * Optionally we can enable hardware flow control, by setting UART_HW_FLOW_CTRL
 * to 1 and defining pins for cts_pin and rts_pin.
 *
 * @{
 */

static const uart_conf_t uart_config[] = {
 {
     .regs = UART0,
     .tx_pin = 13,
     .rx_pin = 12,
#ifdef MODULE_PERIPH_UART_HW_FC
     .rts_pin = GPIO_UNDEF,
     .cts_pin = GPIO_UNDEF,
#endif
     .intn = UART0_IRQN
 },
 {
     .regs = UART1,
     .tx_pin = 26,
     .rx_pin = 25,
#ifdef MODULE_PERIPH_UART_HW_FC
     .rts_pin = GPIO_UNDEF,
     .cts_pin = GPIO_UNDEF,
#endif
     .intn = UART1_IRQN
  }
};
#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
#define I2C_NUMOF           (1)
#define I2C_SCL_PIN         (21)
#define I2C_SDA_PIN         (5)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
