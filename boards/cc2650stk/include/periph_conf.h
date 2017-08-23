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

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @name    UART configuration
 *
 * The used CC26x0 CPU only supports a single UART device, so all we need to
 * configure are the RX and TX pins.
 *
 * Optionally we can enable hardware flow control, by setting UART_HW_FLOW_CTRL
 * to 1 and defining pins for UART_CTS_PIN and UART_RTS_PIN.
 * @{
 */
#define UART_NUMOF          (1)
#define UART_RX_PIN         (28)
#define UART_TX_PIN         (29)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
