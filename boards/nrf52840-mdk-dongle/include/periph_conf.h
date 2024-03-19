/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52840-mdk-dongle
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the nRF52840 MDK USB Dongle
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_clock_32_1.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 *
 * This board does not have explicit UART pins. These are set as UART 0 to
 * provide an easy serial debug port when not using (or debugging) USB.
 *
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(0, 6),
        .tx_pin     = GPIO_PIN(0, 7),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
};

#define UART_0_ISR          isr_uart0

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   PWM configuration
 *
 * For the nRF52840-mdk-dongle board, the PWM0 module is set to drive the RGB LED.
 * Other PWM outputs are not configured.
 *
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    { NRF_PWM0, { GPIO_PIN(0, 23), GPIO_PIN(0, 22), GPIO_PIN(0, 24) } }
};
#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
