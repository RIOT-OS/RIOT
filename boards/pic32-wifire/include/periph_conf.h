/*
 * Copyright(C) 2016,2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     boards_pic32-wifire
 * @{
 *
 * @file
 * @brief       peripheral configuration for the Digilent PIC32 WiFire
 *
 * @author       Neil Jones <Neil.Jones@imgtec.com>
 */
#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "cpu.h"
#include "macros/units.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configurations
 * @{
 */
/**
 * @brief   The peripheral clock is required for the UART Baud rate calculation
 *          It is configured by the 'config' registers (see pic32_config_settings.c)
 */
#define PERIPHERAL_CLOCK    MHZ(100)

#define CLOCK_CORECLOCK     MHZ(200)
/** @} */

/**
 * @name    Timer definitions
 * @{
 */
#define TIMER_NUMOF         (1)
#define TIMER_0_CHANNELS    (3)
/** @} */

/**
  * @name    UART Definitions
  * @{
  */
static const uart_conf_t uart_config[] = {
    {   /* Virtual COM port */
        .base       = (volatile unsigned int *)_UART4_BASE_ADDRESS,
        .clock      = PERIPHERAL_CLOCK,
        .rx_pin     = GPIO_PIN(PORT_F, 2),
        .tx_pin     = GPIO_PIN(PORT_F, 8),
        .rx_mux_reg = &U4RXR,
        .tx_mux_reg = &RPF8R,
        .rx_af      = GPIO_AF11,
        .tx_af      = GPIO_AF2,
        .vector     = _UART4_RX_VECTOR,
    },
};

#define UART_0_ISR          (isr_usart4)
#define UART_NUMOF          ((unsigned int)ARRAY_SIZE(uart_config))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
