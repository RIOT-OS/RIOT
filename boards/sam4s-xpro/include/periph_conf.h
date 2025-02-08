 /*
 * Copyright (C) 2025 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sam4s-xpro
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for SAM4S Xplained pro
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
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
/* targeted system core clock */
#define CLOCK_CORECLOCK     MHZ(120)
/* external oscillator clock */
#define CLOCK_EXT_OSC       MHZ(12)
/* define PLL configuration
 *
 * The values must fulfill this equation:
 * CORECLOCK = (EXT_OCS / PLL_DIV) * (PLL_MUL + 1)
 */
#define CLOCK_PLL_MUL       (9)
#define CLOCK_PLL_DIV       (1)

/* number of wait states before flash read and write operations */
#define CLOCK_FWS           (5)         /* 5 is safe for 120 MHz */
/** @} */

/**
 * @brief    Enable external oscillator for driving the slow clock
 */
#define CLOCK_SCLK_XTAL     (1)

/**
 * @name    Timer peripheral configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    { .dev = TC0, .id_ch0 = ID_TC0 },
    { .dev = TC1, .id_ch0 = ID_TC3 }
};

#define TIMER_0_ISR         isr_tc0
#define TIMER_1_ISR         isr_tc3

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev    = (Uart *)UART1,
        .rx_pin = GPIO_PIN(PB, 2),
        .tx_pin = GPIO_PIN(PB, 3),
        .mux    = GPIO_MUX_A,
        .pmc_id = ID_UART1,
        .irqn   = UART1_IRQn
    }
};

/* define interrupt vectors */
#define UART_0_ISR          isr_uart1

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
