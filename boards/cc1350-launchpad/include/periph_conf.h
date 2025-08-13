/*
 * SPDX-FileCopyrightText: 2021 Jean Pierre Dudey
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         boards_cc1350_launchpad
 * @{
 *
 * @file
 * @brief           Peripheral MCU configuration for TI CC1350 LaunchPad
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#include "periph_cpu.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @name    Clock configuration
* @{
*/
/* the main clock is fixed to 48MHZ */
#define CLOCK_CORECLOCK     MHZ(48)
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
* The used CC26x0 CPU only supports a single UART device, so all we need to
* configure are the RX and TX pins.
*
* Optionally we can enable hardware flow control, by using periph_uart_hw_fc
* module (USEMODULE += periph_uart_hw_fc) and defining pins for cts_pin and
* rts_pin.
* @{
*/

static const uart_conf_t uart_config[] = {
    {
        .regs = UART0,
        .tx_pin = GPIO_PIN(0, 3),
        .rx_pin = GPIO_PIN(0, 2),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin = GPIO_UNDEF,
        .cts_pin = GPIO_UNDEF,
#endif
        .intn = UART0_IRQN
    }
};
#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
#define I2C_NUMOF           (1)
#define I2C_SDA_PIN         GPIO_PIN(0, 5)
#define I2C_SCL_PIN         GPIO_PIN(0, 4)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
