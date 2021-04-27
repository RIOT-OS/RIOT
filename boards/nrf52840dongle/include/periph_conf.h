/*
 * Copyright (C) 2019 Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52840dongle
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the nRF52840-Dongle
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_clock_32_1.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"
#include "cfg_spi_default.h"

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
        .rx_pin     = GPIO_PIN(0, 13),
        .tx_pin     = GPIO_PIN(0, 15),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
};

#define UART_0_ISR          (isr_uart0)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   PWM configuration
 *
 * For the nRF52840-Dongle board, the PWM0 module is set to drive the LEDs LD1
 * and the channels LD2 red, green and blue in the four channels of PWM_DEV(0);
 * other PWM outputs are not configured.
 *
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        NRF_PWM0,
        {
            GPIO_PIN(0, 6),
            GPIO_PIN(0, 8),
            GPIO_PIN(1, 9),
            GPIO_PIN(0, 12),
        },
    },
};
#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
