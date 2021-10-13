/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_cc2650stk
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the CC2650STK board
 *
 * @author      Leon M. George <leon@georgemail.eu>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

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
 * The used CC26x0 CPU only supports a single UART device, so all we need to
 * configure are the RX and TX pins.
 *
 * Optionally we can enable hardware flow control, by setting flow_control
 * to 1 and defining pins for cts_pin and rts_pin.
 * @{
 */

static const uart_conf_t uart_config[] = {
    {
        .regs = UART0,
        .tx_pin = 29,
        .rx_pin = 28,
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
#define I2C_SDA_PIN         (5)
#define I2C_SCL_PIN         (6)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
