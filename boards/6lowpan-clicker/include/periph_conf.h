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
 * @ingroup     boards_6lowpan-clicker
 * @{
 *
 * @file
 * @brief       peripheral configuration for the MikroE 6LoWPAN Clicker
 *
 * @author      Neil Jones <Neil.Jones@imgtec.com>
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
 *          Note 120MHz is the max F for this device.
 */
#define PERIPHERAL_CLOCK    MHZ(96)

#define CLOCK_CORECLOCK     MHZ(120)
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
    {   /* UART port available on MikroBus */
        .base       = (volatile unsigned int *)_UART3_BASE_ADDRESS,
        .clock      = PERIPHERAL_CLOCK,
        .rx_pin     = GPIO_PIN(PORT_F, 5),
        .tx_pin     = GPIO_PIN(PORT_F, 4),
        .rx_mux_reg = &U3RXR,
        .tx_mux_reg = &RPF4R,
        .rx_af      = GPIO_AF2,
        .tx_af      = GPIO_AF1,
        .vector     = _UART_3_VECTOR,
        .irq        = _UART3_RX_IRQ,
    },
};

#define UART_0_ISR          (isr_usart3)
#define UART_NUMOF          ((unsigned int)ARRAY_SIZE(uart_config))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
